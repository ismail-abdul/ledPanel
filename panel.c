#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  
#include "libopencm3/stm32/adc.h" //Needed to convert analogue signals to digital



/*
Coordinate systerm for this game will be zero-indexed, discrete and have it's origin (0,0) be the 
top and left-most pixel on the pannel. 
Meaning that the panel we are representing has it */

#define ROW_WORD_SIZE 4
#define PANEL_HEIGHT 32
#define PANEL_WIDTH 32

#define PADDLE_LENGTH 4
#define JOYSTICK_REGISTER ADC1

#define JOYSTICK_MAX  //Varies. Depends on joystick. Must be a calibrated value for accurate operation.
#define JOYSTICK_MIN 


int renderingData[16][192]; //This stores the frame for rendering
int arr[3] = {1,1,1};
int binaryBits[ROW_WORD_SIZE];

//since the paddle had no middle row or colums, use the top left corner to represent the co-ordinates of every object.  
int x_PaddleA = 0 ;
uint32_t down_PaddleA; 
uint32_t up_PaddleA;

int x_PaddleB;
uint32_t down_PaddleB; 
uint32_t up_PaddleB; 

int x_velocity = 0;
int y_velocity = 0;

bool winner = false;

//should be reusable for other channels as well.
//set macros for all the arguements in all the functions that are called.


void setup();
void setup() {
    //set up clocks for our ports
    rcc_periph_clock_enable(RCC_GPIOA); //Enable clock for Joysticks
    rcc_periph_clock_enable(RCC_GPIOC); //Enable clock for LED panel's Row Selection pins, latch, input signal and system clock. 
    
    //Setup Pins 

    //GPIO Port Name, GPIO Mode, GPIO Push Up Pull Down Mode, GPIO Pin Number
    //Setup & configure input pins.
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1); //Joystick A Up
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); //Joystick A Down
    gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1); //Joystick A Up
    gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0); //Joystick B Down


    //Setup output pins. 
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8); //Latch (Active Low) - Determien if new memory is displayed.
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7); // Clock for rows
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6); //Input Signal - Allow for right-most pixel in shift register to be set to 0 or 1
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5); //Row Selection - Most Significant Bit
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4); //Row Selection
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3); //Row Selection
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2); //Row Selection - Least Significant Bit

    //Configure output pins.
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO7);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO6);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO5);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO4);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO3);   
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO2);   


}

//Returns digital input from the ADC Register dedicated to the joysticks. 
uint32_t readJoystickChannel(int channel_id){
    uint8_t channelArray[1] = {channel_id};
    adc_set_regular_sequence(JOYSTICK_REGISTER, 1, channelArray);  //Set up the channel
    adc_start_conversion_regular(JOYSTICK_REGISTER);  //Start converting the analogue signal

    while(!(adc_eoc(JOYSTICK_REGISTER)));  //Wait until the register is ready to read data

    return adc_read_regular(JOYSTICK_REGISTER); //Read the value from the register and channel
}

//Prepares ADC Register 1 to be read from. *Function could be reusable if we had documentation to tell us the type of this expression.
void setupJoystickRegister(){

    /*Steps tp Setup Register for reading from it's channels*/

    rcc_periph_clock_enable(RCC_ADC12); //Enable clock for ADC registers 1 and 2. No need to disable apparently

    adc_power_off(JOYSTICK_REGISTER);  //Turn off ADC register 1 whist we set it up

    adc_set_clk_prescale(ADC1, ADC_CCR_CKMODE_DIV1);  //Setup a scaling, none is fine for this
    adc_disable_external_trigger_regular(JOYSTICK_REGISTER);   //We don't need to externally trigger the register...
    adc_set_right_aligned(JOYSTICK_REGISTER);  //Make sure it is right aligned to get more usable values
    adc_set_sample_time_on_all_channels(JOYSTICK_REGISTER, ADC_SMPR_SMP_61DOT5CYC);  //Set up sample time
    adc_set_resolution(JOYSTICK_REGISTER, ADC_CFGR1_RES_12_BIT);  //Get a good resolution

    adc_power_on(ADC1);  //Finished setup, turn on ADC register 1
}


void readInput();
void readInput() {
	//define a channel to be looked at
	//setup channel to be read from
	//start conversion
	//listen for register to be ready to be read from
	//read the value into a global variable

    // setupJoystickRegister();
    up_PaddleA = readJoystickChannel(1);
    down_PaddleA = readJoystickChannel(2);
    up_PaddleB = readJoystickChannel(6);
    down_PaddleB = readJoystickChannel(7);

	printf("Finished reading input from joysticks!")
}


void input();
void input(){
	inputPreProcessing();
	readInput(); //should there be a public variable that should be getting updated? Or should it just be updating the location and speed variables?
}

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



    //Preperation to start game.
    setup();
    setupJoystickRegister();

    
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