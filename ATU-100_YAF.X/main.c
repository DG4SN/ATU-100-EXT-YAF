/**
 * ATU-100_EXT_YAF 
 * Yet Another Firmware
 * by DG4SN
 */

// PIC16F1938 Configuration Bit Settings

// CONFIG1
#pragma config FOSC = ECH       // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = ON        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include "defines.h"

global_t global;


static volatile uint8_t tick_count =0;

void MAIN_Init(void)
{
 

  
  //Clock
  OSCCON = 0b01111010; // 16 MHz oscillator, Internal

  //All Ports off
  LATA = 0;
  LATB = 0;
  LATC = 0;

  
  
  
   
 //Timer 2
 //set to 1ms = 1000Hz
 //Fosc/4 = 4Mhz, Prescaler 1:16, Comp value 250,  Postscaler 1:1
   
 T2CONbits.T2CKPS = 2; //Prescaler 1:16
 T2CONbits.T2OUTPS = 0; //Postscaler 1:1
 PR2 = 250;            //Periode Register
 
 PIE1bits.TMR2IE = 1;   //Timer 2 interrupt enable
 INTCONbits.PEIE = 1; // Enable Perpherial Interrupt
 
 T2CONbits.TMR2ON = 1;  //Timer on
 INTCONbits.GIE = 1; // Enable Global Interrupt
  
 
 
 
// Set watchdog  
//  CLRWDT();
//  WDTCONbits.WDTPS4 = 0;
//  WDTCONbits.WDTPS3 = 1;
//  WDTCONbits.WDTPS2 = 0; // 1 sec WDT
//  WDTCONbits.WDTPS1 = 1;
//  WDTCONbits.WDTPS0 = 0;
//  CLRWDT();


  ANSELBbits.ANSELB = 0; // all as digital
  
  CM1CON0bits.C1ON = 0; // Disable comparators
  CM2CON0bits.C2ON = 0;


  
  
//  //  If we are using the auto button for an output for uart
//  //  set B1 (auto) to an output
//#ifdef DEBUG_UART 
//  TRISB = 0b00000101;
//#else
//  TRISB = 0b00000111;  
//#endif
//  
//  
//  TRISC = 0b00000000; //
//  //
//  //ADC_Init();

  //Init Relays
  REL_CAP_1_DIR = DIR_OUTPUT;
  REL_CAP_2_DIR = DIR_OUTPUT;
  REL_CAP_3_DIR = DIR_OUTPUT;
  REL_CAP_4_DIR = DIR_OUTPUT;
  REL_CAP_5_DIR = DIR_OUTPUT;
  REL_CAP_6_DIR = DIR_OUTPUT;
  REL_CAP_7_DIR = DIR_OUTPUT;
  REL_CAP_SW_DIR = DIR_OUTPUT;
  
  REL_IND_1_DIR = DIR_OUTPUT;
  REL_IND_2_DIR = DIR_OUTPUT;
  REL_IND_3_DIR = DIR_OUTPUT;
  REL_IND_4_DIR = DIR_OUTPUT;
  REL_IND_5_DIR = DIR_OUTPUT;
  REL_IND_6_DIR = DIR_OUTPUT;
  REL_IND_7_DIR = DIR_OUTPUT;
  
  global.ind_relays =0;
  global.cap_relays =0;
  global.cap_sw =0;


  global.tune_stop_swr = 100; // SWR 1.10
  

  //Global Enable of Weak PullUp
  OPTION_REGbits.nWPUEN = 0;
 

  
//  WPUBbits.WPUB1 = 1; // PORTB1 Pull-up resistor
//  WPUBbits.WPUB2 = 1; // PORTB2 Pull-up resistor
  
  


  
}


void __interrupt() myIsr(void)
{
  //Timer 2 interrupt 1000Hz
  if(PIR1bits.TMR2IF)
  {
    PIR1bits.TMR2IF =0;
    tick_count++;
  }
  
  
  return;  
  
} 


void main(void)
{
  static uint8_t tick_old=0;
  static uint8_t tick_10ms =0;

  
  //uint16_t count=0;
  
  //uint8_t c;
  
  MAIN_Init();
  EEPROM_Init();
  BUTTON_Init();
  ADC_Init();
 

  
  #ifdef DEBUG_UART
  UART_Init();
  UART_WriteChar(' ');
  #endif
  

  
  DELAY_ms(DISPLAY_INIT_DELAY);
  DISP_Init(DISPLAY_I2C_ADDR, DISPLAY_ROTATE);
  DISP_Str(DISP_COL_CENTER,0,str_ATU100EXT,0);
  DISP_Str(DISP_COL_CENTER,1,str_YAF,0);
  DISP_Str(DISP_COL_CENTER,2,str_Version,0);
  
  #ifdef DEBUG_UART
  UART_WriteStrLn("\n\rHello DG4SN");
  #endif

  DELAY_ms(1500);
  

  DISP_Clr();
  MENU_Init();
  
    //Load index 0 = startup value
  {
    tunemem_t tunemem;
    EEPROM_Read((uint8_t) & ee_tunemem , &tunemem, sizeof (tunemem_t));
    UTILI_Set_LC_Relays(tunemem.relays);
  }
  
  
  while(1)
  {
    
    if(tick_old != tick_count)
    {
      tick_old++;
      tick_10ms++;
      //1ms
      ADC_Run();
      
#ifdef DEBUG_UART
      UART_Run();
#endif      
      
      
    }
    
    
    //10x 1ms = 10ms
    if(tick_10ms==10)
    {
      tick_10ms=0;
      //10ms 

      BUTTON_Run();
      MENU_Run();
     
      

      
      CLRWDT();   
    }  
  }
}
