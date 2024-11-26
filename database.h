#ifndef DATABASE_H
#define DATABASE_H

#include "inventario.h"

int open_database();
int close_database();
int add_product(Product *product);
int delete_product(int id);
int get_product_by_id(int id, Product *product);
int list_all_products(Product products[], int max_products);


#endif

