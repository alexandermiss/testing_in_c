#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nombre_cliente[100];
    int numero_vuelo;
    char asiento[4];
    int reservacion_id;
    int confirmada;
} Reserva;

typedef struct {
    Reserva *datos;      // arreglo dinámico de reservas
    size_t size;         // cuántas reservas hay actualmente
    size_t capacity;     // cuántas puede contener sin redimensionar
} ArregloReservas;

// Inicializar el arreglo vacío
void init_arreglo(ArregloReservas *arr) {
    arr->datos = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

// Agregar una nueva reserva (copia los datos)
void agregar_reserva(ArregloReservas *arr, Reserva nueva) {
    // ¿Necesitamos más espacio?
    if (arr->size == arr->capacity) {
        size_t nueva_capacidad = (arr->capacity == 0) ? 4 : arr->capacity * 2;
        Reserva *temp = (Reserva*) realloc(arr->datos, nueva_capacidad * sizeof(Reserva));
        if (!temp) {
            perror("Error al redimensionar");
            exit(1);
        }
        arr->datos = temp;
        arr->capacity = nueva_capacidad;
    }
    // Agregar la nueva reserva al final
    arr->datos[arr->size] = nueva;
    arr->size++;
}

// Eliminar la última reserva (opcional)
void eliminar_ultima(ArregloReservas *arr) {
    if (arr->size > 0) {
        arr->size--;
    }
}

// Liberar toda la memoria
void liberar_arreglo(ArregloReservas *arr) {
    free(arr->datos);
    arr->datos = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

// Ejemplo de uso
int main() {
    ArregloReservas reservas;
    init_arreglo(&reservas);

    // Crear algunas reservas
    Reserva r1 = {"Juan Perez", 202, "12A", 1001, 1};
    Reserva r2 = {"Maria Gomez", 305, "23B", 1002, 0};
    Reserva r3 = {"Carlos Ruiz", 202, "15C", 1003, 1};

    agregar_reserva(&reservas, r1);
    agregar_reserva(&reservas, r2);
    agregar_reserva(&reservas, r3);

    // Mostrar todas las reservas
    printf("=== Lista de reservas ===\n");
    for (size_t i = 0; i < reservas.size; i++) {
        printf("Cliente: %s | Vuelo: %d | Asiento: %s | ID: %d | Confirmada: %s\n",
               reservas.datos[i].nombre_cliente,
               reservas.datos[i].numero_vuelo,
               reservas.datos[i].asiento,
               reservas.datos[i].reservacion_id,
               reservas.datos[i].confirmada ? "Sí" : "No");
    }

    liberar_arreglo(&reservas);
    return 0;
}
