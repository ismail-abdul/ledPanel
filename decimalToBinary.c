#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ROW_WORD_SIZE 4


/*Choosing to avoid the use of malloc, calloc and realloc.
Due to inexperience with malloc and calloc, I want to avoid issues with returning
arrays // pointers to arrays. Too risky and not necessary. Especially when using a VM. */


void decimalToBinary(int denary, int bits);
void decimalToBinary(int denary, int bits) {
    /* Assume that the number of bits
    is suffiecient to store the denary number.
    */
    assert((1<<bits) >= denary);
    assert(bits == 4); //2*3 

    int binaryNum[bits];

    for (int exp = bits-1; exp>=0; exp--) {
        if ((1 << exp) > denary) {
            //access the binary bit that is (exp+1) from the MSB
            printf("0");
            binaryNum[bits-(exp+1)] = 0;
        } else {
            printf("1");
            denary -= 1 << exp;
            binaryNum[bits-(exp+1)] = 1; 
        }
    }
}


int main(){
    int denary_ = 5; int bits_ = ROW_WORD_SIZE;
    decimalToBinary(denary_, bits_); 
}