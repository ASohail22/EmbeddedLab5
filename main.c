#include <msp430.h> 

#define CALADC_15V_30C *((unsigned int *)0x1A1A)

#define CALADC_15V_85C *((unsigned int *)0x1A1C)

volatile long temp, tempRAW;
volatile float IntDegF;
volatile float IntDegC;

int count;

void ConfigureAdc_temp();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    _delay_cycles(5);
    ConfigureAdc_temp();
    while(1){
        ADCCTL0 |= ADCENC | ADCSC;
        while (!(ADCIFG & ADCIFG0));
            temp = ADCMEM0;
            IntDegC = (temp-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
            IntDegF = 9*IntDegC/5+32;
    }
}
void ConfigureAdc_temp(){
    ADCCTL0 |= ADCSHT_8 | ADCON;
    ADCCTL1 |= ADCSHP | ADCCONSEQ_2;
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;
    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;
    ADCIE |= ADCIE0;

    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN | TSENSOREN;
}
