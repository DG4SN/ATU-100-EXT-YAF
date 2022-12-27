#include "defines.h"

#ifdef DEBUG_UART
#define TUNE_DEBUG
#endif


#define TUNE_COARSE_FAST


#define RELAY_DELAY    2  //2=20ms 

#define AddTolerance(x) (x + (x/20))  //+5%




#define COARSE_CAP_STEPS   11 
const uint8_t coarse_cap_value[COARSE_CAP_STEPS] ={
  0b00000000,
  0b00000001, //1
  0b00000010, //2
  0b00000100, //4
  0b00001000, //8 
  0b00010000, //16
  0b00100000, //32
  0b01000000, //64
  0b01100000, //96
  0b01110000,  //112
  0b01111111   //127
};


#define COARSE_IND_STEPS   11 
const uint8_t coarse_ind_value[COARSE_IND_STEPS] ={
  0b00000000,
  0b00000001, //1
  0b00000010, //2
  0b00000100, //4
  0b00001000, //8 
  0b00010000, //16
  0b00100000, //32
  0b01000000, //64
  0b01100000, //96
  0b01110000,  //112
  0b01111111   //127
};

enum
{
  fine_low=0, fine_mid, fine_high
} ;


tunestate_t TUNE_state;
static uint8_t TUNE_step;
static uint8_t TUNE_delay;

void TUNE_Init(void)
{
  TUNE_state=TUNE_IDLE;
  TUNE_step=0;
  TUNE_delay=0;
  global.cap_sw=0;
  global.cap_relays=0;
  global.ind_relays=0;
  UTILI_SetRelays();
  TUNE_delay = RELAY_DELAY;
}

