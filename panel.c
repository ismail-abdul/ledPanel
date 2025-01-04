#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
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

//Array used to store all frame data
int renderingData[16][192];
int WHITE_BITS[3] = {1,1,1}; //Used as an argument when wanting to display a white pixel on the LED Panel, BGR format
int PURPLE_BITS[3] = {1,0,1}; //For purple instead, for contrast


//struct to represent the velocity vector
typedef struct {
    int x;
    int y;
} Velocity;

//struct to store the Player's paddle data
typedef struct {
    Velocity Velocity;
    int x;
    int y;
    int score;
} Player;

//Struct to represent the ball's data
typedef struct {
    int x;
    int y;
    Velocity Velocity;
} Ball;

// All prototypes, so that we don't have issues with running code at the wrong time //
void initialSetup(void);
uint32_t readJoystickChannel(int channel_id);
void setupJoystickRegisters(void);
void setupBall(void);
int randInt (int min, int max);
void setupPaddles(void);

void readInput(void);

void update(void);
void addDot(int x, int y, int col[3]);
int clear_row(void);
void clear_screen(void);
void clear_data(void);
void select_row(int row);

void drawPaddleRight(int y);
void drawPaddleLeft(int y);
void drawBall(int x, int y);
void drawHorizontalLine(int x1, int x2, int y);
void drawVerticalLine(int x, int y1, int y2);
void drawHorizontalThicken(int scale, int x1, int x2, int y);
void drawVerticalThicken(int scale, int x, int y1, int y2);
void drawZero(int x, int y, int scale);
void drawOne(int x, int y, int scale);
void drawTwo(int x, int y, int scale);
void drawThree(int x, int y, int scale);
void drawFour(int x, int y, int scale);
void drawFive(int x, int y, int scale);
void drawSix(int x, int y, int scale);
void drawSeven(int x, int y, int scale);
void drawEight(int x, int y, int scale);
void drawNine(int x, int y, int scale);
void drawScores(void);

void render(void);

void onGoal(void);
char isGameOver(void);
void onGameOver(void);
void main(void);

Player paddle_A;
Player paddle_B;

//This is the column the paddle moves up and down
paddle_A.x = 31;
paddle_B.x = 0;

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
int justScored = 0;

//should be reusable for other channels as well.
//set macros for all the arguements in all the functions that are called.

