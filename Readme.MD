# C++ Crow CRUD Demo with PostgreSQL

A minimalist, high-performance CRUD REST API built with the **Crow** microframework and **libpqxx** for PostgreSQL connectivity.

This project serves as an experimental playground for backend web development in native C++. It explores modern C++ web development while comparing compilation time, executable size, and runtime performance with Java frameworks such as Spring Boot.

---

## Prerequisites

- C++17 compatible compiler (e.g. `g++`)
- PostgreSQL client libraries (`libpq` and `libpqxx`)
- Docker Desktop (or Docker Engine)

---

## 1. Start PostgreSQL

Run a PostgreSQL container:

```bash
docker run --name my_postgres \
  -e POSTGRES_USER=myuser \
  -e POSTGRES_PASSWORD=mypassword \
  -e POSTGRES_DB=mydb \
  -p 5432:5432 \
  -d postgres:16
```

---

## 2. Build the Server

Compile the application:

```bash
g++ server.cpp -o my_server \
  -I crow/include \
  -pthread \
  -lpqxx \
  -lpq
```

---

## 3. Run the Application

```bash
./my_server
```

The server initializes the database schema (if necessary) and starts listening on **http://localhost:8080**.

---

## Performance Notes

> **Note**
>
> This project intentionally does **not** implement a database connection pool in order to keep the example simple and focused on CRUD operations.
>
> Opening a new PostgreSQL connection for every HTTP request is acceptable for a demonstration project but is **not recommended for production**. For high-concurrency workloads or performance benchmarking, consider implementing a thread-safe connection pool using `std::mutex` and `std::queue`.

---

## Tested Environment

- Lubuntu Linux
- Docker Desktop
- PostgreSQL 16
- Crow
- libpqxx
