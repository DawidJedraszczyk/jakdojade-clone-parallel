#include <iostream>
#include <pqxx/pqxx>
#include "algorithm.h"
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

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

std::string categorize_date(const std::string& date_str) {
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

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Coordinates getCoordinates(const std::string& address)
{
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    Coordinates coords = {0.0, 0.0}; // Initialize with default values

    std::string encoded_address = url_encode(address);
    std::string url = "https://nominatim.openstreetmap.org/search?q=" + encoded_address + "&format=json&limit=1";

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "YourAppName/1.0 (your.email@example.com)"); // Set custom user agent

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
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
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return coords;
}

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
    std::string day_type = categorize_date(date);

    Coordinates start_coords = getCoordinates(start_location);
    Coordinates goal_coords = getCoordinates(goal_location);

    std::cout << "Start Coordinates: Latitude = " << start_coords.latitude << ", Longitude = " << start_coords.longitude << std::endl;
    std::cout << "Goal Coordinates: Latitude = " << goal_coords.latitude << ", Longitude = " << goal_coords.longitude << std::endl;

    std::vector<BusStop> nearest_start_stops = get_nearest_stops(conn, start_coords.latitude, start_coords.longitude, 5);
    std::vector<BusStop> nearest_goal_stops = get_nearest_stops(conn, goal_coords.latitude, goal_coords.longitude, 5);

    std::cout << "Nearest Start Stops:" << std::endl;
    for (const auto &stop : nearest_start_stops) {
        std::cout << stop.name << " (" << stop.latitude << ", " << stop.longitude << "), Distance: " << stop.distance << " meters" << std::endl;
    }

    std::cout << "Nearest Goal Stops:" << std::endl;
    for (const auto &stop : nearest_goal_stops) {
        std::cout << stop.name << " (" << stop.latitude << ", " << stop.longitude << "), Distance: " << stop.distance << " meters" << std::endl;
    }

    // Check for buses that have departures from the nearest start stops
    for (const auto &start_stop : nearest_start_stops) {
        pqxx::work txn(conn);
        std::string query = "SELECT bl.name, bd1.time AS departure_time, bd2.time AS arrival_time, bd2.bus_stop_id AS goal_stop_id "
                            "FROM route_search_busline bl "
                            "JOIN route_search_busdeparture bd1 ON bl.id = bd1.bus_line_id "
                            "JOIN route_search_busdeparture bd2 ON bl.id = bd2.bus_line_id "
                            "WHERE bd1.bus_stop_id = " + txn.quote(start_stop.id) + " "
                            "AND bd1.time >= " + txn.quote(time) + " "
                            "AND bd1.departure_ordinal_number = bd2.departure_ordinal_number "
                            "AND bd1.route_day = " + txn.quote(day_type) + " "
                            "ORDER BY bd1.time";
        pqxx::result result = txn.exec(query);

        for (auto row : result) {
            std::string bus_line = row["name"].c_str();
            std::string departure_time = row["departure_time"].c_str();
            std::string arrival_time = row["arrival_time"].c_str();
            std::string goal_stop_id = row["goal_stop_id"].c_str();

            for (const auto &goal_stop : nearest_goal_stops) {
                if (goal_stop.id == goal_stop_id) {  // Compare using id instead of name
                    Solution sol;
                    sol.bus_line = bus_line;
                    sol.departure_time = departure_time;
                    sol.arrival_time = arrival_time;
                    solutions.push_back(sol);
                }
            }
        }
    }

    return solutions;
}
