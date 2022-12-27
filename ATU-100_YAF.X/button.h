/* 
 * File:   button.h
 * Author: DG4SN
 *
 * Created on 26. März 2022, 19:30
 */

#ifndef BUTTON_H
#define	BUTTON_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define BUTTON_IDLE          0  
#define BUTTON_SHORT_PRESSED 1  
  
#define BUTTON_RELEASED      251
#define BUTTON_RESET         255  
#define BUTTON_LONG_PRESSED  80
#define BUTTON_LONG_REPEAT_DELAY_MAX 40
#define BUTTON_LONG_REPEAT_DELAY_MIN 10
#define BUTTON_LONG_REPEAT_DELAY_STEP 5  
  
#define BUTTON_MAX_COUNT     250  //uint8_t
  
  
  
  

extern uint8_t BUTTON_count; 
  
extern uint8_t BUTTON_Auto_count;
extern uint8_t BUTTON_Bypass_count;

#define BUTTON_AUTO_RE_TRIGGER     20 // x10mS
#define BUTTON_BYPASS_RE_TRIGGER     20 // x10mS


//typedef union
//{
//  uint8_t byte;
//
//  struct
//  {
//    unsigned Trig_Pressed : 1;
//    unsigned Trig_Released : 1;
//    unsigned Trig_LongPressed : 1;
//    unsigned Enable_Released : 1;
//    unsigned State_Pressed : 1;
//    unsigned State_Released : 1;
//    unsigned State_LongPressed : 1;
//    unsigned Reset : 1;
//  } bits;
//} button_state_t;
//#define  BUTTON_STATE_RESET              0x80 //only bit7 set  
//
//
//extern button_state_t BUTTON_state;
//
//#define BUTTON_LONG_PRESS_TRIGGER        40 // x10ms
//#define BUTTON_LONG_PRESS_RE_TRIGGER     80 // x 10ms

void BUTTON_Init(void);
void BUTTON_Run(void);
//#define BUTTON_Reset()  (BUTTON_state.byte = BUTTON_STATE_RESET)

void BUTTON_Reset(void);


#ifdef	__cplusplus
}
#endif

#endif	/* BUTTON_H */

