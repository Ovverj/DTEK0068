/*
 * File:   main.c
 * Author: ovverj, jihjer, tbalva
 *
 * Created on 14 December 2020, 15:24
 */
#define F_CPU   3333333
#define RTC_PERIOD            (511)
#define SERVO_PWM_PERIOD   (0x1046)
#define SERVO_PWM_DUTY_NEUTRAL (0x0138)

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>


//init methods for ldr value reading
void adc0_init(void);
uint16_t adc0_read(void);
// Saving the value of LDR in this variable
volatile uint16_t adc_raw;



void adc0_init(void)
{
    /* Disable digital input buffer */
    PORTE.PIN0CTRL &= ~PORT_ISC_gm;
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    /* Disable pull-up resistor */
    PORTE.PIN0CTRL &= ~PORT_PULLUPEN_bm;

    VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;
    ADC0.CTRLC = ADC_PRESC_DIV4_gc;      /* CLK_PER divided by 4 */
                 /* Internal reference */
    
    ADC0.CTRLA = ADC_ENABLE_bm          /* ADC Enable: enabled */
               | ADC_RESSEL_10BIT_gc;   /* 10-bit mode */
    
    /* Select ADC channel */
    ADC0.MUXPOS  = ADC_MUXPOS_AIN8_gc;
}



uint16_t adc0_read(void)
{
    /* Start ADC conversion */
    ADC0.COMMAND = ADC_STCONV_bm;
    
    /* Wait until ADC conversion done */
    while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
    {
        ;
    }
    
    /* Clear the interrupt flag by writing 1: */
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    
    return ADC0.RES;
}
ISR(ADC0_RESRDY_vect)
{
    //Read LDR value to clear interrupt flag
    adc_raw = ADC0.RES;
}

int main(void) 
{
    adc0_init();
    
    
    // Route TCA0 PWM waveform to PORTB
    PORTMUX.TCAROUTEA |= PORTMUX_TCA0_PORTB_gc;
    // Set 0-WO2(PB2) as digital output
    PORTB.DIRSET = PIN2_bm;
    // Set TCA0 prescaler value to 16 (~208 kHz)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;
    // Set single-slop PWM generation mode
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    // Using double-buffering, set PWM period(20 ms)
    TCA0.SINGLE.PERBUF = SERVO_PWM_PERIOD;
    // Set initial servo arm position as neutral (0 deg)
    TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL;
    // Enable Compare Channel 2
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm;
    // Enable TCA0 peripheral
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
    
    //ADC cnfiguration, enable result ready interrupt
    ADC0.INTCTRL = ADC_RESRDY_bm;
    sei();
    //Create autosetup for ldr trigger
    uint16_t max;
    max = adc_raw + 5;  //this value is used to enable autosetup
                        //the value is derived from the white background
                        //of the game and when cactus comes near ldr
                        //this threshold of 5 is broken and engages jump
    
    
    while (1) 
    {

        // Checking the LDR value
        adc_raw = adc0_read();

        //detect cactus
        if(adc_raw > max)  
        {
            //rotate servo to click, value depends on keyboard used
            //value 385 rotates the servo a couple degrees from flat (360)
            //Thus pressing spacebar and jumping with the dino
            TCA0.SINGLE.CMP2BUF = 385;
                 
        }
        else
        {
            //rotate servo to return
            //the value 360 is used as neutral ground (flat)
            //When servo gets value 360 it is hovering on top of spacebar
            TCA0.SINGLE.CMP2BUF = 360;

        }        
    }
}
