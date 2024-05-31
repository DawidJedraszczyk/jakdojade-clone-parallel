#include <iostream>
#include <pqxx/pqxx>
#include "sequence.h"
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <variant>
#include <set>
#include <algorithm>
#include <map>

// Function to encode URL
std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            // Any other characters are percent-encoded
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
        }
    }

    return escaped.str();
}

// Function to categorize the date into "Roboczy", "Sobota", or "Niedziela i święta"
std::string categorize_date(const std::string &date_str) {
    std::tm date = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&date, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date");
    }

    int weekday = date.tm_wday;
    if (weekday >= 1 && weekday <= 5) {
        return "Roboczy";
    } else if (weekday == 6) {
        return "Sobota";
    } else {
        return "Niedziela i święta";
    }
}

// Callback function for cURL write
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to get coordinates from an address using Nominatim API
Coordinates getCoordinates(const std::string &address) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    Coordinates coords = {0.0, 0.0}; // Initialize with default values

    std::string encoded_address = url_encode(address);
    std::string url = "https://nominatim.openstreetmap.org/search?q=" + encoded_address + "&format=json&limit=1";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "YourAppName/1.0 (your.email@example.com)"); // Set custom user agent

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            return coords; // Return default coordinates
        }

        if (readBuffer.empty()) {
            std::cerr << "Empty response from geocoding API" << std::endl;
            return coords; // Return default coordinates
        }

        try {
            auto jsonResponse = nlohmann::json::parse(readBuffer);
            if (!jsonResponse.empty()) {
                auto location = jsonResponse[0];
                coords.latitude = std::stod(location["lat"].get<std::string>());
                coords.longitude = std::stod(location["lon"].get<std::string>());
            } else {
                std::cerr << "Geocoding failed: No results found." << std::endl;
            }
        } catch (const nlohmann::json::parse_error &e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return coords;
}

// Function to calculate the haversine distance between two coordinates
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371e3; // Earth radius in meters
    double phi1 = lat1 * M_PI / 180.0;
    double phi2 = lat2 * M_PI / 180.0;
    double delta_phi = (lat2 - lat1) * M_PI / 180.0;
    double delta_lambda = (lon2 - lon1) * M_PI / 180.0;

    double a = std::sin(delta_phi / 2) * std::sin(delta_phi / 2) +
               std::cos(phi1) * std::cos(phi2) *
               std::sin(delta_lambda / 2) * std::sin(delta_lambda / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return R * c;
}

// Function to get the nearest bus stops from a given location
std::vector<BusStop> get_nearest_stops(pqxx::connection &conn, double latitude, double longitude, int size_of_response) {
    pqxx::work txn(conn);
    pqxx::result result = txn.exec("SELECT id, name, latitude, longitude FROM route_search_busstop");

    std::vector<BusStop> bus_stops;
    for (auto row : result) {
        if (row["latitude"].is_null() || row["longitude"].is_null()) {
            continue; // Skip rows with null latitude or longitude
        }

        BusStop stop;
        stop.id = row["id"].c_str();  // Add the id field
        stop.name = row["name"].c_str();
        stop.latitude = row["latitude"].as<double>();
        stop.longitude = row["longitude"].as<double>();
        stop.distance = haversine(latitude, longitude, stop.latitude, stop.longitude);
        bus_stops.push_back(stop);
    }

    std::sort(bus_stops.begin(), bus_stops.end(), [](const BusStop &a, const BusStop &b) {
        return a.distance < b.distance;
    });

    if (bus_stops.size() > size_of_response) {
        bus_stops.resize(size_of_response);
    }

    return bus_stops;
}

