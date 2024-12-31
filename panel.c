#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libopencm3/stm32/rcc.h"   
#include "libopencm3/stm32/gpio.h"  
#include "libopencm3/stm32/adc.h" //Needed to convert analogue signals to digital



/*
Coordinate systerm for this game will be zero-indexed, discrete and have it's origin (0,0) be the 
top and right-most pixel on the panel. 
Meaning that the panel we are representing has it
 */

/*
The "physics" of the game will be simple.
When the ball and paddle collide, their velocities in the y-planve will be additive.
If the ball is moving down and the paddle is moving down, the velocity of the ball 
in the y-plane will be the vector addition of their velocities (in the y-plane).

It will be reset when a goal is scored.
*/

/*
Player A is on the left side (i.e x = 31) and Player B is on the right side.
*/

#define ROW_WORD_SIZE 4
#define PANEL_HEIGHT 32
#define PANEL_WIDTH 32

#define PADDLE_LENGTH 4
#define JOYSTICK_REGISTER ADC1

#define JOYSTICK_MAX 4095  //Varies. Depends on joystick. Must be a calibrated value for accurate operation.
#define JOYSTICK_MIN 0 //For the sake of accuracy, we'll use 4095 as the STM32 Documentation says.  Since the max value of a 12 bit number is 4095.

typedef struct {
    int velocity;
    int y;
    int score;
} Player;

typedef struct {
    int x;
    int y;
} Velocity;

typedef struct {
    int x;
    int y;
    Velocity Velocity;
} Ball;

Player paddle_A;
Player paddle_B;

Ball ball;

int renderingData[16][192]; //This stores the frame for rendering
int binaryBits[ROW_WORD_SIZE];

//since the paddle had no middle row or colums, use the top left corner to represent the co-ordinates of every object.  
int y_paddle_y = PANEL_HEIGHT/2 - 2 ; //paddle at middle of height of panel
uint32_t down_paddle_A; 
uint32_t up_paddle_A;

int y_PaddleB = PANEL_HEIGHT/2 - 2;
uint32_t down_paddle_B; 
uint32_t up_paddle_B; 

int x_velocity = 0;
int y_velocity = 0;

char winner = 'X';

//should be reusable for other channels as well.
//set macros for all the arguements in all the functions that are called.

void initialSetup(void) {

    //set up inital values for the Ball and 

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

    setupJoystickRegisters();
}

//Returns digital input from the ADC Register dedicated to the joysticks. 
uint32_t readJoystickChannel(int channel_id) {
    uint8_t channelArray[1] = {channel_id};
    adc_set_regular_sequence(JOYSTICK_REGISTER, 1, channelArray);  //Set up the channel
    adc_start_conversion_regular(JOYSTICK_REGISTER);  //Start converting the analogue signal

    while(!(adc_eoc(JOYSTICK_REGISTER)));  //Wait until the register is ready to read data

    return adc_read_regular(JOYSTICK_REGISTER); //Read the value from the register and channel
}

