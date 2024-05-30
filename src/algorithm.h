#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>
#include <vector>
#include <pqxx/pqxx>

// Define the Solution struct if not already defined
struct Solution {
    std::string bus_line;
    std::string departure_time;
    std::string arrival_time;
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

#endif // ALGORITHM_H
