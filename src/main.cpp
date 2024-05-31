#include <iostream>
#include <chrono>
#include "sequence.h"
#include "openmp.h"
#include <iostream>
#include <pqxx/pqxx> // Include libpqxx headers
#include <vector>
#include <variant>
#include <string>
#include <fstream> // Include the fstream header

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
        std::string goal_location = "Piaski Szczygliczka, Ostrów Wielkopolski";  
        std::string date = "2024-05-30";                   
        std::string time = "12:00";                        
        auto start_time = std::chrono::high_resolution_clock::now();

        Coordinates start_coords = getCoordinates(start_location);
        Coordinates goal_coords = getCoordinates(goal_location);


        //auto solutions = find_routes(conn, start_location, goal_location, date, time);

        auto solutions = find_routes_openmp(conn, start_location, goal_location, date, time, start_coords, goal_coords);

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = end_time - start_time;

        std::cout << "Execution time for squence algorithm: " << elapsed_time.count() << " seconds" << std::endl;


        std::ofstream file("solutions_openmp.txt");
        if (!file) {
            std::cerr << "Error opening file for writing." << std::endl;
            return 1;
        }

        file << "Elapsed Time: " << elapsed_time.count() << " seconds" << std::endl;

       
        for (const auto &solution : solutions) {
            std::visit([&file](auto&& arg) { 
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Solution>) {
                    file << "Bus Line: " << arg.bus_line
                        << ", Direction: " << arg.direction
                        << ", Departure Time: " << arg.departure_time
                        << ", Arrival Time: " << arg.arrival_time
                        << ", Start Stop: " << arg.start_stop
                        << ", Goal Stop: " << arg.goal_stop << std::endl;
                } else if constexpr (std::is_same_v<T, SolutionTwoBuses>) {
                    file << "Bus Line: " << arg.bus_line
                        << ", Direction: " << arg.direction
                        << ", Departure Time: " << arg.departure_time
                        << ", Arrival Time: " << arg.arrival_time
                        << ", Start Stop: " << arg.start_stop
                        << ", Goal Stop: " << arg.goal_stop << std::endl;
                    file << "Second Bus Line: " << arg.second_bus_line
                        << ", Second Direction: " << arg.second_direction
                        << ", Second Departure Time: " << arg.second_departure_time
                        << ", Second Arrival Time: " << arg.second_arrival_time
                        << ", Second Start Stop: " << arg.second_start_stop
                        << ", Second Goal Stop: " << arg.second_goal_stop << std::endl;
                }
                file << std::endl;
            }, solution);
        }

        file.close();




    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