//Prepares ADC Register 1 to be read from. *Function could be reusable if we had documentation to tell us the type of this expression.
void setupJoystickRegisters(void) {

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

void readInput(void) {
	//define a channel to be looked at
	//setup channel to be read from
	//start conversion
	//listen for register to be ready to be read from
	//read the value into a global variable

    up_paddle_A = readJoystickChannel(1);
    down_paddle_A = readJoystickChannel(2);
    up_paddle_B = readJoystickChannel(6);
    down_paddle_B = readJoystickChannel(7);

    paddle_A.velocity = (up_paddle_A < 205) ? 0 : (up_paddle_A < 2049 ? 1 : 2);
    paddle_A.velocity = (down_paddle_A < 205) ? 0 : (down_paddle_A < 2049 ? -1 : -2);

    paddle_B.velocity = (up_paddle_B < 205) ? 0 : (up_paddle_B < 2049 ? 1 : 2);
    paddle_B.velocity = (down_paddle_B < 205) ? 0 : (down_paddle_B < 2049 ? -1 : -2);

	printf("Finished reading & interpretting input from joysticks!");
}

//Overwrite the display data where necessary.
void update(void) {

    //Move paddles & ensure they stay in bounds
    paddle_A.y += paddle_A.velocity;
    paddle_B.y += paddle_B.velocity;

    //Check for paddle co-ordinates that exist "outside of the panel".
    
    if (paddle_A.y + PADDLE_LENGTH > 32) {
        paddle_A.y  = 32 - PADDLE_LENGTH;
    } else if (paddle_A.y < 0) {
        paddle_A.y = 0;
    }

    if (paddle_B.y + PADDLE_LENGTH > 31) {
        paddle_B.y = 32 - PADDLE_LENGTH;
    } else if (paddle_B.y < 0) {
        paddle_B.y = 0;
    }


    //Check to see if the new ball positions are valid.
    ball.x += ball.Velocity.x;
    ball.y += ball.Velocity.y;

    //if there are no collisions.
    if ((ball.x > 1) && (ball.x < 30) && (ball.y > 0) && (ball.y < 31)) {
        return;
    }
    
    //condition for hitting roof or floor
    if (ball.y > 31) {
        ball.y = 31;
        ball.Velocity.y = -ball.Velocity.y;
    } else if (ball.y < 0) {
        ball.y = 0;
        ball.Velocity.y = -ball.Velocity.y;
    }

    //Check if the ball hit Player A's paddle
    if(ball.y <= (paddle_A.y-1) || ball.y >= (paddle_A.y + 3)) {
        if(ball.x == 31 || ball.x == 30) {
	    ball.x = 29;
	    ball.Velocity.x = -ball.Velocity.x;
	    return; //Return since we no longer need to check for a goal if it was blocked
	}
    }
    //Check if the ball hit Player B's paddle
    if(ball.y <= (paddle_B.y-1) || ball.y >= (paddle_B.y + 3)) {
        if(ball.x == 0) {
	    ball.x = 1;
	    ball.Velocity.x = -ball.Velocity.x;
	    return; //Return since we no longer need to check for a goal if it was blocked
	}
    }
    
    //Now check for goals
    if (ball.x < 0) {
        paddle_A.score += 1;
        
    } else if (ball.x > 30) {
        paddle_B.score += 1;
    }

}


void input(void);
void input(void){
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

/*int removeDot for manipulating objects rather than just clearing the array*/

void rendering(int renderingData[16][192]) {

}

int clear_row(void) {
    for (int i = 0; i<(192); i++) {
        gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
        gpio_clear(GPIOC, GPIO6);//set register to 0
        gpio_clear(GPIOC, GPIO7);//SETS THE CLOCK LOW
    }
    return 0;
}

//Function for selecting a row from 0-15
void select_row(int row) {

    //Bitmasking to select a certain bit and check if the bit is there
    int bit_0 = row & (1 << 3);
    int bit_1 = row & (1 << 2);
    int bit_2 = row & (1 << 1);
    int bit_3 = row & 1;

    //Checking the specific bit and using it to select the row
    if (bit_0 == 0) {
        gpio_clear(GPIOC, GPIO5);
    } else {
        gpio_set(GPIOC, GPIO5);
    }
    if (bit_1 == 0) {
        gpio_clear(GPIOC, GPIO4);
    } else {
        gpio_set(GPIOC, GPIO4);
    }
    if (bit_2 == 0) {
        gpio_clear(GPIOC, GPIO3);
    } else {
        gpio_set(GPIOC, GPIO3);
    }
    if (bit_3 == 0) {
        gpio_clear(GPIOC, GPIO2);
    } else {
        gpio_set(GPIOC, GPIO2);
    }
}

int selectRow(int num) {
    newBinaryBits(num, ROW_WORD_SIZE);
    int C2[1] = {GPIO5};
    // int C3[2]
    // int C4[2]
    // int C5[2]
}

void onGoal(void) {

}

void main(void) {

    //Preperation to start game.
    initialSetup();

    while (winner == 'X') {
        input();
        update();
        render();
        winner = isGameOver();
    }
}
