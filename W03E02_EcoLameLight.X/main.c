/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 15 November 2020, 23:37
 */


#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

int main(void) {
    /* Replace with your application code */
    // Configure port F pins so that LED is output and the rest remain as input
    PORTF.DIRCLR = PIN6_bm;
    
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc;
    
    PORTF.DIRSET = PIN5_bm;
    
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    
    sei();
    
    while (1) 
    {
        sleep_mode();
    }
}
ISR(PORTF_PORT_vect){
        
            //Turn the led on
    if(PORTF.IN & 0b01000000)
        {
            //Turn off the led
            PORTF.OUT = 0b00100000;
        }
        else
        {
            //Turn the led on
            PORTF.OUT = 0b00000000; 
        }                   
    }
