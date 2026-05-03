#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int get_exe(char **nombre, char **ruta);

int main(void) {
    char *nombre = "";
    char *ruta = "";

    get_exe(&nombre, &ruta);

    printf("Nombre del ejecutable: %s\n", nombre);
    printf("Ruta completa del ejecutable: %s\n", ruta);

    printf("mensaje: %s\n", nombre);

    return 0;
}

int get_exe(char **nombre, char **ruta)
{
    char path[PATH_MAX];
    ssize_t len;

    // Leer el enlace simbólico /proc/self/exe
    len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1) {
        perror("readlink");
        return 1;
    }

    path[len] = '\0';  // Asegurar terminación nula

    // Obtener el nombre base (último componente después de '/')
    char *name = strrchr(path, '/');
    if (name != NULL)
        name++;  // Saltar la barra
    else
        name = path;  // No hay barra, usar todo

    *nombre = name;
    *ruta = strdup(path);

    return 0;
}
