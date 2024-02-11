/* A simple test harness for memory alloction. */

#include "mm_alloc.h"
#include <stdio.h>
int main(int argc, char **argv)
{
    int *data;

    data = (int *) mm_malloc(3 * sizeof(int));
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;

    printf("data[0] = %d\n", data[0]);
    printf("data[1] = %d\n", data[1]);
    printf("data[2] = %d\n", data[2]);

    //reallocating the memory to bigger size
    data = (int *) mm_realloc(data, 5 * sizeof(int));
    data[3] = 4;
    data[4] = 5;

    printf("data[3] = %d\n", data[3]);
    printf("data[4] = %d\n", data[4]);
	
    //reallocating the memory to smaller size
    data = (int *) mm_realloc(data, 2 * sizeof(int));
    printf("data[0] = %d\n", data[0]);
    printf("data[1] = %d\n", data[1]);
   

   
}