void initialSetup(void) {

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

    //set up inital values for the Ball and paddles & prepare joystick registers
    setupJoystickRegisters();
    setupBall();
    setupPaddles();
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

int randInt (int max, int min) {
    srand(time(NULL));
    int value = min + (rand() % (max - min + 1) );
    assert(value <= max && value >= min);
    return value;
}

void setupBall(void){
    ball.Velocity.x = randInt(0,2) - 1;
    ball.Velocity.y = randInt(0,2) - 1;
    ball.x = randInt(floor(3/4*PANEL_WIDTH), PANEL_WIDTH - floor(3/4*PANEL_WIDTH));
    ball.y = randInt(PANEL_HEIGHT - 2, PANEL_HEIGHT + 2);
}

void setupPaddles(void) {
    paddle_A.topLeft_y = PANEL_HEIGHT/2 - 2;
    paddle_B.topLeft_y = paddle_A.topLeft_y;
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

    paddle_A.Velocity.y = (up_paddle_A < 205) ? 0 : (up_paddle_A < 2049 ? 1 : 2);
    paddle_A.Velocity.y = (down_paddle_A < 205) ? 0 : (down_paddle_A < 2049 ? -1 : -2);

    paddle_B.Velocity.y = (up_paddle_B < 205) ? 0 : (up_paddle_B < 2049 ? 1 : 2);
    paddle_B.Velocity.y = (down_paddle_B < 205) ? 0 : (down_paddle_B < 2049 ? -1 : -2);
}

//Overwrite the display data where necessary.
void update(void) {

    //Move paddles & ensure they stay in bounds
    paddle_A.y += paddle_A.Velocity.y;
    paddle_B.y += paddle_B.Velocity.y;

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

    //if there are no collisions with the walls (THIS IS UNNECESSARY?)
    if ((ball.x > (paddle_B.x + 1)) && (ball.x < (paddle_A.x - 1)) && (ball.y > paddle_B.x) && (ball.y < paddle_A.x)) {
        return;
    }
    
    //condition for hitting roof or floor
    if (ball.y > 30) {
        ball.y = 30;
        ball.Velocity.y = -ball.Velocity.y;
    } else if (ball.y < 0) {
        ball.y = 0;
        ball.Velocity.y = -ball.Velocity.y;
    }

    //Check if the ball hit Player A's paddle
    if(ball.y <= (paddle_A.y-1) || ball.y >= (paddle_A.y + 3)) {
        if(ball.x == paddle_A.x || ball.x == (paddle_A.x - 1)) {
	    //If it collides, reset the ball to just above the paddle
	    ball.x = paddle_A.x - 2;
	    ball.Velocity.x = -ball.Velocity.x;
	    return; //Return since we no longer need to check for a goal if it was blocked
	}
    }
    //Check if the ball hit Player B's paddle
    if(ball.y <= (paddle_B.y-1) || ball.y >= (paddle_B.y + 3)) {
        if(ball.x == paddle_B.x) {
	    //If it collides, reset the ball to just above the paddle
	    ball.x = paddle_B.x + 1;
	    ball.Velocity.x = -ball.Velocity.x;
	    return; //Return since we no longer need to check for a goal if it was blocked
	}
    }
    
    //Now check for goals
    if (ball.x == 0) {
        paddle_A.score += 1;
        justScored = 1;
        onGoal();
    } else if (ball.x == 30) {
        paddle_B.score += 1;
        justScored = 1;
        onGoal();
        return;
    } else {
        printf("Unexpected vales for ball co-ordinates");
        assert(1 == 0);
    }
}

//This function accepts coordinates as you would see if you labelled the actual panel, 32x32
void addDot(int x, int y, int col[]) {

    //If the check is equal, we don't need to change anything
    //If the check is different, we need to shift over 92 pixels
    //to represent the lower half of the panel
    y %= 16;
    int check = y % 32;
    if(y != check) {
        x += 92;
    }

    int i = 0;
    while(i < 3) {
        //Complicated, but this is mathematics to convert from the coordinate system
        //we chose into what the panel actually accepts as an input, 16x192
        renderingData[y][192 - (x + 32*i)] = col[2 - i];
        i++;
    }
}

/*int removeDot for manipulating objects rather than just clearing the array*/

int clear_row(void) {
    for (int i = 0; i<(192); i++) {
        gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
        gpio_clear(GPIOC, GPIO6);//set register to 0
        gpio_clear(GPIOC, GPIO7);//SETS THE CLOCK LOW
    }
    return 0;
}

//Function for wiping the screen, useful for end screens or blank screens.
void clear_screen(void) {
    for (int j = 0; j < 16; j++) {
        select_row(j);
        for (int i = 0; i<(192); i++) {
            gpio_set(GPIOC, GPIO7); //SETS CLOCK HIGH
            gpio_clear(GPIOC, GPIO6);
            gpio_clear(GPIOC, GPIO7); //SETS CLOCK LOW
        }
    }
}

//Removes all data in the renderingData array for the next frame
void clear_data(void) {
    for(int j = 0; j < 16; j++) {
        for(int i = 0, i < 192; i++) {
            renderingData[j][i] = 0;
        }
    }
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

//The following functions are all for "drawing" a shape onto the screen
void drawPaddleRight(int y) {
    int counter = 4; //This is the height of each paddle
    int x = 0; //This is the column the paddle will travel in

    //We recursively draw dots to make the paddle
    while(counter >= 0) {
        addDot(y, x, WHITE_BITS);
        y++;
        counter--;
    }
}

void drawPaddleLeft(int y) {
    int counter = 4; //This is the height of each paddle
    int x = 31; //This is the column the paddle will travel in

    //We recursively draw dots to make the paddle
    while(counter >= 0) {
        addDot(y, x,WHITE_BITS);
        y++;
        counter--;
    }
}

void drawBall(void) {
    //Add a dot in four pixels to make the ball

    //Check each pixel doesn't overlap     
    int y;
    int x;
    int check;
		
    y = ball.y % 16;
    check = ball.y % 32;
    if(y != check) {
        x = ball.x + 92;
    }			
	

    //This checks if the Green pixel is activated
    if(renderingData[y][192 - x + 32] == 1) {
	addDot(ball.y,   ball.x,   PURPLE_BITS);
    } else {
        addDot(ball.y,   ball.x,   WHITE_BITS);
    }

     y = (ball.y + 1) % 16;
    check = ball.y % 32;
    if(y != check) {
        x = ball.x + 92;
    }	
	
    if(renderingData[y][192 - x + 32] == 1) {
        addDot(ball.y+1, ball.x,   PURPLE_BITS);
    } else {
        addDot(ball.y+1, ball.x,   WHITE_BITS);
    }
	
    y = ball.y % 16;
    check = ball.y % 32;
    if(y != check) {
        x = ball.x + 92 + 1;
    }	
	
    if(renderingData[y][192 - x + 32] == 1) {
        addDot(ball.y,   ball.x+1, PURPLE_BITS);
    } else {
        addDot(ball.y,   ball.x+1, WHITE_BITS);
    }
	
    y = (ball.y + 1) % 16;
    check = ball.y % 32;
    if(y != check) {
        x = ball.x + 92 + 1;
    }
	
    if(renderingData[y][192 - x + 32] == 1) {
        addDot(ball.y+1, ball.x+1, PURPLE_BITS);
    } else {
        addDot(ball.y+1, ball.x+1, WHITE_BITS);
    }
}

void drawHorizontalLine(int x1, int x2, int y) {
    int counter;
    if(x2 >= x1){
        counter = 0;
        while(counter >= x2-x1) {
            addDot(x1 + counter, y, WHITE_BITS);
	    counter++;
	}
    } else {
        counter = 0;
        while(counter >= x1-x2) {
            addDot(x2 + counter, y, WHITE_BITS);
	    counter++;
	}
    }
}

void drawVerticalLine(int x, int y1, int y2) {
    int counter;
    if(y2 >= y1){
        counter = 0;
        while(counter >= y2-y1) {
            addDot(x, y1 + counter, WHITE_BITS);
            counter++;
    }
	}
    else {
        counter = 0;
        while(counter >= y1-y2) {
            addDot(x, y2 + counter, WHITE_BITS);
	    counter++;
	}
    }
}

void drawHorizontalThicken(int scale, int x1, int x2, int y) {
    int counter = 0;
    while(counter <= scale) {
        drawHorizontalLine(x1, x2, y + counter);
	counter++;
    }
}

void drawVerticalThicken(int scale, int x, int y1, int y2) {
    int counter = 0;
    while(counter <= scale) {
        drawVerticalLine(x + counter, y1, y2);
	counter++;
    }
}

//All these digits will be in a default 3x5 box that they are draw in,
//but the scale of these can be modified
//The coordinates all refer to the top righthand corner
void drawZero(int x, int y, int scale) {

    //There are 6 key points for drawing, (0,0), (0, max),  (0, midpoint), (max, midpoint), (max, 0), and (max, max)
    //max is equal to scale*x_max or scale*y_max depending on which dimension its in
    //which are 3 and 5 respectively. The midpoint is therefore (y_max + 1)/2
    //Additionally, there should be 1xscale copies of each line, effectively thickening the line
    
    //Draws from (0,0) to (max, 0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (max, 0) to (max, max)
    drawVerticalThicken(scale, (x + scale*3), y, (y + scale*5));

    //Draws from (max, max) to (0, max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
	
    //Draws from (0, max) to (0,0)
    drawVerticalThicken(scale, x, (y + scale*5), y);
}

void drawOne(int x, int y, int scale) {
    //Draw from (0, 0) to (0, max)
    drawVerticalThicken(scale, x, y, (y + scale*5));
}

void drawTwo(int x, int y, int scale) {
    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (0,0) to (0, midpoint)
    //To find the midpoint, we can add 1 and divide by 2
    drawVerticalThicken(scale, x, y, (y + (scale*5 + 1)/2));

    //Draws from (0, midpoint) to (max, midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (max, midpoint) to (max, max)
    drawVerticalThicken(scale, x + scale*3,  (y + (scale*5 + 1)/2),  (y + scale*5));

    //Draws from (max, max) to (0, max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
}

void drawThree(int x, int y, int scale) {
    //Draws from (0,0) to (max, 0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (0,0) to (0, max)
    drawVerticalThicken(scale, x, y, (y + scale*5));

    //Draws from (0, midpoint) to (max, midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (0, max) to (max, max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
}

void drawFour(int x, int y, int scale) {
    //Draws from (0,0) to (0,max)
    drawVerticalThicken(scale, x, y, (y + scale*5));

    //Draws from (0,midpoint) to (max, midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (max,0) to (max, midpoint)
    drawVerticalThicken(scale, (x + scale*3), y, (y + (scale*5 + 1)/2));
}

void drawFive(int x, int y, int scale) {
    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (max,0) to (max,midpoint)
    drawVerticalThicken(scale, (x + scale*3), y, (y + (scale*5 + 1)/2));

    //Draws from (max,midpoint) to (0,midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (0,midpoint) to (0,max)
    drawVerticalLine(x, (y + (scale*5 + 1)/2), (y + scale*5));

    //Draws from (0,max) to (max,max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
}

void drawSix(int x, int y, int scale) {
    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (max,0) to (max,max)
    drawVerticalThicken(scale, (x + scale*3), y, (y + scale*5));

    //Draws from (max, midpoint) to (0,midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (0, midpoint) to (0,max)
    drawVerticalLine(x, (y + (scale*5 + 1)/2), (y + scale*5));

    //Draws from (0,max) to (max,max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
}

void drawSeven(int x, int y, int scale) {
    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (0,0) to (0,max)
    drawVerticalThicken(scale, x, y, (y + scale*5));
}

void drawEight(int x, int y, int scale) {
    //Draws from (0,0) to (0,max)
    drawVerticalThicken(scale, x, y, (y + scale*5));

    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (0,midpoint) to (max,midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (0,max) to (max,max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));

    //Draws from (max,0) to (max,max)
    drawVerticalThicken(scale, (x + scale*3), y, (y + scale*5));
}

void drawNine(int x, int y, int scale) {
    //Draws from (0,0) to (0,max)
    drawVerticalThicken(scale, x, y, (y + scale*5));

    //Draws from (0,0) to (max,0)
    drawHorizontalThicken(scale, x, (x + scale*3), y);

    //Draws from (max,0) to (max, midpoint)
    drawVerticalThicken(scale, (x + scale*3), y, (y + (scale*5 + 1)/2));

    //Draws from (max, midpoint) to (0, midpoint)
    drawHorizontalThicken(scale, x, (x + scale*3), (y + (scale*5 + 1)/2));

    //Draws from (0,max) to (max,max)
    drawHorizontalThicken(scale, (x + scale*3), x, (y + scale*5));
}

void render(void) {
    //Draw all the data onto the rendering function.
    drawPaddleRight(paddle_A.y);
    drawPaddleLeft(paddle_B.y);
    drawBall(ball.x, ball.y);
	
    int row = 0;

    while(row < 16) {
        gpio_clear(GPIOC, GPIO8); //Shut the latch so we can load in the data
        select_row(row);

        for (int i = 0; i < 192; i++) {
            gpio_set(GPIOC, GPIO7); //sets clock high

            if (renderingData[row][i] == 0) {
                gpio_clear(GPIOC, GPIO6); //sets pixel to 0
            } else {
                gpio_set(GPIOC, GPIO6); //sets pixel to 1
            }
            
            gpio_clear(GPIOC, GPIO7); //sets clock low
        }

        gpio_set(GPIOC, GPIO8); //Open the latch to show what is in the memory
        row++; 
    }

    for (volatile unsigned int tmr=33333; tmr > 0; tmr--); //approx 30 frames per second

}

void onGoal(void) {
    drawScores(); //alter panel data to show scores for each player.
    render(); //draw new frame.
    for (volatile unsigned int tmr=2e6; tmr > 0; tmr--); //Sleep for 2 seconds
    winner = isGameOver();
    justScored = 0;

    switch (winner)
    {
    case 'A':
        onGameOver();
        break;
    case 'B':
        onGameOver();
        break;
    default:
        break;
    }
    setupBall();
    setupPaddles();
    render(); //draw new frame for players to play next point


}

char isGameOver(void) {
    if (paddle_A.score == 9){
        return 'A';
    } else if (paddle_B.score == 9) {
        return 'B';
    } else {
        return 'X';
    }
}

void onGameOver() {
    if (winner == 'A'){
        winner = 'X';
        //flash score for A
        //do cool stuff
    } else if (winner == 'B') {
        winner = 'X';
        //flash score for B
        //do cool stuff
    } else {
        printf("Somethign went very wrong.");
        assert(1 == 0);
    }

    //wait for celebration to finish
    //reset environment for next game to be played
    paddle_A.score = 0;
    paddle_B.score = 0;
    setupBall();
    setupPaddles();
    render();

}

void main(void) {

    //Preparation to start game.
    initialSetup();

    while (winner == 'X') {
        readInput();
        update();
        render();
        winner = isGameOver();
    }

    onGameOver();
    main();
}
