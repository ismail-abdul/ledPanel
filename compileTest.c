#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  

//Needed to enable clocks for particular GPIO ports
//Needed to define things on the GPIO
//GPIO Port Name, GPIO Mode, GPIO Push Up Pull Down Mode, GPIO Pin Number


/**
 * C6 => input signal
 * C7 => clock signal
 * C8 => latch (active low)
 * C2-5 => row selection
 */

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

//Bottom panel is an extension of the top panel. to it's left


int main(void) {
    rcc_periph_clock_enable(RCC_GPIOC); //Enable clock for GPIO Port C
    
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

    gpio_clear(GPIOC, GPIO5); 
    gpio_clear(GPIOC, GPIO4);
    gpio_clear(GPIOC, GPIO3);
    gpio_clear(GPIOC, GPIO2);
    clear_row();
    for (volatile unsigned int tmr=1e6; tmr > 0; tmr--); //Sleep for 5 seconds

    //Repeat steps 3 through 5 16 times
    for (int i = 0; i<15; i++) {
        set_row(i);
        for (volatile unsigned int tmr=1e5; tmr > 0; tmr--); //Sleep for 5 seconds
        clear_row();
    }

    
    
    

    return 0;
}
