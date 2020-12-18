/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 14 December 2020, 15:24
 */
#define F_CPU   3333333
#define RTC_PERIOD            (511)
#define USART0_BAUD_RATE(BAUD_RATE) \
((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#define SERVO_PWM_PERIOD   (0x1046)
#define SERVO_PWM_DUTY_NEUTRAL (0x0138)

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

int main(void) 
{
    ADC0_init();
    USART0_init();
    
    
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
    adc_raw = ADC0_read();
    uint16_t max;
    max = adc_raw + 32;
    
    
    while (1) 
    {

        // Checking the LDR value
        adc_raw = ADC0_read();
        printf("%d\r\n",adc_raw);

            if(adc_raw > max)
            {
                TCA0.SINGLE.CMP2BUF = 400;
                 
            }
            else
            {
                TCA0.SINGLE.CMP2BUF = 355;

            }        
    }
}
