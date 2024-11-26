#include "database.h"
#include "ui.h"
#include <stdio.h>

int main() {
    if (open_database() != 0) {
        return 1;  // Si no se puede abrir la base de datos, sale del programa
    }

    setup_ui();

    close_database();
    return 0;
}
