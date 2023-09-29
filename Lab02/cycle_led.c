#include <msp430.h> 
#include <stdbool.h>

#define LED1_DIR     P1DIR // Red LED
#define LED2_DIR     P6DIR // Green LED
#define LED1_OUT     P1OUT
#define LED2_OUT     P6OUT
#define LED1_PIN     BIT0
#define LED2_PIN     BIT6

void init();
bool s1buttonPressed();
void setRedLED(bool state);
void setGreenLED(bool state);
void setBothLEDs(bool state);
void cycleLEDs(int* cycle);
typedef enum {RED,GREEN, BOTH} state_t;

int main(void){
     init();

    int cycle = 0;

    while(1){
        if(s1buttonPressed()){
        cycleLEDs(&cycle);
        }
    }
}

void init(){
    // Stop watch dog timer
    WDTCTL = 0x5A80;
    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 = 0xFFFE;

    // Button Configurations
    // Set P4.1 as input(all others as input due to being 0)
    P4DIR &= ~0x00;
    // Enable P4.1 pull-up resistor
    P4REN |= BIT1;
    // Set P4.1 as high to configure pull-up
    P4OUT |= BIT1;

    // LED configurations
    // Set P1.0 as output (all others as input due to being 0)
     LED1_DIR |= LED1_PIN;
     // Set P6.6 as output (all others as  input due to being 0)
     LED2_DIR |= LED2_PIN;
}

bool s1buttonPressed(){
    return (P4IN & BIT1)== 0x00;
}


void setRedLED(bool state){
    if(state){
        LED1_OUT |= LED1_PIN; // P1OUT = P1OUT | BIT0
    } else {
        LED1_OUT &= ~LED1_PIN; // P1OUT = P1OUT & ~BIT0
    }
}
void setGreenLED(bool state){
    if(state){
        LED2_OUT  |= LED2_PIN;
    } else {
        LED2_OUT &= ~LED2_PIN;
    }
}
void setBothLEDs(bool state){
    if(state){
        LED2_OUT |= LED2_PIN;
        LED1_OUT |= LED1_PIN;
    } else {
        LED2_OUT &= ~LED2_PIN;
        LED2_OUT &= ~LED1_PIN;

    }
}
void cycleLEDs(int* cycle){

    switch(*cycle){
    case 0:
        setRedLED(true);
        setGreenLED(false);
        break;
    case 1:
        setRedLED(false);
        setGreenLED(true);
        break;
    case 2:
        setBothLEDs(true);
        break;

    }
    if(s1buttonPressed()){
        __delay_cycles(500000);
        *cycle = *cycle + 1;
    }

    if(*cycle > 2){
        *cycle = 0;
    }
}
