/**
 * ADC functions
 */
#include "defines.h"

#define mV_per_Digit    4

#define AVERAGE_DEEP  10

static int16_t f_buffer[AVERAGE_DEEP];
static int16_t r_buffer[AVERAGE_DEEP];


void ADC_Init(void)
{

  //Set Ports
  //ANSELAbits.ANSELA = 0;

  ADC_REVERSE_DIR = DIR_INPUT;
  ADC_REVERSE_SEL = 1;

  ADC_FORWARD_DIR = DIR_INPUT;
  ADC_FORWARD_SEL = 1;




  FVRCON = 0b10000011; // ADC 4096 mV Vref
  //while (FVRCONbits.FVRRDY == 0);

  //ADCON1 settings 
  ADCON1bits.ADFM = 1; //right justified format 
  ADCON1bits.ADCS = 2; //Fosc/32

  ADCON1bits.ADNREF = 0; //negative Ref to Vss
  ADCON1bits.ADPREF = 3; //positive Ref to FVR module


  //ADCON0 settings
  ADCON0bits.ADON = 1;
}

int16_t ADC_GetSample(uint8_t ch)
{
  ADCON0bits.CHS = ch;
  DELAY_5_us();
  ADCON0bits.ADGO = 1; /* start A/D */
  while (ADCON0bits.ADGO == 1);
  int16_t hi = ADRESH;
  
  return (((hi << 8) + ADRESL) * mV_per_Digit);
  
}


void ADC_Run(void)
{
//  int16_t value;
//  int32_t average;
  
  int32_t f_sum =0;
  int32_t r_sum =0;
  
  uint8_t i;
  uint8_t s;
  
  //move buffer
  s=0;
  for(i=0; i < (AVERAGE_DEEP -1); i++)
  {
    s++;
    f_sum += f_buffer[i] = f_buffer[s];
    r_sum += r_buffer[i] = r_buffer[s];
  }
  
   f_sum += f_buffer[s] = ADC_GetSample(ADC_FORWARD_CH);
   r_sum += r_buffer[s] = ADC_GetSample(ADC_REVERSE_CH);
  

  //cal average
  global.adc_f_mV = (int16_t)(f_sum / AVERAGE_DEEP);
  global.adc_r_mV = (int16_t)(r_sum / AVERAGE_DEEP);
  
}


