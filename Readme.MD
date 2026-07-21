# C++ Crow CRUD Demo with PostgreSQL

A minimalist, high-performance CRUD REST API built using the **Crow** microframework and **libpqxx** for PostgreSQL connectivity. 

This project was built as an experimental playground to explore backend web development in native C++ and compare its efficiency, compilation footprint, and execution performance directly against traditional Java frameworks like Spring Boot.

## Prerequisites & Setup

### 1. Run PostgreSQL Database
Launch an isolated PostgreSQL instance inside Docker Desktop:
```bash
docker run --name my_postgres \
  -e POSTGRES_USER=myuser \
  -e POSTGRES_PASSWORD=mypassword \
  -e POSTGRES_DB=mydb \
  -p 5432:5432 \
  -d postgres:16
```

### 2. Compile the Server
Compile the native C++ executable using `g++`, linking the required threading and database driver libraries:
```bash
g++ server.cpp -o my_server -I crow/include -pthread -lpqxx -lpq
```

### 3. Run the Application
```bash
./my_server
```
The server will initialize the database schema and start listening on port `8080`.