std::vector<Solution> find_route_without_changing_bus(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time) {
    std::vector<Solution> solutions;
    std::map<std::pair<std::string, std::string>, Solution> earliest_solutions;
    std::string day_type = categorize_date(date);

    Coordinates start_coords = getCoordinates(start_location);
    Coordinates goal_coords = getCoordinates(goal_location);

    std::vector<BusStop> nearest_start_stops = get_nearest_stops(conn, start_coords.latitude, start_coords.longitude, 10);
    std::vector<BusStop> nearest_goal_stops = get_nearest_stops(conn, goal_coords.latitude, goal_coords.longitude, 10);

    for (const auto &start_stop : nearest_start_stops) {
        pqxx::work txn(conn);
        std::string query = "SELECT bl.name, bl.direction, bd1.time AS departure_time, bd2.time AS arrival_time, bd2.bus_stop_id AS goal_stop_id, "
                            "bs1.ordinal_number AS start_ordinal, bs2.ordinal_number AS goal_ordinal "
                            "FROM route_search_busline bl "
                            "JOIN route_search_busdeparture bd1 ON bl.id = bd1.bus_line_id "
                            "JOIN route_search_busdeparture bd2 ON bl.id = bd2.bus_line_id "
                            "JOIN route_search_busstopinbusline bs1 ON bl.id = bs1.bus_line_id AND bd1.bus_stop_id = bs1.bus_stop_id "
                            "JOIN route_search_busstopinbusline bs2 ON bl.id = bs2.bus_line_id AND bd2.bus_stop_id = bs2.bus_stop_id "
                            "WHERE bd1.bus_stop_id = " + txn.quote(start_stop.id) + " "
                            "AND bd1.time >= " + txn.quote(time) + " "
                            "AND bd1.departure_ordinal_number = bd2.departure_ordinal_number "
                            "AND bd1.route_day = " + txn.quote(day_type) + " "
                            "ORDER BY bd1.time";
        pqxx::result result = txn.exec(query);

        for (auto row : result) {
            std::string bus_line = row["name"].c_str();
            std::string direction = row["direction"].c_str();
            std::string departure_time = row["departure_time"].c_str();
            std::string arrival_time = row["arrival_time"].c_str();
            std::string goal_stop_id = row["goal_stop_id"].c_str();
            int start_ordinal = row["start_ordinal"].as<int>();
            int goal_ordinal = row["goal_ordinal"].as<int>();

            if (start_ordinal < goal_ordinal) {
                for (const auto &goal_stop : nearest_goal_stops) {
                    if (goal_stop.id == goal_stop_id) {
                        Solution sol;
                        sol.bus_line = bus_line;
                        sol.direction = direction;
                        sol.departure_time = departure_time;
                        sol.arrival_time = arrival_time;
                        sol.start_stop = start_stop.name;
                        sol.goal_stop = goal_stop.name;

                        auto key = std::make_pair(bus_line, direction);
                        if (earliest_solutions.find(key) == earliest_solutions.end() || sol.departure_time < earliest_solutions[key].departure_time) {
                            earliest_solutions[key] = sol;
                        }
                    }
                }
            }
        }
    }

    for (const auto &entry : earliest_solutions) {
        solutions.push_back(entry.second);
    }

    return solutions;
}



