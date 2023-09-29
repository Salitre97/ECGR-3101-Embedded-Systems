/**
 * main.c
 *
 * Name:        Cristian Salitre
 * Date:        07/31/2023
 * Assignment:  Lab 4 - Ultrasonic Alarm
 * Youtube:     https://youtu.be/0BucV4ScR3c
 *
 */

//#include "buzzer_sensor.h"
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define MCLK_FREQ_HZ    1000000
#define TOLERANCE       0.08

typedef enum {IDLE, ARMING, ARMED} state_t;

float distance = 0;

void init();
void initGPIO();
float getDistance();
void initTimerB0();
void initTimers();
void initClocks();
bool isbuttonPressed();
void initUltrasonic_sensor();
void stopTimerB0();
state_t runIdle();
state_t runArming();
state_t runArmed();
void reset();

int main(void)
{
    state_t currentState = IDLE;
    state_t nextState = currentState;

    init();

    while(1){
        switch(currentState){
            case IDLE:
                nextState = runIdle();
            case ARMING:
                nextState = runArming();
            case ARMED:
                nextState = runArmed();
        }
        currentState = nextState;
    }
}
void init(){
    // Stop watch dog timer
    WDTCTL = 0x5A80;
    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 = 0xFFFE;

    initGPIO();
    initTimers();
}
void initTimers(){
    TB0CTL = TBSSEL__SMCLK | MC__STOP;
}
void initGPIO(){
    // Button config P2.3
    P2DIR = 0x00;
    P2REN |= BIT3;
    P2OUT |= BIT3;

    // Buzzer config P6.2
    P6DIR |= BIT2;
    P6OUT &= ~BIT2;
}
float getDistance(){
    volatile float count = 0;
    TB0R = 0;

    initUltrasonic_sensor();

    // wait until signal goes back to high
    while((P6IN & BIT3) == 0x00);
        //initTimerB0
        initTimerB0();

        //timer counts until signal goes back to
    while(P6IN & BIT3);
        // Stop timer
        stopTimerB0();
        count = TB0R;

    return ((count/MCLK_FREQ_HZ)*343)/2; // speed of sound 343
}

void initTimerB0(){
    // Configure Timer B0 to use SMCLK and be stopped
    TB0CTL = TBSSEL__SMCLK | MC__CONTINOUS;
}

void initUltrasonic_sensor(){
    // configure sensor as output and set pin to high
    P6DIR |= BIT3;
    P6OUT &= ~BIT3;

    // delay 10 micro-seconds
    __delay_cycles(10);

    P6OUT |= BIT3;
    __delay_cycles(10);

    // set pin to low and configure pin as input
    P6OUT &= ~BIT3;
    P6DIR &= ~BIT3;
}
void stopTimerB0(){
    //
    TB0CTL = TBSSEL__SMCLK | MC__STOP;
}
state_t runIdle(){
    while(!isbuttonPressed());

    return ARMING;
}
state_t runArming(){
    distance = getDistance();

    return ARMED;
}
state_t runArmed(){
    float newDistance = 0.0;

    do {
        newDistance = getDistance();
        if(fabs(newDistance - distance) > TOLERANCE){
            P6OUT |= BIT2; // Turn on buzzer
        }
    } while(!isbuttonPressed());
    reset();
    return IDLE;
}
void reset(){
    // Turn off buzzer
    P6OUT &= ~BIT2;
}
bool isbuttonPressed(){
    if((P2IN & BIT3) == 0x00){
        //debounce
        __delay_cycles(300000);
        return true;
    }
    return false;
}


