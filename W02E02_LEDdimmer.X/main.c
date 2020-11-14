/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 14 November 2020, 18:42
 */

#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>



int main(void) {
   
    // Configure port F pins so that LED is output and the rest remain as input
    PORTF.DIR = 0b00100000;
    int i;
    int pwm_period(int duty){
        if (duty == 0){
            _delay_ms(2000);
            return 0;
        }
                if(i == 1){
                PORTF.OUT = 0b00000000;
                _delay_ms(5000);
                PORTF.OUT = 0b00100000;
                _delay_ms(5000);
                return 0;
                
                }
                if (i == 2){
                PORTF.OUT = 0b00000000;
                _delay_ms(10);
                PORTF.OUT = 0b00100000;
                _delay_ms(10);
                return 0;
                }
            
            
        }
    int a;
    void delay_us( int us ){
   for (int i = 0; i < us; i++)
   {
      _delay_us(1);
   }
}
    while (1) 
    {

        //IF button is not pressed, do something. 
        if(PORTF.IN & PIN6_bm){   
            /*
             * ideana tallentaa pulssin i arvo ja täten välkkymisen taajuus
             * näin saataisiin valo lukittua siihen kirkkauteen,
             * jossa pulssi on kun vapauttata napin
             * toteututs ei kuitenkaan toimi
             */
                PORTF.OUTTGL = PIN5_bm;
                delay_us(a * 7);
                PORTF.OUTTGL = PIN5_bm;
            
               
            
            }
        else{
            /*
             * nappia painaessa valo sykkii kuten soft blink toteutuksessa
             */
                for (int i = 255; i>0; i--){
                PORTF.OUTTGL = PIN5_bm;
                delay_us(i * 7);
                PORTF.OUTTGL = PIN5_bm;
                a = i;
            }                
               
            for(int i=1; i <= 255 ;i++ ){
                PORTF.OUTTGL = PIN5_bm;
                delay_us(i * 7);
                PORTF.OUTTGL = PIN5_bm;
                                a = i;


            }
        }
        
    }
}

