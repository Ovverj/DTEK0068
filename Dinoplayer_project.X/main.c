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
#include <avr/sleep.h>

/* These both methods are copied from 
*http://ww1.microchip.com \
* /downloads/en/Appnotes/TB3209-Getting-Started-with-ADC-90003209A.pdf
* with minor corrections to right ports and pins
*/
void adc0_init(void);
uint16_t adc0_read(void);
// Saving the value of LDR in this variable
uint16_t adc_raw;
// Saving button status on this value to determine if running or sleeping
volatile uint16_t status;

void adc0_init(void)
{
    /* Disable digital input buffer */
    PORTE.PIN0CTRL &= ~PORT_ISC_gm;
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    /* Disable pull-up resistor */
    PORTE.PIN0CTRL &= ~PORT_PULLUPEN_bm;

    // Setting Vref to 2,5V
    VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;
    ADC0.CTRLC = ADC_PRESC_DIV4_gc;    /* CLK_PER divided by 4 */
    
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
    
    PORTF.DIRCLR = PIN6_bm; // Set onboard button as Input
    
    PORTF.PIN6CTRL = PORT_ISC_FALLING_gc; //set button as trigger for intflag
    
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc); //enable sleep mode
    
    sei(); //enable interrupts
    
    // Setting status to 0 so device is running from as code is ran
    status = 0;
    
    /* Checking the LDR value at the beginning and setting
     * limit value in variable max for servo to know, when to rotate to
     */
    adc_raw = adc0_read();
    uint16_t max = adc_raw + 12;
    
    while (1) 
    {
        //Checking if button is pressed to set status to 1 to put device asleep
        if(status == 1)
        {
            sleep_mode();
        }
        // Else status is 0, device is running
        else
        {
            // Checking the LDR value
            adc_raw = adc0_read();
        
            /* If LDR value is more than limit value Max, Servo rotates
            * and clicks space.
            */
            if(adc_raw > max)
            {
                /*rotate servo to click, value depends on keyboard used
                *value 245 rotates the servo a couple degrees from flat (295)
                *Thus pressing spacebar and jumping with the dino
                */
                TCA0.SINGLE.CMP2BUF = 245;
            
            }
            /*Else rotate Servo back to this position
            * value can be changed depending on keyboard
            */
            else
            {
                TCA0.SINGLE.CMP2BUF = 295;
            }
        }
        
    }
}
/* Interrupt service routine, used to set device on sleep or running by
 * pressing button onboard. Device starts running and by pressing button
 * you can set device asleep. Pressing button again wakes the device up.
 */
ISR(PORTF_PORT_vect)
{
    if(status == 0)
    {
        //set status value to 1 to enable sleep mode
        status = 1;
    }
    else
    {
        //set status to 0 to run the dinoplayer
        status = 0; 
    }
    PORTF.INTFLAGS = PIN6_bm; //clear the intflag
}