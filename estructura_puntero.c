#include <stdio.h>
#include <string.h>

#define SIZE 60

struct persona {
    char nombre[SIZE];
    char apellido[SIZE];
};

int main(){
    
    struct persona Persona;
    struct persona *p = NULL;
    
    strcpy(Persona.nombre, "Alexander");
    strcpy(Persona.apellido, "Miss");
    p = &Persona;
    
    printf("Persona %p\n", p);
    printf("Nombre* *p= %p\n", &Persona.nombre);
    printf("Apellido* *p= %p\n", &Persona.apellido);
    printf("Nombre value= %d\n", p->nombre);
}
