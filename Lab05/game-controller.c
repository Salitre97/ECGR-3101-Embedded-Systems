/*
 * Name:        Cristian Salitre
 * Date:        08/08/2023
 * Assignment:  Lab05
 * Youtube:     https://youtu.be/W_7rFZ9b3is
 *
 */




#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

#define MCLK_FREQ_HZ    1000000
#define BUTTON_DELAY    150

uint8_t readADC();
void initADC();
void initGPIO();
void init();
void UART_Transmit(uint8_t data);
void initUart1();
void initClocks();
bool leftButtonPressed();
bool rightButtonPressed();
void delay_ms(uint16_t milliSeconds);
void transmitData(uint8_t data);


bool rightButtonFlag = false;
bool leftButtonFlag = false;

uint8_t data = 0;

int main(void)
{
    init();
    while(1){
        data = readADC();

        rightButtonFlag = rightButtonPressed();
        leftButtonFlag = leftButtonPressed();

        transmitData(data);

        // resets button flags
        leftButtonFlag = false;
        rightButtonFlag = false;
    }
}


void initClocks(){
    // Set XT1CLK as ACLK source
    CSCTL4 |= SELA__XT1CLK;
    // Use external clock in low-frequency mode
    CSCTL6 |= XT1BYPASS_1 | XTS_0;
}
void initUart1(){
    // Configure UART pins

    // set 2-UART pin as second function
    P4SEL0 |= BIT3 | BIT2;
    P4SEL1 &= ~(BIT3 | BIT2);

    // set UART pin as second function
    P1SEL0 |= BIT1;
    P1SEL1 &= ~BIT1;

    // Configure UART
    // Hold UART in reset state
    UCA1CTLW0 = UCSWRST;
    // CLK = ACLK
    UCA1CTLW0 |= UCSSEL__ACLK;
    // Baud Rate calculation
    // 32768/(9600) = 3.4133
    // Fractional portion = 0.4133
    // User's Guide Table 17-4: 0x92

    // 32768/9600
    UCA1BR0 = 3;
    UCA1BR1 = 0;
    //0x9200 is UCBRSx = 0x92
    UCA1MCTLW |= 0x9200;
    // Release reset state for operation
    UCA1CTLW0 &= ~UCSWRST;
}
void UART_Transmit(uint8_t data) {
    // Configure UART1 to transmit data
    while (!(UCA1IFG & UCTXIFG));   // Wait for UART transmit buffer to be empty

    UCA1TXBUF = data;   // Send data over UART

}
void init(){
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    __enable_interrupt();

    initGPIO();
    initClocks();
    initUart1();
    initADC();
}
bool leftButtonPressed(){
    return (P2IN & BIT3) == 0x00;
}
bool rightButtonPressed() {
    return (P4IN & BIT1) == 0x00;
}
void initGPIO(){
    // Configure button s1
    P4DIR &= ~BIT1;
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IE |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;

    // Configure button s2
    P2DIR &= ~BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IE |= BIT3;
    P2IES |= BIT3;
    P2IFG &= ~BIT3;
}
void initADC(){
    // Configure ADC12
    ADCCTL0 |= ADCSHT_2 | ADCON;
    // ADCON S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;
    // ADCCLK = MODOSC; sampling timer
    ADCCTL2 &= ~ADCRES;
    // 8-bit conversion results
    ADCCTL2 |= ADCRES;
    // Internal channel 1 select; vref=3.3v
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_0;
}

uint8_t readADC(){
    // Sampling and conversion start
    ADCCTL0 |= ADCENC | ADCSC;
    // Wait for conversion to finish
    while(ADCCTL1 & ADCBUSY);
    return ADCMEM0;
}

void delay_ms(uint16_t milliSeconds){
    volatile uint16_t i;
    for(i = 0; i < milliSeconds; i++){
        __delay_cycles(MCLK_FREQ_HZ/1000);
    }
}
void transmitData(uint8_t data) {
    if (rightButtonFlag || leftButtonFlag) {
        UART_Transmit(255);
    }
    // 255 is the shoot command in the python code
    if(data != 255) {
    UART_Transmit(data);
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2ISR(){
    delay_ms(150);
    leftButtonFlag = (P2IN & BIT3) == 0x00;

    P2IFG &= ~BIT3;
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4ISR(){
    delay_ms(150);

    rightButtonFlag = (P4IN & BIT1) == 0x00;

    P4IFG &= ~BIT1;
}
