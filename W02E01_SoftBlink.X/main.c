/*
 * File:   main.c
 * Author: dtek0068
 *
 * Created on 13 November 2020, 16:27
 */



#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    //aluistetaan k‰ytett‰v‰t arvot ja m‰‰ritet‰‰n k‰ytett‰v‰ PF5 outputiksi
    
    int i;
    int a;
    PORTF.DIRSET = PIN5_bm; 
                           
    //oma metodi joka sallii muuttujien k‰ytt‰misen delay funktiossa
    //muokattu sopiivaksi 7 us time slice
    void delay_us( int us ){
   for (int i = 0; i < us; i++)
   {
      _delay_us(1);
   }
}
    /*pyydetty pwm metodi, aiheuttaa halutun pulssin
     * ei kuitenkaan tunnu toimivcan vaan pit‰‰ valoa p‰‰ll‰ jatkuvasti
     * 
     * myˆhempi kommentti: tuntuu vilkuttavan valoa
     * ei oikeen tyydyt‰ mielest‰ni vaadittua.
     * Tuntuu kyll‰ ett‰ olen seurannut teht‰v‰nantoa(joskin nopeus hidastettu)
    */
     int pwm_period(int duty){
        if (duty == 0){
            _delay_ms(2000);
            return 0;
        }
                if(i == 1){
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(500);
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(1500);
                return 0;
                
                }
                if (i == 2){
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(1000);
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(1000);
                return 0;
                }
            
            if (i == 3){
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(1500);
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(500);
                return 0;
            }
            
            if (i == 4){
                PORTF.OUTTGL = PIN5_bm;
                _delay_ms(2000);
                PORTF.OUTTGL = PIN5_bm;
                return 0;
            }
            
        }
        
    
    
    while (1) {
        /*tasaisesti vilkkuva ledi
         * tuntuu vilkkuvan kovin himme‰n‰, 
         * mutta aiheuttaa kuitenkin tasaisen pulssin lediss‰
         * mielest‰ni vastaa teht‰v‰nannon kuvausta 
         * (pulssi aika noin 1 sekunti jne)
         */
        for (i = 255; i>0; i--){
                PORTF.OUTTGL = PIN5_bm;
                delay_us(i * 7);
                PORTF.OUTTGL = PIN5_bm;
            }
               
            for(i=1; i <= 255 ;i++ ){
                PORTF.OUTTGL = PIN5_bm;
                delay_us(i * 7);
                PORTF.OUTTGL = PIN5_bm;

            }
        /*pwm period testi
         * 
         *
         *
        for (a = 0; a <= 4; a++){
            pwm_period(a);
        }
        */
       

    }
}

   

