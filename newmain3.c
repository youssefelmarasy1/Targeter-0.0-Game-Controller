/* 
 * File:   newmain3.c
 * Author: youss
 *
 * Created on November 18, 2022, 7:28 PM
 */


#define F_CPU 14745600UL
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include "hd44780.h"
#include "lcd.h"
#include "defines.h"
#include "uart.h"
#include <string.h>

/*
 * 
 */
/*This is the code for the Fuse bits that allow the MCU to work with the external crystal*/
FUSES = {
    .low = 0xFF, // LOW {SUT_CKSEL=EXTXOSC_8MHZ_XX_16KCK_14CK_65MS, CKOUT=CLEAR, CKDIV8=CLEAR}
    .high = 0xD9, // HIGH {BOOTRST=CLEAR, BOOTSZ=2048W_3800, EESAVE=CLEAR, WDTON=CLEAR, SPIEN=SET, DWEN=CLEAR, RSTDISBL=CLEAR}
    .extended = 0xFF, // EXTENDED {BODLEVEL=DISABLED}
};

LOCKBITS = 0xFF; // {LB=NO_LOCK, BLB0=NO_LOCK, BLB1=NO_LOCK}

/*The following two functions are for sending and receiving data from the UART*/

/*Note that they're copied from the ATMega 328P datasheet*/
void USART_Transmit(unsigned char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char USART_Receive(void) {
    /* Wait for data to be received */
    while (!(UCSR0A & (1 << RXC0)))
        ;
    /* Get and return received data from buffer */
    return UDR0;
}

/*Setting up the file streams for the lcd and the uart, respectively*/
FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void) {

    uart_init();
    lcd_init();

    char g_name[20], handle[12], handle_new[15], g_name_new[16], controller_resp[9], score_new[4], ammo_new[8];
    uint16_t vx, vy; //x and y positions
    uint16_t x_perc, y_perc; //x and y percentages
    uint8_t button; //button status

    DDRB &= ~(1 << PB3); //ensuring PB2 is an input (Pushbutton)
    PORTB |= (1 << PB3); // turning on the pullup for the pushbutton pin

    /**/
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0); //SET VREF =AVcc
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D); //Disabling the digital inputs

    fprintf(&uart_str, "Q\n"); //Sending a query to the game 
    /*This for loop is for receiving the un-filtered game name from the game using the UART*/

    for (uint8_t i = 0; i < 16; i++) {
        g_name[i] = USART_Receive();
    }

    fprintf(&uart_str, "HYou\n"); //sending the hard-coded player name to the game using the UART

    /*This is for receiving the un-filtered handle back from the game*/

    for (uint8_t i = 0; i < 11; i++) {
        handle[i] = USART_Receive();
    }

    /*This is for filtering the handle from the original received string */
    for (uint8_t j = 3; j < 9; j++) {
        handle_new[j - 3] = handle[j];

    }

    handle_new[4] = 0; //EOF-equivalent termination of the array
    /*This is for receiving the game name back from the game*/
    for (uint8_t k = 0; k < 14; k++) {
        g_name_new[k] = g_name[k];

    }

    g_name_new[14] = 0; //EOF-equivalent termination of the array

    while (1) {
        /*The following part is for the ADC
         *The approach I followed is that basically I had pins ADC0 and ADC1
         *connected as my vx and vy, respectively. So what I did is for selecting
         * the ADC0 pin, you need to disable MUX0 in the ADMUX register, so I 
         * did that in the first line of that chunk of code, and then I started
         * the ADC conversion by setting the ADSC flag in the ADCSRA register
         * to 1. Then I said that while the ADC conversion is ongoing, wait.
         * And then, vx would be that current 10-bit value in the ADC register.
         * For vy, I did the same approach except that this time, I set the MUX0
         * to 1 i.e I enabled MUX0 instead of disabling it.
         */
        ADMUX &= ~(1 << MUX0);
        ADCSRA |= (1 << ADSC);
        while ((ADCSRA)&(1 << ADSC));
        vx = ADC;

        ADMUX |= (1 << MUX0);
        ADCSRA |= (1 << ADSC);
        while ((ADCSRA)&(1 << ADSC));
        vy = ADC;

        /*These two calculations are for calculating the x and y coordinates
         from the vx and vy values*/
        x_perc = (int) (((double) vx / 1023)*200) - 100;
        y_perc = (int) (((double) vy / 1023)*200) - 100;

        hd44780_wait_ready(false); //This is a busy wait
        hd44780_outcmd(HD44780_HOME);

        if (!(PINB & (1 << PB3))) { // if the button is pressed
            button = 1; //buton status = 1(on)

        } else {
            button = 0; //button status = 0 (off)
        }


        /*Sending the joystick's x and y-axes percentages and the 
         * button status to the game using the UART */
        fprintf(&uart_str, "C%+04d%+04d%d\n", x_perc, y_perc, button);


        /*The following for loop is for receiving the controller response string 
         * from the game using the UART
         */
        for (uint8_t l = 0; l < 9; l++) {
            controller_resp[l] = USART_Receive();
        }

        /*This for loop is for filtering out the score out of the response
         string that I got from the game.
         */
        for (uint8_t m = 0; m < 3; m++) {
            score_new[m] = controller_resp[m + 4]; //This is the letter L+1   
        }
        score_new[3] = 0; //EOF-equivalent termination of the array

        /*The following part is for manually, picking out the characters that 
         * make up the ammunition left since the server response string 
         * was completely unordered*/
        ammo_new[0] = controller_resp[8];
        ammo_new[1] = controller_resp[0];
        ammo_new[2] = controller_resp[1];
        ammo_new[3] = 0; //EOF-equivalent termination of the array

        /*This fprintf statement is for printing all the parameters that needed
         to be printed on the LCD screen. The "\x1b\xc2" is for shifting to the 
         * 2nd line, 2nd position.*/
        fprintf(&lcd_str, "%s \x1b\xc2%s %s %s", g_name_new, handle_new, ammo_new, score_new);

        /*This is a 100-ms delay to make sure that the controlller information
         gets sent as quickly as the game can handle but without it actually
         disconnecting*/
        _delay_ms(100);

        /*This is for turning off the cursor, this eliminates the 
         * flickering on the LCD*/
        fprintf(&lcd_str, "\x1b\x0c");

    }

    return (EXIT_SUCCESS);
}

