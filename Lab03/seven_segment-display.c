/**
 * Name:        Cristian Salitre
 * Date:        07/06/2023
 * Assignment:  Lab03
 * YouTube:     https://youtu.be/WA7-fQvy9jk
 *
 * When this code is compiled and begins to run, the train should run through different states depending on inputs.
 * Starting at STOPPED state at the start of the program.  Train will NOT be moving.
 * If SW1 is pressed then the train will start to run.
 * If SW1 is pressed again the train will stop.  Return to STOPPED state.
 * If SW2 is pressed at any point in time, it will run in reverse.
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "Four_Digit_Display.h"

#define MCLK_FREQ_HZ    1000000
#define SEG_DELAY_MS    300
#define BUTTON_DELAY    150
#define MAX_POSITIONS   11
#define MIN_POSITIONS   0


bool S1_buttonFlag = false;
bool S2_buttonFlag = false;

typedef enum {STOPPED, RUNNING, REVERSE} state_t;

typedef struct{
    bool isS1_ButtonPressed;
    bool isS2_ButtonPressed;
    int ledPosition;
} inputs_t;

state_t runStoppedState(inputs_t* inputs);
state_t runRunningState(inputs_t* inputs);
state_t runReverseState(inputs_t* inputs);

void init();
bool readButtonPress_S1();
bool readButtonPress_S2();
void setRedLED(bool state);
void setGreenLED(bool state);
void delay_ms(uint16_t milliSeconds);
void drawTrain(int position);

int main(void){

    state_t currentState = STOPPED;
    state_t nextState = currentState;


    inputs_t inputs;
    inputs.isS1_ButtonPressed = false;
    inputs.isS2_ButtonPressed = false;
    inputs.ledPosition = 0;

    init();

    while(1){

        switch(currentState){
        case STOPPED:
            nextState = runStoppedState(&inputs);
            break;
        case RUNNING:
            nextState = runRunningState(&inputs);
            break;
        case REVERSE:
            nextState = runReverseState(&inputs);
            break;
        }

        inputs.isS1_ButtonPressed = S1_buttonFlag;
        inputs.isS2_ButtonPressed = S2_buttonFlag;
        S1_buttonFlag = false;
        S2_buttonFlag = false;
        currentState = nextState;


    }
}
void init(){
    // Stop watch dog timer
    WDTCTL = 0x5A80;
    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 = 0xFFFE;

    four_digit_init();
    four_digit_set_brightness(BRIGHT_TYPICAL);
    four_digit_set_point(POINT_ON);

    //Button configurations
    P2DIR &= ~BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IE |= BIT3;
    P2IES |= BIT3;
    P2IFG &= ~BIT3;

    P4DIR &= ~BIT1;
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IE |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;


    // LED configurations
    P1DIR |= BIT0;
    P6DIR |= BIT6;

    __enable_interrupt();
}
bool readButtonPress_S1(){
    if((P4IN & BIT1) == 0x00){
        delay_ms(BUTTON_DELAY);
        return (P4IN & BIT1) == 0x00;
    }
    return false;
}
bool readButtonPress_S2(){
    if((P2IN & BIT3) == 0x00){
        delay_ms(BUTTON_DELAY);
        return (P2IN & BIT3) == 0x00;
    }
    return false;
}
void setRedLED(bool state){
    if(state){
        P1OUT |= BIT0;
    } else {
        P1OUT &= ~BIT0;
    }
}
void setGreenLED(bool state){
    if(state){
        P6OUT |= BIT6;
    } else {
        P6OUT &= ~BIT6;
    }
}
state_t runStoppedState(inputs_t* inputs){
    drawTrain(inputs->ledPosition);
    setRedLED(true);
    setGreenLED(false);

    if(inputs->isS1_ButtonPressed){
        return RUNNING;
    }
    else if (inputs->isS2_ButtonPressed){
        return REVERSE;
    } else {
    return STOPPED;
    }
}
state_t runRunningState(inputs_t* inputs){
    drawTrain(inputs->ledPosition);
    inputs->ledPosition++;
    if(inputs->ledPosition > MAX_POSITIONS){
        inputs->ledPosition = 0;
    }
    delay_ms(SEG_DELAY_MS);
    setRedLED(false);
    setGreenLED(true);

    if(inputs->isS1_ButtonPressed){
        inputs->ledPosition = 0;
        return STOPPED;
    }
    else if(inputs->isS2_ButtonPressed){
        inputs->ledPosition = MAX_POSITIONS;
        return REVERSE;
    } else {
        return RUNNING;
    }
}
state_t runReverseState(inputs_t* inputs){
    drawTrain(inputs->ledPosition);
    inputs->ledPosition--;

    if(inputs->ledPosition < MIN_POSITIONS){
        inputs-> ledPosition = MAX_POSITIONS;
    }
    delay_ms(SEG_DELAY_MS);
    setRedLED(false);
    setGreenLED(true);

    if(inputs->isS1_ButtonPressed){
        inputs->ledPosition = 0;
        return STOPPED;
    } else {
    return REVERSE;
    }
}
void delay_ms(uint16_t milliSeconds){
    volatile uint16_t i;
    for(i = 0; i < milliSeconds; i++){
        __delay_cycles(MCLK_FREQ_HZ/1000);
    }
}


void drawTrain(int position){

    switch(position){
    case 0:
        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_D);
        display_segment(POS_2, SEG_D);
        display_segment(POS_3, SEG_D);

        break;
    case 1:
        display_segment(POS_3, 0x00);
        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_D | SEG_E);
        display_segment(POS_2, SEG_D);

        break;
    case 2:
        display_segment(POS_2, 0x00);
        display_segment(POS_3, 0x00);
        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_D | SEG_E | SEG_F);

        break;
    case 3:
        display_segment(POS_2, 0x00);
        display_segment(POS_3, 0x00);
        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_A | SEG_E | SEG_F);

        break;
    case 4:
        display_segment(POS_3, 0x00);
        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_A | SEG_F);
        display_segment(POS_2, SEG_A);


        break;
    case 5:

        display_segment(POS_4, 0x00);
        display_segment(POS_1, SEG_A);
        display_segment(POS_2, SEG_A);
        display_segment(POS_3, SEG_A);

        break;
    case 6:
        display_segment(POS_1, 0x00);
        display_segment(POS_2, SEG_A);
        display_segment(POS_3, SEG_A);
        display_segment(POS_4, SEG_A);
        break;
    case 7:
        display_segment(POS_1, 0x00);
        display_segment(POS_2, 0x00);
        display_segment(POS_3, SEG_A);
        display_segment(POS_4, SEG_A | SEG_B);
        break;
    case 8:
        display_segment(POS_1, 0x00);
        display_segment(POS_2, 0x00);
        display_segment(POS_3, 0x00);
        display_segment(POS_4, SEG_A | SEG_B | SEG_C);
        break;
    case 9:
        display_segment(POS_1, 0x00);
        display_segment(POS_2, 0x00);
        display_segment(POS_3, 0x00);
        display_segment(POS_4, SEG_B | SEG_C | SEG_D);
        break;
    case 10:
        display_segment(POS_1, 0x00);
        display_segment(POS_2, 0x00);
        display_segment(POS_4, SEG_C | SEG_D);
        display_segment(POS_3, SEG_D);
        break;
    case 11:
        display_segment(POS_1, 0x00);
        display_segment(POS_4, 0x00);
        display_segment(POS_4, SEG_D);
        display_segment(POS_3, SEG_D);
        display_segment(POS_2, SEG_D);

        break;
    }
}
#pragma vector=PORT2_VECTOR
__interrupt void Port_2ISR(){
    delay_ms(150);
    S2_buttonFlag = (P2IN & BIT3) == 0x00;

    P2IFG &= ~BIT3;
}
#pragma vector=PORT4_VECTOR
__interrupt void Port_4ISR(){
    delay_ms(150);
    S1_buttonFlag = (P4IN & BIT1) == 0x00;

    P4IFG &= ~BIT1;
}



