#include "defines.h"

//button_state_t BUTTON_state;

uint8_t BUTTON_count;
uint8_t BUTTON_was_long_pressed;
uint8_t BUTTON_Auto_count;
uint8_t BUTTON_Bypass_count;


void BUTTON_Init(void)
{
  BUTTON_TUNE_DIR = DIR_INPUT;
  BUTTON_TUNE_PULLUP = 1;
  
  BUTTON_AUTO_DIR = DIR_INPUT;
  BUTTON_AUTO_PULLUP = 1; 
 
  BUTTON_BYPASS_DIR = DIR_INPUT;
  BUTTON_BYPASS_PULLUP = 1;
  
  //BUTTON_state.byte = BUTTON_STATE_RESET;
}


void BUTTON_Reset(void)
{
  BUTTON_count=BUTTON_RESET;
  BUTTON_was_long_pressed =0;
}

void BUTTON_Run(void)
{
  //static uint8_t count;
  

  //Decouncing
// static uint8_t last;
//  if (BUTTON_TUNE_R != last)
//  {
//    last = BUTTON_TUNE_R;
//    return;
//  }

//-----------------------------------------------------------------------
//if(BUTTON_count > BUTTON_MAX_COUNT)
//{
//  BUTTON_count=BUTTON_IDLE;
//}  
 
if (BUTTON_TUNE_R == 0) //Button pressed
{  
  if(BUTTON_count < BUTTON_MAX_COUNT)
  {
    BUTTON_count++;
  }
  
  if(BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_was_long_pressed =1;
  }
  
}
else
{
  if((!BUTTON_was_long_pressed) && (BUTTON_count > 0) && (BUTTON_count <= BUTTON_MAX_COUNT))
  {
    BUTTON_count = BUTTON_RELEASED;
  }
  else
  {
    BUTTON_count =BUTTON_IDLE;
  }
  
  BUTTON_was_long_pressed =0;
}  
  
  
//-----------------------------------------------------------------------  

//  if (BUTTON_TUNE_R == 0) //Button pressed
//  {
//    if(BUTTON_state.bits.Reset ==0)
//    {
//    
//      BUTTON_state.bits.State_Released = 0;
//      BUTTON_state.bits.Trig_Released = 0;
//
//
//      if (BUTTON_state.bits.State_Pressed == 0)
//      {
//        BUTTON_state.bits.State_Pressed = 1;
//        BUTTON_state.bits.Trig_Pressed = 1;
//        BUTTON_state.bits.Enable_Released =1;
//      }
//
//      count++;
//      if(count == BUTTON_LONG_PRESS_RE_TRIGGER)
//      {
//        count = BUTTON_LONG_PRESS_TRIGGER;
//      }
//      
//      //Long Pressed  
//      if (count == BUTTON_LONG_PRESS_TRIGGER)
//      {
//        BUTTON_state.bits.State_LongPressed = 1;
//        BUTTON_state.bits.Trig_LongPressed = 1;
//      }
//      
//    }
//  }
//  else
//  {
//    if(BUTTON_state.bits.Enable_Released) //Only when pressed set released
//    {  
//      if (BUTTON_state.bits.State_Released == 0)
//      {
//        BUTTON_state.bits.State_Released = 1;
//        BUTTON_state.bits.Trig_Released = 1;
//      }
//    } 
//    
//    BUTTON_state.bits.State_Pressed = 0;
//    BUTTON_state.bits.Trig_Pressed = 0;
//    BUTTON_state.bits.State_LongPressed = 0;
//    BUTTON_state.bits.Trig_LongPressed = 0;
//    BUTTON_state.bits.Reset =0;
//    count = 0;
//  
//  }
  
  // Button Auto -------------------------------------------------
  if (BUTTON_AUTO_R == 0) //Button pressed
  {
    BUTTON_Auto_count++;
    if(BUTTON_Auto_count >= BUTTON_AUTO_RE_TRIGGER)
    {
      BUTTON_Auto_count=1;
    }
  }
  else
  {
    BUTTON_Auto_count =0;
  }
  
  // Button Bypass -------------------------------------------------
  if (BUTTON_BYPASS_R == 0) //Button pressed
  {
    BUTTON_Bypass_count++;
    if(BUTTON_Bypass_count >= BUTTON_AUTO_RE_TRIGGER)
    {
      BUTTON_Bypass_count=1;
    }
  }
  else
  {
    BUTTON_Bypass_count =0;
  }
  
}