void TUNE_Run(void)
{

  static uint8_t ind_step;
  static uint8_t cap_step;
  static int16_t best_swr;

  static uint8_t best_cap_step;
  static uint8_t best_ind_step;
  static uint8_t best_cap_sw;

  static int16_t best_row_swr;
  static int16_t best_last_row_swr;

  static uint8_t fine_cap_diff_low;
  static uint8_t fine_cap_diff_high;
  static uint8_t fine_ind_diff_low;
  static uint8_t fine_ind_diff_high;
  
  static uint8_t fine_cap[3];
  static uint8_t fine_ind[3];

#ifdef TUNE_DEBUG
   char str[7];
#endif
  
  

  //10ms interval

  if ((TUNE_state == TUNE_BREAK) || (TUNE_state == TUNE_READY)) return;

  //TUNE_delay
  if (TUNE_delay > 0)
  {
    TUNE_delay--;
    return;
  }


#ifdef TUNE_DEBUG
  if (UART_Busy()) return;
#endif


  UTILI_CalPWR();
  UTILI_CalSWR();



  //Check Min PWR
  if ((TUNE_step != 0) && (global.PWR < TUNE_MIN_PWR))
  {
    TUNE_state = TUNE_PWRLOW;
    return;
  }


  switch (TUNE_step) {
  case 0:
    if (global.PWR >= TUNE_MIN_PWR)
    {
      TUNE_step=10;
      TUNE_state=TUNE_RUN;

      best_swr = MAX_SWR;
      best_row_swr = MAX_SWR;
      best_last_row_swr=MAX_SWR;
      ind_step=0;
      cap_step=0;
      TUNE_delay = RELAY_DELAY;

#ifdef TUNE_DEBUG
      UART_WriteLn();
      UART_WriteStrLn("Tune Start");
      UART_WriteStrLn("CAP_SW=0");
#endif

    }
    break;



    //coarse tuning  
  case 10:

#ifdef TUNE_DEBUG
    UART_WriteInt(global.SWR);
#endif

    if(global.SWR <= global.tune_stop_swr)
    {
      TUNE_state = TUNE_READY;
      break;
    }
    
    
    if (global.SWR < best_row_swr)
    {
      best_row_swr = global.SWR;
    }

    if (global.SWR < best_swr)
    {
      best_swr = global.SWR;
      best_cap_step = cap_step;
      best_ind_step = ind_step;
      best_cap_sw = global.cap_sw;
    }





    cap_step++;
#ifdef TUNE_COARSE_FAST
    if ((cap_step >= COARSE_CAP_STEPS) || (global.SWR > AddTolerance(best_row_swr) ) )
#else
    if (cap_step >= COARSE_CAP_STEPS)
#endif
    {
      cap_step=0;
      ind_step++;

#ifdef TUNE_DEBUG
      UART_WriteLn();
#endif

#ifdef TUNE_COARSE_FAST

      //Stop tuning when current row not better than last row
      if (AddTolerance(best_last_row_swr) < best_row_swr)
      {
        if (global.cap_sw == 1)
        {
#ifdef TUNE_DEBUG
          UART_WriteLn();
#endif
          TUNE_step=20; //-> go to fine tuning 
          break;

        }
        else
        {
          //Start tuning again with cap_sw=1
#ifdef TUNE_DEBUG
          UART_WriteLn();
          UART_WriteStrLn("CAP_SW=1");
#endif
          global.cap_sw =1;
          cap_step=0;
          ind_step=0;
          best_row_swr = MAX_SWR;
          best_last_row_swr=MAX_SWR;
        }
      } else
      {
        best_last_row_swr = best_row_swr;
      }

      best_row_swr = MAX_SWR;

#endif //TUNE_COARSE_FAST


      if (ind_step >= COARSE_IND_STEPS)
      {
        ind_step=0;
        best_row_swr = MAX_SWR;
        if (global.cap_sw == 1)
        {
          TUNE_step=20; //-> go to fine tuning 
          break;
        } else
        {
#ifdef TUNE_DEBUG
          UART_WriteLn();
          UART_WriteStrLn("CAP_SW=1");
#endif

          global.cap_sw =1;
        }

      }
    }

    global.cap_relays = coarse_cap_value[cap_step];
    global.ind_relays = coarse_ind_value[ind_step];
    UTILI_SetRelays();
    TUNE_delay = RELAY_DELAY;
    break;



  //fine tuning    
  case 20:
    
    global.cap_sw = best_cap_sw; //cap_sw is now final 
    global.cap_relays = coarse_cap_value[best_cap_step];
    global.ind_relays = coarse_ind_value[best_ind_step];

#ifdef TUNE_DEBUG
    UART_WriteLn();
    UART_WriteStrLn("Coarse-Tuning");
    UART_WriteStr("Cap: ");
    UTILI_GetCapValueStr(str, sizeof (str));
    UART_WriteStrLn(str);
    UART_WriteStr("Ind: ");
    UTILI_GetIndValueStr(str, sizeof (str));
    UART_WriteStrLn(str);
    UART_WriteStr("SWR: ");
    UART_WriteInt(best_swr);
    UART_WriteLn();
#endif    
    
    //Set best coarse tune in the middle of the fine tune matrix 
    fine_cap[fine_mid] = coarse_cap_value[best_cap_step];
    fine_ind[fine_mid] = coarse_ind_value[best_ind_step];

    //Calculate differences to the next steps
    if (best_cap_step > 0)
    {
      fine_cap_diff_low = coarse_cap_value[best_cap_step] - coarse_cap_value[best_cap_step - 1];
    } else
    {
      fine_cap_diff_low =0;
    }

    if (best_cap_step < (COARSE_CAP_STEPS - 1))
    {
      fine_cap_diff_high = coarse_cap_value[best_cap_step + 1] - coarse_cap_value[best_cap_step];
    } else
    {
      fine_cap_diff_high =0;
    }

    if (best_ind_step > 0)
    {
      fine_ind_diff_low = coarse_ind_value[best_ind_step] - coarse_ind_value[best_ind_step - 1];
    } else
    {
      fine_ind_diff_low =0;
    }

    if (best_ind_step < (COARSE_IND_STEPS - 1))
    {
      fine_ind_diff_high = coarse_ind_value[best_ind_step + 1] - coarse_ind_value[best_ind_step];
    } else
    {
      fine_ind_diff_high =0;
    }

    //half the differences
    fine_cap_diff_low = fine_cap_diff_low / 2;
    fine_cap_diff_high = fine_cap_diff_high / 2;
    fine_ind_diff_low = fine_ind_diff_low / 2;
    fine_ind_diff_high = fine_ind_diff_high / 2;

    //set fine tune matrix values
    fine_cap[fine_low] = fine_cap[fine_mid] - fine_cap_diff_low;
    fine_cap[fine_high] = fine_cap[fine_mid] + fine_cap_diff_high;
    fine_ind[fine_low] = fine_ind[fine_mid] - fine_ind_diff_low;
    fine_ind[fine_high] = fine_ind[fine_mid] + fine_ind_diff_high;
    

#ifdef TUNE_DEBUG
    UART_WriteLn();
    UART_WriteStrLn("Fine-Tuning");
#endif

    best_swr = MAX_SWR;  //Max Default

    //Start in the middle
    cap_step=fine_mid;
    ind_step=fine_mid;

    global.cap_relays = fine_cap[cap_step];
    global.ind_relays = fine_ind[ind_step];
    UTILI_SetRelays();
    TUNE_delay = RELAY_DELAY;
    TUNE_step=30;

    break;


    //fine tuning 
  case 30:
#ifdef TUNE_DEBUG
    
  
    UART_WriteInt(cap_step);
    UART_WriteStr("_C-Step ");
    
    UTILI_GetCapValueStr(str, sizeof (str));
    UART_WriteStr(str);
    
    UART_WriteInt(ind_step);
    UART_WriteStr("_I-Step ");
    
    UTILI_GetIndValueStr(str, sizeof (str));
    UART_WriteStr(str);

    UART_WriteInt(global.SWR);
    UART_WriteStrLn("_SWR");
 
#endif

    if(global.SWR <= global.tune_stop_swr)
    {
      TUNE_state = TUNE_READY;
      break;
    }
    
    
    if (global.SWR < best_swr)
    {
      best_swr = global.SWR;
      best_cap_step = cap_step;
      best_ind_step = ind_step;
    }

    do
    {

      //cap-steps mid-> low -> high if different
      if (cap_step == fine_mid)
      {
        if (fine_cap[fine_low] != fine_cap[fine_mid])
        {
          cap_step=fine_low;
          break;
        } else
        {
          if (fine_cap[fine_high] != fine_cap[fine_mid])
          {
            cap_step=fine_high;
            break;
          }
        }
      }

      if (cap_step == fine_low)
      {
        if (fine_cap[fine_high] != fine_cap[fine_mid])
        {
          cap_step=fine_high;
          break;
        }
      }
      cap_step=fine_mid; // cap row done


      //ind_steps mid-> low -> high if different
      if (ind_step == fine_mid)
      {
        if (fine_ind[fine_low] != fine_ind[fine_mid])
        {
          ind_step=fine_low;
          break;
        } else
        {
          if (fine_ind[fine_high] != fine_ind[fine_mid])
          {
            cap_step=fine_high;
            break;
          }
        }
      }

      if (ind_step == fine_low)
      {
        if (fine_ind[fine_high] != fine_ind[fine_mid])
        {
          ind_step=fine_high;
          break;
        }
      }

      ind_step=fine_mid; //all done
    } while ((cap_step != fine_mid) || (ind_step != fine_mid));


    //Check if loop ready
    if ((cap_step == fine_mid) && (ind_step == fine_mid))
    {

      global.cap_relays = fine_cap[best_cap_step];
      global.ind_relays = fine_ind[best_ind_step];
      UTILI_SetRelays();
      TUNE_state = TUNE_READY;
      break; 
    }


    global.cap_relays = fine_cap[cap_step];
    global.ind_relays = fine_ind[ind_step];
    UTILI_SetRelays();
    TUNE_delay = RELAY_DELAY;
    break;


    
 


  default:
    TUNE_state = TUNE_BREAK;
    break;
  }

}