std::vector<std::variant<Solution, SolutionTwoBuses>> find_route_with_changing_bus(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time, const std::set<std::string> &used_buses) {
    std::vector<std::variant<Solution, SolutionTwoBuses>> solutions;
    std::map<std::pair<std::string, std::string>, SolutionTwoBuses> earliest_solutions;
    std::string day_type = categorize_date(date);

    Coordinates start_coords = getCoordinates(start_location);
    Coordinates goal_coords = getCoordinates(goal_location);

    std::cout << "Start Coordinates: Latitude = " << start_coords.latitude << ", Longitude = " << start_coords.longitude << std::endl;
    std::cout << "Goal Coordinates: Latitude = " << goal_coords.latitude << ", Longitude = " << goal_coords.longitude << std::endl;

    std::vector<BusStop> nearest_start_stops = get_nearest_stops(conn, start_coords.latitude, start_coords.longitude, 10);
    std::vector<BusStop> nearest_goal_stops = get_nearest_stops(conn, goal_coords.latitude, goal_coords.longitude, 10);
    std::set<std::pair<std::string, std::string>> first_bus_list;
    
    for (const auto &start_stop : nearest_start_stops) {
        pqxx::work txn(conn);
        std::string query = "SELECT bl.name, bl.direction, bd1.time AS departure_time, bd2.time AS arrival_time, bd2.bus_stop_id AS second_stop_id, "
                            "bs1.ordinal_number AS start_ordinal, bs2.ordinal_number AS goal_ordinal "
                            "FROM route_search_busline bl "
                            "JOIN route_search_busdeparture bd1 ON bl.id = bd1.bus_line_id "
                            "JOIN route_search_busdeparture bd2 ON bl.id = bd2.bus_line_id "
                            "JOIN route_search_busstopinbusline bs1 ON bl.id = bs1.bus_line_id AND bd1.bus_stop_id = bs1.bus_stop_id "
                            "JOIN route_search_busstopinbusline bs2 ON bl.id = bs2.bus_line_id AND bd2.bus_stop_id = bs2.bus_stop_id "
                            "WHERE bd1.bus_stop_id = " + txn.quote(start_stop.id) + " "
                            "AND bd1.time >= " + txn.quote(time) + " "
                            "AND bd1.departure_ordinal_number = bd2.departure_ordinal_number "
                            "AND bd1.route_day = " + txn.quote(day_type) + " "
                            "ORDER BY bd1.time";
        pqxx::result result = txn.exec(query);

        for (auto row : result) {
            std::string bus_line = row["name"].c_str();
            std::string direction = row["direction"].c_str();
            std::string departure_time = row["departure_time"].c_str();
            std::string arrival_time = row["arrival_time"].c_str();
            std::string second_stop_id = row["second_stop_id"].c_str();
            int start_ordinal = row["start_ordinal"].as<int>();
            int goal_ordinal = row["goal_ordinal"].as<int>();
            bool goal_station = false;
            first_bus_list.insert({bus_line, direction});

            if (start_ordinal < goal_ordinal && used_buses.find(bus_line) == used_buses.end()) {
                for (const auto &goal_stop : nearest_goal_stops) {
                    if (goal_stop.id == second_stop_id) {
                        goal_station = true;
                        SolutionTwoBuses solTwoBuses;
                        solTwoBuses.bus_line = bus_line;
                        solTwoBuses.direction = direction;
                        solTwoBuses.departure_time = departure_time;
                        solTwoBuses.arrival_time = arrival_time;
                        solTwoBuses.start_stop = start_stop.name;
                        solTwoBuses.goal_stop = goal_stop.name;

                        auto key = std::make_pair(bus_line, direction);
                        if (earliest_solutions.find(key) == earliest_solutions.end() || solTwoBuses.departure_time < earliest_solutions[key].departure_time) {
                            earliest_solutions[key] = solTwoBuses;
                        }
                    }
                }

                if (!goal_station) {
                    std::string query_second_bus = "SELECT bl.name, bl.direction, bd1.time AS departure_time, bd2.time AS arrival_time, bd2.bus_stop_id AS second_stop_id, "
                        "bs1.ordinal_number AS start_ordinal, bs2.ordinal_number AS goal_ordinal "
                        "FROM route_search_busline bl "
                        "JOIN route_search_busdeparture bd1 ON bl.id = bd1.bus_line_id "
                        "JOIN route_search_busdeparture bd2 ON bl.id = bd2.bus_line_id "
                        "JOIN route_search_busstopinbusline bs1 ON bl.id = bs1.bus_line_id AND bd1.bus_stop_id = bs1.bus_stop_id "
                        "JOIN route_search_busstopinbusline bs2 ON bl.id = bs2.bus_line_id AND bd2.bus_stop_id = bs2.bus_stop_id "
                        "WHERE bd1.bus_stop_id = " + txn.quote(second_stop_id) + " "
                        "AND bl.name != " + txn.quote(bus_line) + " "
                        "AND bd1.time >= " + txn.quote(arrival_time) + " "
                        "AND bd1.departure_ordinal_number = bd2.departure_ordinal_number "
                        "AND bd1.route_day = " + txn.quote(day_type) + " "
                        "ORDER BY bd1.time";
                    pqxx::result result_second_bus = txn.exec(query_second_bus);

                    for (auto row : result_second_bus) {
                        std::string second_bus_line = row["name"].c_str();
                        std::string second_direction = row["direction"].c_str();
                        std::string second_departure_time = row["departure_time"].c_str();
                        std::string second_arrival_time = row["arrival_time"].c_str();
                        std::string third_stop_id = row["second_stop_id"].c_str();
                        int second_start_ordinal = row["start_ordinal"].as<int>();
                        int second_goal_ordinal = row["goal_ordinal"].as<int>();
                        bool second_goal_station = false;

                        if (first_bus_list.find({second_bus_line, second_direction}) == first_bus_list.end() && used_buses.find(second_bus_line) == used_buses.end()) {
                            for (const auto &goal_stop : nearest_goal_stops) {
                                if (goal_stop.id == third_stop_id) {
                                    second_goal_station = true;

                                    SolutionTwoBuses solTwoBuses;
                                    solTwoBuses.bus_line = bus_line;
                                    solTwoBuses.direction = direction;
                                    solTwoBuses.departure_time = departure_time;
                                    solTwoBuses.arrival_time = arrival_time;
                                    solTwoBuses.start_stop = start_stop.name;
                                    solTwoBuses.goal_stop = second_stop_id;

                                    solTwoBuses.second_bus_line = second_bus_line;
                                    solTwoBuses.second_departure_time = second_departure_time;
                                    solTwoBuses.second_arrival_time = second_arrival_time;
                                    solTwoBuses.second_start_stop = second_stop_id;
                                    solTwoBuses.second_goal_stop = goal_stop.name;
                                    solTwoBuses.second_direction = second_direction;

                                    auto second_key = std::make_pair(second_bus_line, second_direction);
                                    if (earliest_solutions.find(second_key) == earliest_solutions.end() || solTwoBuses.second_departure_time < earliest_solutions[second_key].second_departure_time) {
                                        earliest_solutions[second_key] = solTwoBuses;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (const auto &entry : earliest_solutions) {
        solutions.push_back(entry.second);
    }

    return solutions;
}

std::vector<std::variant<Solution, SolutionTwoBuses>> find_routes(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time) {
    std::vector<Solution> solutions_without_changing_bus = find_route_without_changing_bus(conn, start_location, goal_location, date, time);

    // Collect used bus lines
    std::set<std::string> used_buses;
    for (const auto &sol : solutions_without_changing_bus) {
        used_buses.insert(sol.bus_line);
    }

    std::vector<std::variant<Solution, SolutionTwoBuses>> all_solutions;
    all_solutions.insert(all_solutions.end(), solutions_without_changing_bus.begin(), solutions_without_changing_bus.end());

    std::vector<std::variant<Solution, SolutionTwoBuses>> solutions_with_changing_bus = find_route_with_changing_bus(conn, start_location, goal_location, date, time, used_buses);
    all_solutions.insert(all_solutions.end(), solutions_with_changing_bus.begin(), solutions_with_changing_bus.end());

    return all_solutions;
}
