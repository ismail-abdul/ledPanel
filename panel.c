#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// #include "libopencm3/stm32/rcc.h"   
// #include "libopencm3/stm32/gpio.h"  
// #include "libopencm3/stm32/adc.h" //Needed to convert analogue signals to digital



#define ROW_WORD_SIZE 4

int renderingData[16][192]; //This stores the frame for rendering
int arr[3] = {1,1,1};
int binaryBits[ROW_WORD_SIZE];

int Paddles(int y,int renderingData[16][192]);
int Paddles(int y,int renderingData[16][192]) {
    int counter = 6;

    while(counter > 0) {
        int x = 34;
        if (y > 16) {
            y = y % 16;
            x += 32;
        }
        if (y > 32) {
            x -= 32;
        }

        addDot(x,y, arr, renderingData);
        y++;
        counter--;
    }
    return 0;
}

int addDot(int x, int y, int col[3], int renderingData[16][192]);
int addDot(int x, int y, int col[3], int renderingData[16][192]) {
    int i;
    for (i = 0; i <3; i++) {
        if (x < 32) {
            renderingData[y][x + 32*i] = 1;//col[i]; //Colour is put in backwards, so it must be BGR format.
        } else {
            renderingData[y][x + 32*i + 96] = 1;//col[i]; //Colour is put in backwards, so it must be BGR format.

        }
    }
    return 0;
}

int rendering(int renderingData[16][192]) {

    return 0;
}

int clear_row(void);
int clear_row(void) {
    for (int i = 0; i<(192); i++) {
        gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
        gpio_clear(GPIOC, GPIO6);//set register to 0
        gpio_clear(GPIOC, GPIO7);//SETS THE CLOCK LOW
    }
    return 0;
}

//We can change this to a binary number and set each line depending on if the digit is a 1 in that position or not
//Really this should be called select row.
int set_row(int num);
int set_row(int num) {
    switch(num) {
        case 0:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 1:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 2:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 3:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 4:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 5:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 6:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 7:
            {gpio_clear(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;
        
        case 8:
            {gpio_set(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 9:
           { gpio_set(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 10:
            {gpio_set(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 11:
            {gpio_set(GPIOC, GPIO5); 
            gpio_clear(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 12:
            {gpio_set(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 13:
            {gpio_set(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_clear(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;

        case 14:
            {gpio_set(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_clear(GPIOC, GPIO2);}
            
            break;

        case 15:
            {gpio_set(GPIOC, GPIO5); 
            gpio_set(GPIOC, GPIO4);
            gpio_set(GPIOC, GPIO3);
            gpio_set(GPIOC, GPIO2);}
            
            break;
        
    }
    return 0;
}


void newBinaryBits(int denary, int bits);
void newBinaryBits(int denary, int bits) {
    /* Assume that the number of bits
    is suffiecient to store the denary number.
    */
    assert((1<<bits) >= denary);
    assert(bits == 4); //2*3 

    for (int exp = bits-1; exp>=0; exp--) {
        if ((1 << exp) > denary) {
            //access the binary bit that is (exp+1) from the MSB
            printf("0");
            binaryBits[bits-(exp+1)] = 0;
        } else {
            printf("1");
            denary -= 1 << exp;
            binaryBits[bits-(exp+1)] = 1; 
        }
    }
}

int selectRow(int num) {
    newBinaryBits(num, ROW_WORD_SIZE);
    int C2[1] = {GPIO5};
    // int C3[2]
    // int C4[2]
    // int C5[2]
}

int main(){
    selectRow(12);
}

int main2(void){
    rcc_periph_clock_enable(RCC_GPIOC); //Enable clock for GPIO Port C
    renderingData[1][1]=1;
    //"Opens" 2-8 pins and enables. 
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);

    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO7);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO6);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO5);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO4);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO3);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO2);   

    gpio_set(GPIOC, GPIO8); // Sets C8. i.e. THE LATCH. It must be high for you show what is in memory. active low.

    Paddles(3, renderingData);
    while(1){
        int i=0; //i is the row
        int j=0; //j/3 is the column, and there are 3 bits of colour per column
        for(i = 0; i < 16; i++) {
            gpio_clear(GPIOC, GPIO8);
            set_row(i); //Select the correct row
            for(j=0; j<(192); j++) {
                gpio_clear(GPIOC, GPIO7); //Set clock low
                if (renderingData[i][j] == 1) { //if we're putting a bit in, set the input to 1
                    gpio_set(GPIOC, GPIO6);
                } else {                        //if we're not putting a bit in, set the input to 0
                    gpio_clear(GPIOC, GPIO6);
                }
                                     int j=0;
                gpio_set(GPIOC, GPIO7); //Set clock high

            }
            gpio_set(GPIOC, GPIO8);
                    for(j=0;j++;j<1e4);

        }
   
    }
    return 0;
}