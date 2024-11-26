#include <sqlite3.h>
#include <stdio.h>
#include "database.h"

sqlite3 *db;

// Función para abrir la base de datos

int open_database() {
        if (sqlite3_open("inventory.db", &db) != SQLITE_OK) {
        fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Crear la tabla de productos 
    const char *sql = "CREATE TABLE IF NOT EXISTS products ("
                      "id INTEGER PRIMARY KEY, "
                      "name TEXT NOT NULL, "
                      "quantity INTEGER, "
                      "price REAL, "
                      "supplier TEXT);";
    char *err_msg = NULL;

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Error al crear la tabla: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

}


// Función para cerrar la base de datos
int close_database() {
    return sqlite3_close(db);
}

// Función para agregar un producto
int add_product(Product *product) {

     Product existing_product;
    if (get_product_by_id(product->id, &existing_product) == 0) {
        fprintf(stderr, "El producto con ID %d ya existe.\n", product->id);
        return -1; 
    }

    const char *sql = "INSERT INTO products (id, name, quantity, price, supplier) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error al preparar la declaración: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, product->id);
    sqlite3_bind_text(stmt, 2, product->name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, product->quantity);
    sqlite3_bind_double(stmt, 4, product->price);
    sqlite3_bind_text(stmt, 5, product->supplier, -1, SQLITE_STATIC);

    int step_result = sqlite3_step(stmt);
    if (step_result != SQLITE_DONE) {
        fprintf(stderr, "Error al ejecutar la declaración: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}


// Función para eliminar un producto 
int delete_product(int id) {
    const char *sql = "DELETE FROM products WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error al preparar la declaración: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, id);

    int result = sqlite3_step(stmt) == SQLITE_DONE ? 0 : -1;
    if (result == -1) {
        fprintf(stderr, "Error al eliminar el producto con ID %d: %s\n", id, sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return result;
}


// Función para buscar un producto por ID

int get_product_by_id(int id, Product *product) {
    const char *sql = "SELECT name, quantity, price, supplier FROM products WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error al preparar la declaración: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        const char *supplier = (const char *)sqlite3_column_text(stmt, 3);

        if (name == NULL || supplier == NULL) {
            fprintf(stderr, "Error: Datos nulos encontrados en la base de datos.\n");
            sqlite3_finalize(stmt);
            return -1;
        }

        // Asignar valores a la estructura del artículo
        snprintf(product->name, sizeof(product->name), "%s", name);
        product->quantity = sqlite3_column_int(stmt, 1);
        product->price = (float)sqlite3_column_double(stmt, 2);
        snprintf(product->supplier, sizeof(product->supplier), "%s", supplier);

        sqlite3_finalize(stmt);
        return 0;
    }

    fprintf(stderr, "Producto con ID %d no encontrado.\n", id);
    sqlite3_finalize(stmt);
    return -1;
}

int list_all_products(Product products[], int max_products) {
    const char *sql = "SELECT id, name, quantity, price, supplier FROM products;";
    sqlite3_stmt *stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error al preparar la declaración: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_products) {
        products[count].id = sqlite3_column_int(stmt, 0);
        snprintf(products[count].name, sizeof(products[count].name), "%s", sqlite3_column_text(stmt, 1));
        products[count].quantity = sqlite3_column_int(stmt, 2);
        products[count].price = (float)sqlite3_column_double(stmt, 3);
        snprintf(products[count].supplier, sizeof(products[count].supplier), "%s", sqlite3_column_text(stmt, 4));
        count++;
    }

    sqlite3_finalize(stmt);
    return count;
}
