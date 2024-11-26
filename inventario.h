
#ifndef INVENTARIO_H
#define INVENTARIO_H

typedef struct {
    int id;
    char name[100];
    int quantity;
    float price;
    char supplier[100];
} Product;

#endif