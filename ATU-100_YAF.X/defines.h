/* 
 * File:   defines.h
 * Author: DG4SN
 *
 * Created on 22. März 2022, 15:59
 */

#ifndef DEFINES_H
#define	DEFINES_H

#ifdef	__cplusplus
extern "C"
{
#endif

 
  
#include <xc.h>

//----------------------------------------------------------------------    
  
//  the internal oscillator is 16 Mhz
//  the xc8 compiler needs the crystal freq defined here


// Include debug with software uart  
// #define DEBUG_UART 

#define TRUE          1
#define FALSE         0  
  
#define DISPLAY_I2C_ADDR    0x78  
#define DISPLAY_INIT_DELAY  500  //ms
#define DISPLAY_ROTATE      FALSE  
  
#define _XTAL_FREQ 16000000

//  the CPU clock is 4 Mhz, so to convert to microseconds, there are
//  4 clocks per usec

#define DELAY_5_US_CLOCKS 20
#define DELAY_5_us() _delay((unsigned long)(DELAY_5_US_CLOCKS))
//#define DELAY_100_US_CLOCKS 400
//#define DELAY_100_us() _delay((unsigned long)(DELAY_100_US_CLOCKS))  
  

#define DELAY_1_MS_CLOCKS 4000  
#define DELAY_ms(x) UTILI_Delay_ms(x)

#define MAX_SWR                 999  //9.99 
#define SWR_FORWORD_THRESHOLD   100 //mV  
  


#define CAL_GAIN_MULTIPLIER      4096  
#define DIR_OUTPUT 0
#define DIR_INPUT 1  
  
  

  
  
#define UART_PIN_W LATAbits.LATA6
#define UART_PIN_DIR TRISAbits.TRISA6
  
#define BUTTON_TUNE_R       PORTBbits.RB0   
#define BUTTON_TUNE_DIR     TRISBbits.TRISB0 
#define BUTTON_TUNE_PULLUP  WPUBbits.WPUB0   

#define BUTTON_AUTO_R       PORTBbits.RB1   
#define BUTTON_AUTO_DIR     TRISBbits.TRISB1 
#define BUTTON_AUTO_PULLUP  WPUBbits.WPUB1  

#define BUTTON_BYPASS_R       PORTBbits.RB2   
#define BUTTON_BYPASS_DIR     TRISBbits.TRISB2 
#define BUTTON_BYPASS_PULLUP  WPUBbits.WPUB2    

#define n_Tx LATBbits.LATB2
#define p_Tx LATBbits.LATB0
    
#define LED_GREEN LATBbits.LATB2
#define LED_RED   LATBbits.LATB0
#define LED_GREEN_DIR TRISAbits.LATB2
#define LED_RED_DIR   TRISAbits.LATB0   

#define I2C_SCL_W LATBbits.LATB6
#define I2C_SDA_W LATBbits.LATB7 
#define I2C_SCL_DIR TRISBbits.TRISB6
#define I2C_SDA_DIR TRISBbits.TRISB7  
    
#define ADC_REVERSE_DIR TRISAbits.TRISA0  
#define ADC_REVERSE_SEL ANSELAbits.ANSA0    
#define ADC_REVERSE_CH  0
  
#define ADC_FORWARD_DIR TRISAbits.TRISA1  
#define ADC_FORWARD_SEL ANSELAbits.ANSA1   
#define ADC_FORWARD_CH  1  

#define REL_CAP_1           LATCbits.LATC7
#define REL_CAP_1_DIR       TRISCbits.TRISC7   
  
#define REL_CAP_2           LATCbits.LATC3
#define REL_CAP_2_DIR       TRISCbits.TRISC3
   
#define REL_CAP_3           LATCbits.LATC6
#define REL_CAP_3_DIR       TRISCbits.TRISC6  
  
#define REL_CAP_4           LATCbits.LATC2
#define REL_CAP_4_DIR       TRISCbits.TRISC2  
  
#define REL_CAP_5           LATCbits.LATC5
#define REL_CAP_5_DIR       TRISCbits.TRISC5  
  
#define REL_CAP_6           LATCbits.LATC1
#define REL_CAP_6_DIR       TRISCbits.TRISC1  
  
#define REL_CAP_7           LATCbits.LATC4
#define REL_CAP_7_DIR       TRISCbits.TRISC4  
  
#define REL_CAP_SW          LATCbits.LATC0
#define REL_CAP_SW_DIR      TRISCbits.TRISC0
  
//
#define REL_IND_1           LATBbits.LATB3
#define REL_IND_1_DIR       TRISBbits.TRISB3  
  
#define REL_IND_2           LATAbits.LATA2
#define REL_IND_2_DIR       TRISAbits.TRISA2    
  
#define REL_IND_3           LATBbits.LATB4
#define REL_IND_3_DIR       TRISBbits.TRISB4  

#define REL_IND_4           LATAbits.LATA3
#define REL_IND_4_DIR       TRISAbits.TRISA3  
  
#define REL_IND_5           LATBbits.LATB5
#define REL_IND_5_DIR       TRISBbits.TRISB5  
  
#define REL_IND_6           LATAbits.LATA5
#define REL_IND_6_DIR       TRISAbits.TRISA5    
  
#define REL_IND_7           LATAbits.LATA4 
#define REL_IND_7_DIR       TRISAbits.TRISA4      
 
//----------------------------------------------------------------------    

#define TUNEMEM_ITEMS       10
#define TUNEMEM_STRLEN      8  
  
#define SLEEP_WAKEUP_PWR      5 // = 0.5W
  
typedef struct
{
  uint16_t relays;
  char str_name[TUNEMEM_STRLEN];
}tunemem_t;  
  
//----------------------------------------------------------------------  
  
  typedef union
  {
      uint8_t byte;
      struct
      {
          unsigned B0 : 1;
          unsigned B1 : 1;
          unsigned B2 : 1;
          unsigned B3 : 1;
          unsigned B4 : 1;
          unsigned B5 : 1;
          unsigned B6 : 1;
          unsigned B7 : 1;
      } bits;
  } bitfield_t;  
  
  


  

const    int16_t CAP_VALUES[7] = {10, 22, 47, 100, 220, 470, 1000}; //Cap in pF
const    int16_t IND_VALUES[7] = {50, 100, 220, 450, 1000, 2200, 4400}; //Ind in nH
  
  
//Note: eeprom values in global due to eeprom address felxibility
  typedef struct
  {
    int16_t PWR; //deziWatt -> 10 = 1W
    int16_t SWR; //100 = 1.00

    int16_t tune_stop_swr;
    int16_t tune_auto_swr;
    uint8_t tune_auto_enable;
    
    uint8_t sleep_enable;
    int16_t sleep_delay_sec;

    uint8_t bypass_enable;
    uint16_t bypass_save_relais;
    
    int16_t cal_point[2];
    int16_t cal_offset;
    int16_t cal_gain;
    int16_t adc_r_mV;
    int16_t adc_f_mV;
    uint8_t cap_sw;
    uint8_t cap_relays;
    uint8_t ind_relays;

  }global_t;
  

  
  
  
//variables
  
  
global_t extern global;  

#include "uart.h"
#include "eeprom.h"
#include "utili.h"
#include "disp.h"
#include "button.h"
#include "menu.h"
#include "adc.h"  
#include "tune.h"  
  

const char str_ATU100EXT[] = "ATU-100";
const char str_YAF[]       = "EXT-YAF";
const char str_Version[]   =  "V0.68";
const char str_YetAnother[] = "YetAnother";
const char str_Firmware[] = "Firmware";
const char str_Hardware[] = "Hardware";
const char str_designed[] = "designed";
const char str_by[] ="by";
const char str_N7DCC[] = "N7DDC";
const char str_DG4SN[] = "DG4SN";

const char str_SWR[]   =  "SWR";
const char str_PWR[]   =  "PWR";

const char str_L_[]   =  "L=";
const char str_C_[]   =  "C=";

const char str_C_L[] = ">C-L";
const char str_L_C[] = ">L-C";


const char str__Tune_[] = " Tune ";

const char str_Esc[] = "Esc";
//const char str_Esc_[] = "Esc ";
const char str_Off[] = "Off";
const char str_On[] = " On";
const char str_Ok[] = "Ok";

const char str_AUTO[]  = "AUTO";
const char str_Power[] = "Power";
const char str_Point[] = "Point";

const char str_Bypass[]    = " Bypass ";
    
//const char str_Sec[] = "Sec";
const char str_Delay[] = "Delay";
const char str_Sleep[] = "Sleep";
const char str_sleeping[] ="sleeping";

const char str_Auto[]   =  "Auto";
const char str_Start[]   =  "Start";
const char str_Stop[]   =  "Stop";
const char str_Save[]   = "Save";


const char str_MENU_Reset[]         =  "Reset     ";
const char str_MENU_Bypass[]        =  "Bypass    ";
const char str_MENU_Save[]          =  "Save Tune ";
const char str_MENU_Load[]          =  "Load Tune ";
const char str_MENU_TunePar[]       =  "Tuning    ";
const char str_MENU_Sleep[]         =  "Sleep     ";
const char str_MENU_RelTest[]       =  "Test      ";
const char str_MENU_Cal[]           =  "Calibrate ";
const char str_MENU_Setup[]         =  "Setup     ";
const char str_MENU_About[]         =  "About     ";
  

const char str_SpaceLine[] = "          ";
const char str_WaitForHF[] = "wait f. HF";
const char str_Running[]   = " Running  "; 
const char str_Break[]     = "  Break   ";
const char str_PWR_low[]   = " PWR low  ";
const char str_Ready[]     = "  Ready   ";

 const char* str_TUNE_State[6] ={
 str_SpaceLine,
 str_WaitForHF,
 str_Running,
 str_Break,
 str_PWR_low,
 str_Ready};


 const char NameEditChar[] =" 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ?!/+-#%$_";
 
 

 
 
 
#ifdef	__cplusplus
}
#endif

#endif	/* DEFINES_H */

