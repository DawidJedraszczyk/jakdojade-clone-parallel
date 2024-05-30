#include <iostream>
#include "algorithm.h"

int main() {
    try {
        pqxx::connection conn("dbname=ebus2 user=dawid password=Dragon11 host=localhost port=5432");
        if (conn.is_open()) {
            std::cout << "Connected to database successfully!" << std::endl;
        } else {
            std::cerr << "Failed to connect to database!" << std::endl;
            return 1;
        }

        std::string start_location = "Gorzycka 110, Ostrów Wielkopolski";  
        std::string goal_location = "Centrum przesiadkowe, Ostrów Wielkopolski";  
        std::string date = "2024-05-30";                   
        std::string time = "12:00";                        

        auto solutions = find_route_without_changing_bus(conn, start_location, goal_location, date, time);

        for (const auto& solution : solutions) {
            std::cout << "Bus line: " << solution.bus_line << ", Departure time: " << solution.departure_time << ", Arrival time: " << solution.arrival_time << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
