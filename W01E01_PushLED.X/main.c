/*
 * File:   main.c
 * Author: dtek0068-OVVERJ
 *
 * Created on 11 November 2020, 15:26
 */


#include <avr/io.h>

int main(void) {
   
    // Configure port F pins so that LED is output and the rest remain as input
    PORTF.DIR = 0b00100000;
    
    while (1) 
    {
        //IF button is pressed, do something. 
        if(PORTF.IN & 0b01000000)
        {
            //Turn on the led
            PORTF.OUT = 0b00100000;
        }
        else
        {
            //Turn the led off
            PORTF.OUT = 0b00000000;
        }
    }
}