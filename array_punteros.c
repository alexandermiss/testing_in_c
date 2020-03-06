#include <stdio.h>

int main(){

    int arreglo[4] = {4,3,2,1};
    
    for( int i=0; i < 4; i++ ){
    
    printf("all %p\n", &arreglo);
    printf("p %p\n", &arreglo[i]);
    printf("value %d\n", arreglo[i]);
    printf("value %d\n", *(arreglo+i));
    printf("----------------------------\n\n");
    
    }
    
    return 0;

}
