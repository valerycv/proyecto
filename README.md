El código que se encuentra en las branches adjuntas, consiste de los módulos necesarios para construir y ejecutar un gestor de inventario. 
Para que este programa funcione de la manera correcta usted debe tener algunas dependencias instaladas en su equipo: 
GTK +3, se intstala con los comandos: sudo apt install libgtk-3-dev
libsqLite3, se instala con: sudo apt install libsqlite3-dev
Además se usan herramienras que son un poco máscómunes pero en caso de que no las tenga instaladas: 
Pkg-config, se instala con: sudo apt install build-essential pkg-config
Para compilar el programa utilice el comando: 
gcc main.c database.c ui.c -o inventario_admin `pkg-config --cflags --libs gtk+-3.0` -lsqlite3
Y para abrirlo:
./inventario_admin
Una vez que ejecute esos comandos podrá tener acceso a la interfaz y el programa. 
Es importante mencionar que lo primero que debe hcerpara poder ejecutar este programa es guardar todos los móduolos que se encuntran djuntos en una misma carpeta, una vez que
haya realizado ese paso, puede proceder a utilizar los comandos adjuntos en la terminal de su equipo y así podrá hacer uso del gestor de inventario.
