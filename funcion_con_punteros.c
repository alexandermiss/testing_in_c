#include <stdio.h>
#include <string.h>

void intercambia( int*, int* );

int main(){

    int a = 2;
    int b = 4;
    printf("a= %d, b= %d\n", a, b);
    intercambia(&a, &b);
    printf("a= %d, b= %d\n", a, b);
    
    return 0;

}

void intercambia(int *a, int *b ){
    int z;
    z = *a;
    *a = *b;
    *b = z;
}
