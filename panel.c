#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  

int renderingData[16][192]; //This stores the frame for rendering
int arr[3] = {1,1,1};

int Paddles(int y);
int Paddles(int y) {
    int counter = 6;

    while(counter > 0) {
        int x = 2;
        if (y > 16) {
            y = y % 16;
            x += 32;
        }
        if (y > 32) {
            x -= 32;
        }

        addDot(x,y, arr);
        y++;
        counter--;
    }
    return 0;
}

int addDot(int x, int y, int col[3]);
int addDot(int x, int y, int col[3]) {
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

int setPaddleLocation (int y);
int setPaddleLocation (int y) {
    //Input a co-ordinate and store it. 
    //y range => 0 - 31
    //if y =  15 and moving down, then shift 32 bits across such that it lines up with it's previous state.
    if (y>15) {
        y %= 16;
        
    }        
    return 0;                                 
}

int rendering(int renderingData[16][192]) {

    return 0;
}

int clear_row(void);
int clear_row(void) {
    for (int i = 0; i<(193); i++) {
        gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
        gpio_clear(GPIOC, GPIO6);
        gpio_clear(GPIOC, GPIO7);
    }
    return 0;
}

//We can change this to a binary number and set each line depending on if the digit is a 1 in that position or not
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


int main(){
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

    Paddles(3);
    while(1){
    int i=0; //i is the row
    int j=0; //j/3 is the column, and there are 3 bits of colour per column
    for(i = 0; i < 16; i++) {
        set_row(i); //Select the correct row
        for(j=0; j<(192); j++) {
            gpio_clear(GPIOC, GPIO7); //Set clock low
            if (renderingData[i][j] == 1) { //if we're putting a bit in, set the input to 1
                gpio_set(GPIOC, GPIO6);
            } else {                        //if we're not putting a bit in, set the input to 0
                gpio_clear(GPIOC, GPIO6);
            }
            gpio_set(GPIOC, GPIO7); //Set clock high
        }
    }
    }
    return 0;
}