//-----------------------------------------------------
//          //Calculate next differences
//    //Best value to the middle
//    fine_cap[fine_mid] = fine_cap[best_cap_step];
//    fine_ind[fine_mid] = fine_ind[best_ind_step];
//
//    //Cal new cap differences
//    if (best_cap_step == fine_mid)
//    {
//      fine_cap_diff_low = fine_cap_diff_low / 2;
//      fine_cap_diff_high = fine_cap_diff_high / 2;
//    }
//    if (best_cap_step == fine_low)
//    {
//      fine_cap_diff_low = fine_cap_diff_low / 2;
//      fine_cap_diff_high = fine_cap_diff_low / 2;
//    }
//    if (best_cap_step == fine_high)
//    {
//      fine_cap_diff_low = fine_cap_diff_high / 2;
//      fine_cap_diff_high = fine_cap_diff_high / 2;
//    }
//
//    //Cal new ind differences
//    if (best_ind_step == fine_mid)
//    {
//      fine_ind_diff_low = fine_ind_diff_low / 2;
//      fine_ind_diff_high = fine_ind_diff_high / 2;
//    }
//    if (best_ind_step == fine_low)
//    {
//      fine_ind_diff_low = fine_ind_diff_low / 2;
//      fine_ind_diff_high = fine_ind_diff_low / 2;
//    }
//    if (best_ind_step == fine_high)
//    {
//      fine_ind_diff_low = fine_ind_diff_high / 2;
//      fine_ind_diff_high = fine_ind_diff_high / 2;
//    }
//      
//      
//      
//      
//      //It ends when there are no differens
//      if ((fine_cap_diff_low <= 1) &&
//          (fine_cap_diff_high <= 1) &&
//          (fine_ind_diff_high <= 1) &&
//          (fine_ind_diff_high <= 1))
//      {
//        TUNE_state = TUNE_READY;
//      } 
//      else
//      {
//        TUNE_step=35; //Call next round
//      }


