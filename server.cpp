#include "crow.h"
#include <pqxx/pqxx>
#include <vector>
#include <string>
#include <iostream>

struct User {
    int id;
    std::string name;
    int age;

    crow::json::wvalue to_json() const {
        crow::json::wvalue json_obj;
        json_obj["id"] = id;
        json_obj["name"] = name;
        json_obj["age"] = age;
        return json_obj;
    }
};

void init_db() {
    try {
        pqxx::connection conn("dbname=mydb user=myuser password=mypassword host=localhost port=5432");
        pqxx::work tx(conn);
        
        tx.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id SERIAL PRIMARY KEY,
                name TEXT NOT NULL,
                age INT NOT NULL
            );
        )");
        tx.commit();
        std::cout << "Database initialized successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "DB Init Error: " << e.what() << "\n";
    }
}

int main() {
    init_db();
    crow::SimpleApp app;
    
    std::string conn_str = "dbname=mydb user=myuser password=mypassword host=localhost port=5432";

    // 1. READ ALL (GET http://localhost:8080/users)
    CROW_ROUTE(app, "/users")([&conn_str]() {
        try {
            pqxx::connection conn(conn_str);
            pqxx::nontransaction tx(conn);
            
            pqxx::result res = tx.exec("SELECT id, name, age FROM users ORDER BY id ASC");
            
            std::vector<crow::json::wvalue> json_list;
            for (auto row : res) {
                User u{ row["id"].as<int>(), row["name"].as<std::string>(), row["age"].as<int>() };
                json_list.push_back(u.to_json());
            }
            return crow::response(crow::json::wvalue(json_list));
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });

        // 2. CREATE (POST http://localhost:8080/users)
    CROW_ROUTE(app, "/users").methods(crow::HTTPMethod::POST)([&conn_str](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name") || !body.has("age")) 
            return crow::response(400, "Invalid JSON or missing fields");

        try {
            // Extract explicitly into primitive C++ types to satisfy libpqxx templates
            std::string name = body["name"].s();
            int age = body["age"].i();

            pqxx::connection conn(conn_str);
            pqxx::work tx(conn);
            
            tx.exec_params("INSERT INTO users (name, age) VALUES ($1, $2)", name, age);
            tx.commit();
            
            return crow::response(201, "User created in Postgres");
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });

    // 3. UPDATE (PUT http://localhost:8080/users/<int>)
    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::PUT)([&conn_str](const crow::request& req, int id) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("name") || !body.has("age")) 
            return crow::response(400, "Invalid JSON or missing fields");

        try {
            // Extract explicitly into primitive C++ types
            std::string name = body["name"].s();
            int age = body["age"].i();

            pqxx::connection conn(conn_str);
            pqxx::work tx(conn);
            
            pqxx::result res = tx.exec_params("UPDATE users SET name=$1, age=$2 WHERE id=$3", 
                                              name, age, id);
            tx.commit();

            if (res.affected_rows() == 0) return crow::response(404, "User not found");
            return crow::response(200, "User updated");
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });


    // 4. DELETE (DELETE http://localhost:8080/users/<int>)
    CROW_ROUTE(app, "/users/<int>").methods(crow::HTTPMethod::DELETE)([&conn_str](int id) {
        try {
            pqxx::connection conn(conn_str);
            pqxx::work tx(conn);
            
            pqxx::result res = tx.exec_params("DELETE FROM users WHERE id=$1", id);
            tx.commit();

            if (res.affected_rows() == 0) return crow::response(404, "User not found");
            return crow::response(200, "User deleted");
        } catch (const std::exception& e) {
            return crow::response(500, e.what());
        }
    });

    app.port(8080).multithreaded().run();
}

