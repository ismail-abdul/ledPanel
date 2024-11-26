#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  


int co_ord_input (int y);
int setPaddleLocation (int _y) {
    //Input a co-ordinate and store it. 
    //y range => 0 - 31
    //if y =  15 and moving down, then shift 32 bits across such that it lines up with it's previous state.
    int y;
    if (_y>15) {
        y %= 16;
        
    }                                         
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
    
    for (int i=0; i<(193); i++) {
        gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
        gpio_set(GPIOC, GPIO6);
        gpio_clear(GPIOC, GPIO7); // set clock low
    }
    return 0;
}


int main (){

}