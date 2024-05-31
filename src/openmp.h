#ifndef OPENMP_H
#define OPENMP_H

#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "sequence.h"
#define NUM_THREADS 8


std::string categorize_date_openmp(const std::string& date_str);
Coordinates getCoordinates_openmp(const std::string& address);
std::vector<BusStop> get_nearest_stops_openmp(pqxx::connection &conn, double latitude, double longitude, int size_of_response);
std::vector<Solution> find_route_without_changing_bus_openmp(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time, Coordinates start_coords, Coordinates goal_coords);
std::vector<std::variant<Solution, SolutionTwoBuses>> find_route_with_changing_bus_openmp(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time, Coordinates start_coords, Coordinates goal_coords);
std::vector<std::variant<Solution, SolutionTwoBuses>> find_routes_openmp(pqxx::connection &conn, const std::string &start_location, const std::string &goal_location, const std::string &date, const std::string &time, Coordinates start_coords, Coordinates goal_coords);
#endif // OPENMP_H
