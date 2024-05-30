#ifndef DATABASE_QUERIES_H
#define DATABASE_QUERIES_H

#include <pqxx/pqxx>

void execute_queries(pqxx::connection &conn);

#endif
