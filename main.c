#include <msp430.h> 
#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C //6682
// See device datasheet for TLV table memory mapping //6684
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-High Temperature (85 for Industrial, 105 for Extended)
volatile long temp, tempRaw;
volatile float IntDegF;
volatile float IntDegC;
volatile float thermistorVal;
volatile float ADC_Result;
volatile int counter = 0;
//10k resistor value
/**
 * main.c
 */
float readADCSingle();
void configureADC_thermistorVal();
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    _delay_cycles(5);
    configureADC_thermistorVal();
    while (1)
    {
        int channel = ADCMCTL0 & 0b1111; //bit masking
        ADCCTL0 |= ADCENC | ADCSC;
        while (!(ADCIFG & ADCIFG0)); //start sampling and converting
        __no_operation();

        if (channel== 12)
        {
            ADC_Result = ADCMEM0;
            temp = ADC_Result;
            IntDegC = (temp - CALADC_15V_30C) * (85 - 30)
                    / (CALADC_15V_85C - CALADC_15V_30C) + 30;
            // Temperature in Fahrenheit
            // Tf = (9/5)*Tc | 32
            IntDegF = 9 * IntDegC / 5 + 32;

        }
        else if (channel == 1)
        {
            //readADCSingle();
            ADC_Result = ADCMEM0;
            thermistorVal = ADC_Result;
        }
        if (counter == 0)
        {
            counter = 12;
        }
        else
        {
            counter--;
        }

    }
    return 0;
}

void configureADC_thermistorVal()
{
    // Configure ADC Pin
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;

    ADCCTL1 |= ADCSHP | ADCCONSEQ_1;
    ADCCTL0 |= ADCSHT_8 | ADCON;                       // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                        // ADCCLK = MODOSC; sampling timer
    ADCCTL2 &= ~ADCRES;                                // clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2;                            // 12-bit conversion results

    ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                    // A1 ADC input select; Vref=AVCC
    ADCIE |= ADCIE0;
    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN | TSENSOREN;

}

float readADCSingle()
{
    // while (!(ADCIFG & ADCIFG0));   // Wait for sample to be sampled and converted
    ADCCTL0 |= ADCENC | ADCSC;                  // Sampling and conversion start
    //__bis_SR_register(LPM0_bits | GIE);                  // LPM0, ADC_ISR will force exit
    //__no_operation();
    ADC_Result = ADCMEM0;
    return ADC_Result;            // For debug only

}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    case ADCIV_NONE:
        break;
    case ADCIV_ADCOVIFG:
        break;
    case ADCIV_ADCTOVIFG:
        break;
    case ADCIV_ADCHIIFG:
        break;
    case ADCIV_ADCLOIFG:
        break;
    case ADCIV_ADCINIFG:
        break;
    case ADCIV_ADCIFG:
        ADC_Result = ADCMEM0;
        __bic_SR_register_on_exit(LPM0_bits);      // Clear CPUOFF bit from LPM0
        break;
    default:
        break;
    }
}
