#include <gtk/gtk.h>
#include "database.h"

// Almacena las entradas de texto
typedef struct {
    GtkWidget *name_entry;
    GtkWidget *id_entry;
    GtkWidget *price_entry;
    GtkWidget *supplier_entry;
    GtkWidget *quantity_entry;
} ProductEntryWidgets;

// Errores en consola
void show_error_dialog(const char *message) {
    g_print("Error: %s\n", message);
}

// Callback para guardar un producto desde la ventana de agregar productos
void on_save_button_clicked(GtkWidget *widget, gpointer data) {
    ProductEntryWidgets *entries = (ProductEntryWidgets *)data;
    Product product;

    const char *name = gtk_entry_get_text(GTK_ENTRY(entries->name_entry));
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entries->id_entry));
    const char *price_text = gtk_entry_get_text(GTK_ENTRY(entries->price_entry));
    const char *supplier = gtk_entry_get_text(GTK_ENTRY(entries->supplier_entry));
    const char *quantity_text = gtk_entry_get_text(GTK_ENTRY(entries->quantity_entry));

    if (strlen(name) == 0 || strlen(id_text) == 0 || strlen(quantity_text) == 0 || strlen(price_text) == 0) {
        show_error_dialog("Todos los campos son obligatorios.");
        return;
    }

    int id = atoi(id_text);
    int quantity = atoi(quantity_text);
    float price = atof(price_text);
    if (id <= 0 || quantity <= 0 || price <= 0) {
        show_error_dialog("ID, cantidad y precio deben ser números positivos.");
        return;
    }

    snprintf(product.name, sizeof(product.name), "%s", name);
    product.id = id;
    product.price = price;
    product.quantity = quantity;
    snprintf(product.supplier, sizeof(product.supplier), "%s", supplier);

    if (add_product(&product) == 0) {
        g_print("Producto añadido: %s, ID: %d, Cantidad: %d, Precio: %.2f, Proveedor: %s\n",
                product.name, product.id, product.quantity, product.price, product.supplier);
    } else {
        g_print("Error al añadir el producto. Verifica que el ID no esté duplicado.\n");
    }

    GtkWidget *dialog = gtk_widget_get_toplevel(widget);
    if (GTK_IS_WIDGET(dialog)) {
        gtk_widget_destroy(dialog); 
    }
}

// Ventana de agregar productos
void open_add_product_dialog(GtkWidget *parent) {
    GtkWidget *dialog, *content_area, *grid, *save_button;
    ProductEntryWidgets *entries = g_malloc(sizeof(ProductEntryWidgets));

    dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Agregar Artículo");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 200);

    content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(dialog), content_area);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    // Etiquetas y entradas
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Nombre:"), 0, 0, 1, 1);
    entries->name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entries->name_entry, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("ID:"), 0, 1, 1, 1);
    entries->id_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entries->id_entry, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Precio:"), 0, 2, 1, 1);
    entries->price_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entries->price_entry, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Proveedor:"), 0, 3, 1, 1);
    entries->supplier_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entries->supplier_entry, 1, 3, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Cantidad:"), 0, 4, 1, 1);
    entries->quantity_entry = gtk_entry_new(); 
    gtk_grid_attach(GTK_GRID(grid), entries->quantity_entry, 1, 4, 1, 1);

    // Botón para guardar
    save_button = gtk_button_new_with_label("Guardar");
    gtk_grid_attach(GTK_GRID(grid), save_button, 0, 5, 2, 1);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), entries);

    gtk_widget_show_all(dialog);
}

// Callback para buscar un producto
void on_search_button_clicked(GtkWidget *widget, gpointer data) {
    ProductEntryWidgets *entries = (ProductEntryWidgets *)data;

    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entries->id_entry));
    if (strlen(id_text) == 0) {
        g_print("Error: El campo de ID está vacío.\n");
        return;
    }

    int id = atoi(id_text);
    Product product;

    if (get_product_by_id(id, &product) == 0) {
        g_print("Producto encontrado: %s, ID: %d, Cantidad: %d, Precio: %.2f, Proveedor: %s\n",
                product.name, product.id, product.quantity, product.price, product.supplier);
    } else {
        g_print("Producto con ID %d no encontrado.\n", id);
    }
}

// Callback para listar productos
void on_list_button_clicked(GtkWidget *widget, gpointer data) {
    Product products[100];
    int product_count = list_all_products(products, 100);

    if (product_count == -1) {
        g_print("Error al obtener los productos de la base de datos.\n");
        return;
    }

    g_print("Lista de productos:\n");
    for (int i = 0; i < product_count; i++) {
        g_print("ID: %d, Nombre: %s, Cantidad: %d, Precio: %.2f, Proveedor: %s\n",
                products[i].id, products[i].name, products[i].quantity, products[i].price, products[i].supplier);
    }
}

// Callback para eliminar productos
void on_delete_button_clicked(GtkWidget *widget, gpointer data) {
    ProductEntryWidgets *entries = (ProductEntryWidgets *)data;

    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entries->id_entry));
    if (strlen(id_text) == 0) {
        g_print("Error: El campo de ID está vacío.\n");
        return;
    }

    int id = atoi(id_text);
    if (delete_product(id) == 0) {
        g_print("Producto con ID %d eliminado con éxito.\n", id);
    } else {
        g_print("Error: Producto con ID %d no encontrado.\n", id);
    }
}

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    // Carga el CSS
    gtk_css_provider_load_from_path(provider, "pers.css", NULL);

    // Aplica el estilo
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider); // Libera el proveedor después de cargarlo
}


// Configuración de la interfaz principal
void setup_ui() {
    GtkWidget *window, *box;
    GtkWidget *add_button, *delete_button, *search_button, *list_button;
    ProductEntryWidgets *entries = g_malloc(sizeof(ProductEntryWidgets));

    gtk_init(NULL, NULL);
    load_css(); 

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Gestor de Inventario");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    entries->id_entry = gtk_entry_new();

    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("ID del producto:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), entries->id_entry, FALSE, FALSE, 0);

    add_button = gtk_button_new_with_label("Agregar Producto");
    delete_button = gtk_button_new_with_label("Eliminar Producto");
    search_button = gtk_button_new_with_label("Buscar Producto");
    list_button = gtk_button_new_with_label("Listar Productos");

    gtk_style_context_add_class(gtk_widget_get_style_context(add_button), "big-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(delete_button), "big-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(search_button), "big-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(list_button), "big-button");

    gtk_box_pack_start(GTK_BOX(box), add_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), delete_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), search_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), list_button, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(add_button, "clicked", G_CALLBACK(open_add_product_dialog), window);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), entries);
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_button_clicked), entries);
    g_signal_connect(list_button, "clicked", G_CALLBACK(on_list_button_clicked), entries);

    gtk_widget_show_all(window);
    gtk_main();

    g_free(entries);
}
