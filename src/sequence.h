#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <string>
#include <vector>
#include <pqxx/pqxx>

// Define the Solution struct if not already defined
struct Solution {
    std::string bus_line;
    std::string departure_time;
    std::string arrival_time;
    std::string start_stop;
    std::string goal_stop;
    std::string direction;
};

struct SolutionTwoBuses {
    std::string bus_line;
    std::string departure_time;
    std::string arrival_time;
    std::string start_stop;
    std::string goal_stop;
    std::string direction;
    std::string second_bus_line;
    std::string second_departure_time;
    std::string second_arrival_time;
    std::string second_start_stop;
    std::string second_goal_stop;
    std::string second_direction;
};

// Define the Coordinates struct
struct Coordinates {
    double latitude;
    double longitude;
};

// Define the BusStop struct
struct BusStop {
    std::string id; 
    std::string name;
    double latitude;
    double longitude;
    double distance; 
};

std::string categorize_date(const std::string& date_str);
Coordinates getCoordinates(const std::string& address);
std::vector<BusStop> get_nearest_stops(pqxx::connection &conn, double latitude, double longitude, int size_of_response);
std::vector<Solution> find_route_without_changing_bus(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time);
std::vector<std::variant<Solution, SolutionTwoBuses>> find_route_with_changing_bus(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time);
std::vector<std::variant<Solution, SolutionTwoBuses>> find_routes(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time);
#endif // SEQUENCE_H
