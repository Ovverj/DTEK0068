/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 24 November 2020, 15:33
 *
 * some notes from yours truly:
 * Really liked this exercise, finally felt like I got something figured out
 * Thank you for the excellent advice in W04E01-advice.pdf
 * It really helped me to understand the code for AVR rather than 
 * using trial and error to get somewhat desired outcomes.
 * 
 */
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <avr/sleep.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>

/*
 * Init 2 integers: count counter and instruct for counter state
 * instructions said its ok to start from 0, not sure if its desired
 * I commented the line below that can be removed to start the counter from 1
 */

uint16_t count = 1;
uint8_t instruct = 0;

/*
 * Copied code from microchips github to allow printing to terminal
 * used for numeric values of the stopwatch and "start", "stop" etc
 */

static void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{ 
    USART0_sendChar(c);
    return 0; 
}

static FILE USART_stream = \
FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

static void USART0_init(void)
{
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600); 
    
    USART0.CTRLB |= USART_TXEN_bm;  
    
    stdout = &USART_stream;
}
/*
 * Copied code ends here
 */

int main(void)
{
    /*
     * init pins for led and button
     * set button to send flag when pressed
     * also turn off led after init to avoid undesired light
     */
    PORTF.DIRCLR = PIN6_bm;
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc;
    
    PORTF.DIRSET = PIN5_bm;
    PORTF.OUT = PIN5_bm;
    
    /*
     * Init RTC counter with 1 second delay
     */
    
    
    RTC.PITINTCTRL = RTC_PI_bm;
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;
    
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    
    sei();
    
    USART0_init();
        
    /*
     * Set the devices default status as asleep
     */
    while (1) 
    {
        sleep_mode();
    }
}
/*
 * Configure button press flags to set instruct value accordingly
 * Shuts down led when stopped to avoid dependency 
 * regarding the led state on when stopped
 * This is mostly cosmetic, didn't like the fact that the use is not uniform
 * if stopped on even or odd seconds.
 * 
 */
ISR(PORTF_PORT_vect)
{
        
    if(instruct == 1)
    {
        instruct = 2;
        PORTF.OUT = PIN5_bm;

    }
    else
    {
        /*
         * Reset occurs even if there is nothing to reset,
         * this is by design due to instructions, setting if statement as
         * if(count > 1) makes it so the reset only works when
         * there is something to reset.
         * Personally would prefer to use that, 
         * because why reset something that hasn't been changed.
         */
        if(instruct == 2)
        {
            instruct = 0;
            printf("RESET\r\n");
            count = 1;
        }
        else
        {
            printf("0\r\n"); //this can be removed to start counter from 1
            instruct = 1;
        }
    }
    PORTF.INTFLAGS = PORTF.INTFLAGS;
}
/*
 * Catch RTC flags to start the timer
 * each flag happens in interval of 1 sec and adds +1 to count after print
 * 
 */
ISR(RTC_PIT_vect)
{
    if(instruct == 1)
    {
        printf("%d\r\n", count++);
        RTC.PITINTFLAGS = RTC_PI_bm;
        PORTF.OUTTGL = PIN5_bm;
    }
    else
    {
        RTC.PITINTFLAGS = RTC_PI_bm;
    }   
}