#include <iostream>
#include <pqxx/pqxx>
#include "database_queries.h"

void execute_query(pqxx::connection &conn, const std::string &query) {
    pqxx::work txn(conn);
    pqxx::result result = txn.exec(query);

    for (auto row : result) {
        for (auto field : row) {
            std::cout << field.c_str() << " ";
        }
        std::cout << std::endl;
    }

    txn.commit();
}

void execute_queries(pqxx::connection &conn) {
    execute_query(conn, "SELECT * FROM route_search_busline");
    execute_query(conn, "SELECT * FROM route_search_busstop");
    execute_query(conn, "SELECT * FROM route_search_busstopinbusline");
    execute_query(conn, "SELECT * FROM route_search_busdeparture");
}
