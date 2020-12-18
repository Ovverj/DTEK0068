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
void ADC0_init(void);
uint16_t ADC0_read(void);
// Saving the value of LDR in this variable
uint16_t adc_raw;


void ADC0_init(void)
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



uint16_t ADC0_read(void)
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
    ADC0_init();
    
    
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
    //Read ldr data
    adc_raw = ADC0_read();
    //Create autosetup for ldr trigger
    uint16_t max;
    max = adc_raw + 5;
    
    
    while (1) 
    {

        // Checking the LDR value
        adc_raw = ADC0_read();

        //detect cactus
        if(adc_raw > max)
        {
            //rotate servo to click
            TCA0.SINGLE.CMP2BUF = 385;
                 
        }
        else
        {
            //rotate servo to return
            TCA0.SINGLE.CMP2BUF = 360;

        }        
    }
}
