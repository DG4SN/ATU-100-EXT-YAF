/* 
 * File:   tune.h
 * Author: sven
 *
 * Created on 30. Juni 2022, 19:50
 */

#ifndef TUNE_H
#define	TUNE_H

#include "defines.h"


#ifdef	__cplusplus
extern "C"
{
#endif


#define TUNE_MIN_PWR   10   //10 = 1W  

  
 
  
typedef enum
{
  TUNE_IDLE=1,
  TUNE_RUN=2,
  TUNE_BREAK=3,
  TUNE_PWRLOW=4,        
  TUNE_READY=5           
} tunestate_t;  
  




extern tunestate_t TUNE_state;

void TUNE_Init(void);
void TUNE_Run(void);




#ifdef	__cplusplus
}
#endif

#endif	/* TUNE_H */

