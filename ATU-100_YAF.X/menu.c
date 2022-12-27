#include "defines.h"

#define MENU_MEMORY_TIMEOUT   500 // x 10ms

#define MENU_SUB_TIMEOUT   500 // x 10ms

#define UPDATE_PERIODE       10 // x 10ms
#define TUNE_UPDATE_PERIODE       50 // x 10ms

#define CAL_STEP0_MAX   100  //  x 0.1W = 10W
#define CAL_STEP1_MAX   1000 //  x 0.1W = 100W

#define ADC_CAL_START   100 //mV
#define ADC_CAL_STOP      5 //mV

#define TUNE_AUTO_MIN_SWR   120 
#define TUNE_AUTO_MAX_SWR   200

#define TUNE_STOP_MIN_SWR   100 
#define TUNE_STOP_MAX_SWR   150

#define SLEEP_DELAY_MAX       300
#define SLEEP_DELAY_STEP_SLOW   5
#define SLEEP_DELAY_STEP_FAST  15
#define SLEEP_DELAY_FAST_HIGHER  100

typedef struct
{
  const char* title;
  void (*Init)(void) ;
  void (*Run)(void) ;
} menu_t;


//variables
const menu_t* current_menu;


static union
{

  struct
  {
    uint8_t cursor;
    uint16_t relais_backup;
    uint8_t update_cnt;
  } reltest;

  struct
  {
    uint8_t cursor; 
    int16_t stop_swr;
    int16_t auto_swr;
    uint8_t auto_enable;
  } tparam;
  
  struct
  {
    uint8_t cursor;
    uint8_t enable;
    int16_t delay_sec;
  } sleep; 
  
  struct
  {
    uint8_t update_cnt;
    int16_t old_pwr;
    int16_t sleep_timer;
    uint8_t counter_1sec;
    uint16_t relais_backup;
  } main;

  struct
  {
    uint8_t items;
    uint8_t cursor;
    uint8_t top;
    uint16_t timeout;
    const menu_t **ptrMenu;
  } sub;
  
  
  struct
  {
    uint8_t update_cnt;
    uint8_t blink;
    uint16_t relays_backup;
  } tune;

  struct
  {
    uint8_t cursor;
    uint8_t step;
    int16_t cal_point[2];
    int16_t adc_value[2];
    int16_t adc_value_old;

  } cal;
  struct
  {
    uint8_t step;
  } about;
  
  struct
  {
    uint16_t timeout;
    uint8_t cursor;
    uint8_t save_mode;
    uint8_t top;
  } memory;
  
  struct
  {
    uint8_t mem_index;
    uint8_t cursor;
    uint8_t button_repeat_delay;
    tunemem_t tunemem;
  }nameedit;        
          
  struct
  {
    uint8_t mem_index;
    uint8_t cursor;
    tunemem_t tunemem;
    uint8_t str_terminate; //must stay direkt behind memory
  }startupsave;    
  
  
} MENU_var;


static uint8_t MENU_cursor;





//Prototypes

static void MENU_Main_Init(void);
static void MENU_Main_Run(void);

static void MENU_SubSetup_Init(void);
static void MENU_SubTop_Init(void);

static void MENU_Sub_Run(void);
static void MENU_Sub_Update(void);


static void MENU_Reset_Init(void);
static void MENU_Bypass_Init(void);

static void MENU_TParam_Init(void);
static void MENU_TParam_Run(void);

static void MENU_TParam_Init(void);
static void MENU_TParam_Run(void);

static void MENU_Sleep_Init(void);
static void MENU_Sleep_Run(void);

static void MENU_CalPWR_Init(void);
static void MENU_CalPWR_Run(void);

static void MENU_RelTest_Init(void);
static void MENU_RelTest_Run(void);

static void MENU_Tune_Init(void);
static void MENU_Tune_Run(void);

static void MENU_MemorySave_Init(void);
static void MENU_MemoryLoad_Init(void);
static void MENU_Memory_Run(void);


static void MENU_About_Init(void);
static void MENU_About_Run(void);

static void MENU_NameEdit_Init(void);
static void MENU_NameEdit_Run(void);

static void MENU_StartupSave_Init(void);
static void MENU_StartupSave_Run(void);

const menu_t menuMain ={NULL, MENU_Main_Init, MENU_Main_Run};
const menu_t menuSubTop ={NULL, MENU_SubTop_Init, MENU_Sub_Run};
const menu_t menuSubSetup ={str_MENU_Setup, MENU_SubSetup_Init, MENU_Sub_Run};

const menu_t menuBypass   ={str_MENU_Bypass, MENU_Bypass_Init, NULL};
const menu_t menuReset    ={str_MENU_Reset, MENU_Reset_Init, NULL};

const menu_t menuSave    ={str_MENU_Save, MENU_MemorySave_Init, MENU_Memory_Run};
const menu_t menuLoad    ={str_MENU_Load, MENU_MemoryLoad_Init, MENU_Memory_Run};

const menu_t menuAbout    ={str_MENU_About, MENU_About_Init, MENU_About_Run};



const menu_t menuTParam   ={str_MENU_TunePar, MENU_TParam_Init, MENU_TParam_Run};

const menu_t menuSleep    ={str_MENU_Sleep, MENU_Sleep_Init, MENU_Sleep_Run};
const menu_t menuRelTest  ={str_MENU_RelTest, MENU_RelTest_Init, MENU_RelTest_Run};
const menu_t menuCal      ={str_MENU_Cal, MENU_CalPWR_Init, MENU_CalPWR_Run};


#define MENU_SUBTOP_ITEMS    6
const menu_t* ptrSubTopMenu[MENU_SUBTOP_ITEMS] = { &menuBypass, &menuLoad, &menuSave, &menuReset, &menuSubSetup, &menuAbout};

#define MENU_SUBSETUP_ITEMS    4
const menu_t* ptrSubSetupMenu[MENU_SUBSETUP_ITEMS] = { &menuTParam, &menuSleep, &menuCal, &menuRelTest };

const menu_t menuTune     ={NULL, MENU_Tune_Init, MENU_Tune_Run};
const menu_t menuNameEdit ={NULL, MENU_NameEdit_Init, MENU_NameEdit_Run};
const menu_t menuStartupSave ={NULL, MENU_StartupSave_Init, MENU_StartupSave_Run};


//-- MENU_MAIN -------------------------------------------------------------------------------------------------

static void MENU_Main_Update(void)
{
  uint8_t c_row = 2;
  uint8_t l_row = 3;
  
  const char *p_str = NULL;
  char str[7];
 
  DISP_PWR(4, 0, global.PWR, 0);
  DISP_SWR(4, 1, global.SWR, 0);
  
  if (global.cap_sw)
  {
    c_row = 3;
    l_row = 2;
  }


  if (global.bypass_enable == TRUE)
  {
    DISP_Str(0, 3, str_Bypass, 1);
  } 
  else
  {
    DISP_Str(0, c_row,str_C_, 0);
    UTILI_GetCapValueStr(str, sizeof (str));
    DISP_Str(2, c_row, str, 0);

    DISP_Str(0, l_row,str_L_, 0);
    UTILI_GetIndValueStr(str, sizeof (str));
    DISP_Str(2, l_row, str, 0);
  }
  
  //Display AUTO 
  if((global.tune_auto_enable == TRUE) && (global.bypass_enable != TRUE))
  {
    p_str = str_AUTO;
    for(uint8_t i=0; i<4; i++)
    {  
     DISP_Char(9, i, p_str[i], 0);
    }
  }
  
  
  
 

//  UTILI_Int2Str(global.adc_f_mV, str, sizeof (str));
//  DISP_Str(5, 2, str, 0);
//
//  UTILI_Int2Str(global.adc_r_mV, str, sizeof (str));
//  DISP_Str(5, 3, str, 0);


}

static void MENU_Main_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();
  DISP_Str(0, 0,str_PWR,0);
  DISP_Str(0, 1,str_SWR,0);
  MENU_var.main.update_cnt = 0;
  MENU_var.main.old_pwr = -1; //Dummy value
  MENU_var.main.counter_1sec =0;
  MENU_var.main.sleep_timer =0;
  MENU_var.main.relais_backup = UTILI_Get_LC_Relays();
}

static void MENU_Main_Weakup(void)
{
   UTILI_Set_LC_Relays(MENU_var.main.relais_backup);
   MENU_Main_Init(); //Restore display
}

static void MENU_Main_Run(void)
{
   UTILI_CalPWR();
   UTILI_CalSWR();
  
  //-----------------------------------------
  //global.sleep_delay_sec = 100;
  //-----------------------------------------
  
  if ((global.PWR >= SLEEP_WAKEUP_PWR ) && (global.sleep_enable == TRUE))
  {
    if (MENU_var.main.sleep_timer == global.sleep_delay_sec)
    {
      //sleeping weakup by power
      MENU_Main_Weakup();
    }

    MENU_var.main.sleep_timer=0; //Reset sleeptimer if PWR >0
  }
  
  if ((global.sleep_enable == TRUE) && (global.bypass_enable != TRUE))
  {
    if (MENU_var.main.sleep_timer < global.sleep_delay_sec)
    {
      MENU_var.main.counter_1sec++;
      if (MENU_var.main.counter_1sec == 100)
      {
        MENU_var.main.counter_1sec=0;
        MENU_var.main.sleep_timer++;
        if (MENU_var.main.sleep_timer == global.sleep_delay_sec)
        {
          //Sleeping
          DISP_Clr();
          DISP_Str(DISP_COL_CENTER, 1, str_sleeping, 0);
          UTILI_Set_LC_Relays(0); //Reset all relays
        }
      }
    }
  }
  

  if ((MENU_var.main.sleep_timer < global.sleep_delay_sec) || (global.sleep_enable != TRUE)) 
  {
    MENU_var.main.update_cnt++;
    if (MENU_var.main.update_cnt == UPDATE_PERIODE)
    {
      MENU_var.main.update_cnt = 0;
      if ((global.PWR > 0) || (MENU_var.main.old_pwr != 0)) //avoid display refresh when receiving
      {
        MENU_var.main.old_pwr =  global.PWR;
        MENU_Main_Update();
      }
    }

    //Auto Tune
    if((global.tune_auto_enable == TRUE) && (global.bypass_enable != TRUE))
    {
      if((global.PWR > 0) && (global.SWR >= global.tune_auto_swr))
      {
        current_menu = &menuTune;
        current_menu->Init();
      }
    }
    
    
    //short press -> 
    if (BUTTON_count == BUTTON_RELEASED)
    {
      if(global.bypass_enable == TRUE) //-> bypass reset
      {
        global.bypass_enable = FALSE;
        UTILI_Set_LC_Relays(global.bypass_save_relais);
      }
      else // -> Tuning
      {  
        current_menu = &menuTune; 
      }
      current_menu->Init();
    }

    //long pressed -> 
    if (BUTTON_count == BUTTON_LONG_PRESSED)
    {
      if(global.bypass_enable == TRUE) //-> bypass reset
      {
        global.bypass_enable = FALSE;
        UTILI_Set_LC_Relays(global.bypass_save_relais);
      }
      else //-> sub menu
      {
        current_menu = &menuSubTop;
      }  
      current_menu->Init();
    }
  }
  else
  {
     //sleeping weakup by button
    if (BUTTON_count == BUTTON_SHORT_PRESSED)
    {
      MENU_Main_Weakup();
    }
  }



}

//-- Menu_Reset  --------------------------------------------------------------------------------------------------
static void MENU_Reset_Init(void)
{
  UTILI_Set_LC_Relays(0);
  global.bypass_enable = FALSE;  
  MENU_Init();
}

//-- Menu Bypass --------------------------------------------------------------------------------------------------
static void MENU_Bypass_Init(void)
{
  global.bypass_enable = TRUE;
  global.bypass_save_relais = UTILI_Get_LC_Relays(); //Save current relais
  UTILI_Set_LC_Relays(0); //reset relais
  MENU_Init();
}

//-- Menu_Tune  --------------------------------------------------------------------------------------------------

static void MENU_Tune_Update(void)
{
  uint8_t state = TUNE_state;
  if (state == TUNE_RUN)
  {
    if (MENU_var.tune.blink)
    {
      MENU_var.tune.blink = 0;
      state=0;
    } else
    {
      MENU_var.tune.blink = 1;
    }
  }
  DISP_Str(DISP_COL_CENTER, 2, str_TUNE_State[state], 0);
}

static void MENU_Tune_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();
  DISP_Str(DISP_COL_CENTER, 0, str__Tune_, 1);
  MENU_var.tune.blink=0;
  MENU_var.tune.relays_backup = UTILI_Get_LC_Relays();
  TUNE_Init();
}

static void MENU_Tune_Run(void)
{

  //Tune 
  TUNE_Run();

  //Display update
  MENU_var.tune.update_cnt++;
  if (MENU_var.tune.update_cnt == TUNE_UPDATE_PERIODE)
  {
    MENU_var.tune.update_cnt = 0;
    MENU_Tune_Update();
  }

  if (BUTTON_count == BUTTON_SHORT_PRESSED)
  {
    UTILI_Set_LC_Relays(MENU_var.tune.relays_backup);
    MENU_Init();
    return;
  }

  if(TUNE_state == TUNE_READY)
  {
    MENU_Init();
    return;
  }
  
}

//-- Menu_TParam  --------------------------------------------------------------------------------------------------
static void MENU_TParam_Update(void)
{
  uint8_t cursor = MENU_var.tparam.cursor;
  if(MENU_var.tparam.auto_enable == TRUE)
  {
    DISP_Str(7, 0, str_On, (cursor == 0)); 
  }
  else
  {
    DISP_Str(7, 0, str_Off, (cursor == 0)); 
  }
  
  DISP_SWR(6, 1, MENU_var.tparam.auto_swr, (cursor == 1));
  DISP_SWR(6, 2, MENU_var.tparam.stop_swr, (cursor == 2));
  DISP_Str(0,3,str_Save, (cursor == 3));
  DISP_Str(7,3,str_Esc, (cursor == 4));

}

static void MENU_TParam_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();
  DISP_Str(0, 0,str_Auto, 0);
  DISP_Str(0, 1,str_Start,0);
  DISP_Str(0, 2,str_Stop,0);
  MENU_var.tparam.cursor=0;
  MENU_var.tparam.auto_enable = global.tune_auto_enable;
  MENU_var.tparam.auto_swr = global.tune_auto_swr;
  MENU_var.tparam.stop_swr = global.tune_stop_swr;
  MENU_TParam_Update();
}

static void MENU_TParam_Run(void)
{
  
  if (BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.tparam.cursor++;
    if (MENU_var.tparam.cursor > 4)
    {
      MENU_var.tparam.cursor = 0;
    }

    MENU_TParam_Update();
  }
  
  if (BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_count -= BUTTON_LONG_REPEAT_DELAY_MAX;
    
    //cursor at "Auto"
    if(MENU_var.tparam.cursor == 0)
    {
      if(MENU_var.tparam.auto_enable == TRUE)
      {
        MENU_var.tparam.auto_enable = FALSE;
      }
      else
      {
        MENU_var.tparam.auto_enable = TRUE;
      }
      MENU_TParam_Update();
    }    
    
    
    //cursor at "Start"
    if(MENU_var.tparam.cursor == 1)
    {
      MENU_var.tparam.auto_swr += 5; // step SWR 0.05
      if(MENU_var.tparam.auto_swr > TUNE_AUTO_MAX_SWR)
      {
        MENU_var.tparam.auto_swr = TUNE_AUTO_MIN_SWR;
      }
      MENU_TParam_Update();
    }
      
    //cursor at "Stop"
    if(MENU_var.tparam.cursor == 2)
    {
      MENU_var.tparam.stop_swr += 5; // step SWR 0.05
      if(MENU_var.tparam.stop_swr > TUNE_STOP_MAX_SWR)
      {
        MENU_var.tparam.stop_swr = TUNE_STOP_MIN_SWR;
      }
      MENU_TParam_Update();
    }
    
    if (MENU_var.tparam.cursor == 3) //cursor at "Save"
    {
      global.tune_auto_enable = MENU_var.tparam.auto_enable;
      global.tune_auto_swr = MENU_var.tparam.auto_swr;
      global.tune_stop_swr = MENU_var.tparam.stop_swr;
      
      EEPROM_Write((uint8_t)&ee_tune_stop_swr, &global.tune_stop_swr, sizeof(ee_tune_stop_swr));
      EEPROM_Write((uint8_t)&ee_tune_auto_swr, &global.tune_auto_swr, sizeof(ee_tune_auto_swr));
      EEPROM_Write((uint8_t)&ee_tune_auto_enable, &global.tune_auto_enable, sizeof(ee_tune_auto_enable));
      MENU_Init();
      return;
    } 

    if (MENU_var.tparam.cursor == 4) //cursor at "ESC"
    {
      MENU_Init();
      return;
    } 
    
  }
}


//-- Menu_Sleep  --------------------------------------------------------------------------------------------------
static void MENU_Sleep_Update(void)
{
  uint8_t cursor = MENU_var.sleep.cursor;
  if(MENU_var.sleep.enable == TRUE)
  {
    DISP_Str(7, 0, str_On, (cursor == 0)); 
  }
  else
  {
    DISP_Str(7, 0, str_Off, (cursor == 0)); 
  }
  
  char str[4];
  UTILI_Int2Str(MENU_var.sleep.delay_sec, str, sizeof(str));
  DISP_Str(7,1,str,(cursor == 1));
  DISP_Str(0,3,str_Save,(cursor == 2));
  DISP_Str(7,3,str_Esc, (cursor == 3));
  
}

static void MENU_Sleep_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();
  DISP_Str(0, 0, str_Sleep, 0);
  DISP_Str(0, 1, str_Delay,0);
  MENU_var.sleep.cursor=0;
  MENU_var.sleep.enable = global.sleep_enable;
  MENU_var.sleep.delay_sec = global.sleep_delay_sec;
  MENU_Sleep_Update();
}

static void MENU_Sleep_Run(void)
{
  
  if (BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.sleep.cursor++;
    if (MENU_var.sleep.cursor > 3)
    {
      MENU_var.sleep.cursor = 0;
    }
    MENU_Sleep_Update();
  }
  
  if (BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_count -= BUTTON_LONG_REPEAT_DELAY_MAX;
    
    //cursor at "Sleep"
    if(MENU_var.sleep.cursor == 0)
    {
      if(MENU_var.sleep.enable == TRUE)
      {
        MENU_var.sleep.enable = FALSE;
      }
      else
      {
        MENU_var.sleep.enable = TRUE;
      }
      MENU_Sleep_Update();
    }    
    
    
    //cursor at "Delay"
    if(MENU_var.sleep.cursor == 1)
    {
      
      if(MENU_var.sleep.delay_sec >= SLEEP_DELAY_FAST_HIGHER)
      {
        MENU_var.sleep.delay_sec += SLEEP_DELAY_STEP_FAST;
      }
      else
      {
        MENU_var.sleep.delay_sec += SLEEP_DELAY_STEP_SLOW;
      }
          
      if(MENU_var.sleep.delay_sec > SLEEP_DELAY_MAX)
      {
        MENU_var.sleep.delay_sec = SLEEP_DELAY_STEP_SLOW;
      }
      
      MENU_Sleep_Update();
    }

    
    if (MENU_var.tparam.cursor == 2) //cursor at "Save"
    {
      global.sleep_delay_sec = MENU_var.sleep.delay_sec;
      global.sleep_enable = MENU_var.sleep.enable;
      EEPROM_Write((uint8_t)&ee_sleep_delay_sec, &global.sleep_delay_sec, sizeof(ee_sleep_delay_sec));
      EEPROM_Write((uint8_t)&ee_sleep_enable, &global.sleep_enable, sizeof(ee_sleep_enable));
      MENU_Init();
      return;
    }
    
    if (MENU_var.tparam.cursor == 3) //cursor at "Esc"
    {
      MENU_Init();
      return;
    }
    
    
  }
}



//-- Menu_RelTest  --------------------------------------------------------------------------------------------------

static void MENU_RelTest_Update(void)
{
  uint8_t cursor = MENU_var.reltest.cursor;
  uint8_t c_row = 1;
  uint8_t l_row = 2;
  char str[7];



  //switch off : Input -> C -> L -> Output
  //switch on  : Input -> L -> C -> Output
  if (global.cap_sw)
  {
    c_row = 2;
    l_row = 1;
  }


  DISP_Str(0, c_row, str_C_, 0);
  UTILI_GetCapValueStr(str, sizeof (str));
  DISP_Str(4, c_row, str, (cursor == (c_row - 1)));

  DISP_Str(0, l_row, str_L_, 0);
  UTILI_GetIndValueStr(str, sizeof (str));
  DISP_Str(4, l_row, str, (cursor == (l_row - 1)));

  if (!global.cap_sw)
  {
    DISP_Str(0, 3, str_C_L, (cursor == 2));
  } else
  {
    DISP_Str(0, 3, str_L_C, (cursor == 2));
  }

  DISP_Str(7, 3, str_Esc, (cursor == 3));
}

static void MENU_RelTest_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();
  MENU_var.reltest.cursor = 0;
  MENU_var.reltest.relais_backup = UTILI_Get_LC_Relays();
  MENU_var.reltest.update_cnt = 0;
  global.cap_sw = 0;
  global.cap_relays = 0;
  global.ind_relays = 0;
  UTILI_SetRelays();
  MENU_RelTest_Update();
}

static void MENU_RelTest_Run(void)
{

  MENU_var.reltest.update_cnt++;
  if (MENU_var.reltest.update_cnt == UPDATE_PERIODE)
  {
    MENU_var.reltest.update_cnt = 0;
    UTILI_CalPWR();
    UTILI_CalSWR();
    DISP_SWR(0, 0, global.SWR, 0);
    DISP_PWR(6, 0, global.PWR, 0);
  }

  if (BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.reltest.cursor++;
    if (MENU_var.reltest.cursor > 3)
    {
      MENU_var.reltest.cursor = 0;
    }

    MENU_RelTest_Update();
  }


  if (BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_count -= BUTTON_LONG_REPEAT_DELAY_MAX;

    //cursor at "cap"
    if ((!global.cap_sw && (MENU_var.reltest.cursor == 0))
        || (global.cap_sw && (MENU_var.reltest.cursor == 1)))
    {
      if (global.cap_relays == 0)
      {
        global.cap_relays = 1;
      } else
      {
        global.cap_relays = (uint8_t) (global.cap_relays << 1) & 0x7f;
      }
    }


    //cursor at "ind"
    if ((!global.cap_sw && (MENU_var.reltest.cursor == 1))
        || (global.cap_sw && (MENU_var.reltest.cursor == 0)))
    {
      if (global.ind_relays == 0)
      {
        global.ind_relays = 1;
      } else
      {
        global.ind_relays = (uint8_t) (global.ind_relays << 1) & 0x7f;
      }
    }

    if (MENU_var.reltest.cursor == 2) //cursor at "C-SW"
    {
      if (global.cap_sw == 0)
      {
        global.cap_sw = 1;
      } else
      {
        global.cap_sw = 0;
      }
      
      BUTTON_count = BUTTON_RESET;
    
    }

    if (MENU_var.reltest.cursor == 3) //cursor at "Esc"
    {
      //Restore C/L settings
      UTILI_Set_LC_Relays(MENU_var.reltest.relais_backup);
      MENU_Init();
      return;
    } 
    else
    {
      MENU_RelTest_Update();
    }
  
    UTILI_SetRelays();
  }
  
  
    //- Buttons Auto and Bypass for fine tuning
    
   uint8_t update =0;  
  
    //cursor at "cap"
    if ((!global.cap_sw && (MENU_var.reltest.cursor == 0))
        || (global.cap_sw && (MENU_var.reltest.cursor == 1)))
    {
      if(BUTTON_Bypass_count == 1)
      {
        if(global.cap_relays > 0) global.cap_relays--;
        update=1;
      }
      
      if(BUTTON_Auto_count == 1)
      {
        if(global.cap_relays < 0x7f) global.cap_relays++;
        update=1;
      }
    }
    
    
    //cursor at "ind"
    if ((!global.cap_sw && (MENU_var.reltest.cursor == 1))
        || (global.cap_sw && (MENU_var.reltest.cursor == 0)))
    {
      if(BUTTON_Bypass_count == 1)
      {
        if(global.ind_relays > 0) global.ind_relays--;
        update=1;
      }
      
      if(BUTTON_Auto_count == 1)
      {
        if(global.ind_relays < 0x7f) global.ind_relays++;
        update=1;
      }
    }
  
   
   if(update)
   {
    UTILI_SetRelays();
    MENU_RelTest_Update();
   }
    
  
}


//-- MENU_CalMenu -------------------------------------------------------------------------------------------------

static void MENU_CalPWR_Update(void)
{
  char str[2] = " ";
  
  str[0] = MENU_var.cal.step + '1';
  DISP_Str(0, 0,str_Point,0);
  DISP_Str(9, 0, str, 0);
  DISP_Str(0, 1,str_WaitForHF, 0);
  
  DISP_Str(0, 2, str_Power, 0);
  DISP_PWR(6, 2, MENU_var.cal.cal_point[MENU_var.cal.step], (MENU_var.cal.cursor == 0));

  DISP_Str(7, 3,str_Esc, (MENU_var.cal.cursor == 1));
}

static void MENU_CalPWR_Init(void)
{
  DISP_Clr();
  MENU_var.cal.cursor = 0;
  MENU_var.cal.step = 0;
  MENU_var.cal.cal_point[0] = global.cal_point[0];
  MENU_var.cal.cal_point[1] = global.cal_point[1];
  BUTTON_Reset();

  MENU_CalPWR_Update();

}

static void MENU_CalPWR_Run(void)
{
  if (BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.cal.cursor = MENU_var.cal.cursor ^ 0x01; // value 0 or 1
    MENU_CalPWR_Update();
  }


  if (BUTTON_count == BUTTON_LONG_PRESSED)
  {
    BUTTON_count -= BUTTON_LONG_REPEAT_DELAY_MAX;


    if ((MENU_var.cal.cursor == 0)&&(MENU_var.cal.step <=1)) //cursor at "Power"
    {
      int16_t value = MENU_var.cal.cal_point[MENU_var.cal.step];

      if (value < 100) value += 5;
      else value += 50;

      if (MENU_var.cal.step == 0)
      {
        if (value > CAL_STEP0_MAX) value = 0;
      } 
      else
      {
        //Value in step 1 could not be smaller than in step 0
        if (value > CAL_STEP1_MAX) value = MENU_var.cal.cal_point[0];
      }

      MENU_var.cal.cal_point[MENU_var.cal.step] = value;
      
      MENU_CalPWR_Update();

    } 
    else //cursor at "ESC"
    {
      MENU_Init();
      return;
    }
  }

  //ADC 
  if (MENU_var.cal.step <= 1) //step 0 & 1
  {
    if (global.adc_f_mV > ADC_CAL_START)
    {
      if (global.adc_f_mV == MENU_var.cal.adc_value_old) //stable value current == old
      {
        MENU_var.cal.adc_value[MENU_var.cal.step] = global.adc_f_mV; //save 

        DISP_Clr();
        DISP_Str(DISP_COL_CENTER, 1, str_Ok, 0);
        MENU_var.cal.step += 2; //steps 0->2 & 1->3
      }
    }
  } 
  else //step 2 & 3
  {
    //Wait end of power
    if (global.adc_f_mV < ADC_CAL_STOP)
    {
     
      MENU_var.cal.step--;  //steps 2->1 & 3->2
      if (MENU_var.cal.step == 1)
      {
        DISP_Clr();
        MENU_CalPWR_Update();
      } 
      else
      {
        global.cal_point[0] = MENU_var.cal.cal_point[0];
        global.cal_point[1] = MENU_var.cal.cal_point[1];

        //        char str[8];         
        //        UART_WriteStr("Point 0: ");
        //        UTILI_Int2Str(global.cal_point[0],str,sizeof(str));
        //        UART_WriteStrLn(str);        

        //        UART_WriteStr("Point 1: ");
        //        UTILI_Int2Str(global.cal_point[1],str,sizeof(str));
        //        UART_WriteStrLn(str); 

        int16_t y1 = UTILI_deciWatt_to_centiVolt(global.cal_point[0]);  
        int16_t y2 = UTILI_deciWatt_to_centiVolt(global.cal_point[1]);

        //        UART_WriteStr("Point V 0: ");
        //        UTILI_Int2Str(y1,str,sizeof(str));
        //        UART_WriteStrLn(str);        

        //        UART_WriteStr("Point V 1: ");
        //        UTILI_Int2Str(y2,str,sizeof(str));
        //        UART_WriteStrLn(str); 

        global.cal_gain = (int16_t) (((y2 - y1) * (int32_t) CAL_GAIN_MULTIPLIER) / (MENU_var.cal.adc_value[1] - MENU_var.cal.adc_value[0]));

        //        UART_WriteStr("Gain: ");
        //        UTILI_Int2Str(global.cal_gain,str,sizeof(str));
        //        UART_WriteStrLn(str); 

        global.cal_offset = y1 - (int16_t) (((int32_t) global.cal_gain * MENU_var.cal.adc_value[0]) / CAL_GAIN_MULTIPLIER);

        //        UART_WriteStr("Offset: ");
        //        UTILI_Int2Str(global.cal_offset,str,sizeof(str));
        //        UART_WriteStrLn(str); 

        //Save new cal values to eeprom
        EEPROM_Write((uint8_t)&ee_cal_point_0, &global.cal_point[0], sizeof (ee_cal_point_0));
        EEPROM_Write((uint8_t)&ee_cal_point_1, &global.cal_point[1], sizeof (ee_cal_point_1));
        EEPROM_Write((uint8_t)&ee_cal_offset, &global.cal_offset, sizeof (ee_cal_offset));
        EEPROM_Write((uint8_t)&ee_cal_gain, &global.cal_gain, sizeof (ee_cal_gain));

        MENU_Init();
        return;
      }
    }
  }

  MENU_var.cal.adc_value_old = global.adc_f_mV;



}




//static void MENU_Back_Init(void)
//{
//  const menu_t* ptrMenu;
//  if(stack_index > 1)
//  {
//    stack_index--;
//    stack_index--;
//    ptrMenu = (menu_t*) stack_menu[stack_index];
//    ptrMenu->Init(); 
//
//  }
//}


//-- MENU_Setup -------------------------------------------------------------------------------------------------
static void MENU_SubSetup_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();

  MENU_var.sub.cursor =0;
  MENU_var.sub.top=0;
  MENU_var.sub.timeout =0;
  MENU_var.sub.items = MENU_SUBSETUP_ITEMS;
  MENU_var.sub.ptrMenu = ptrSubSetupMenu;
  
  MENU_Sub_Update();
  
}

//-- MENU_Sub -------------------------------------------------------------------------------------------------


static void MENU_SubTop_Init(void)
{
  DISP_Clr();
  BUTTON_Reset();

  MENU_var.sub.cursor =0;
  MENU_var.sub.top=0;
  MENU_var.sub.timeout =0;
  MENU_var.sub.items = MENU_SUBTOP_ITEMS;
  MENU_var.sub.ptrMenu = ptrSubTopMenu;
  
  MENU_Sub_Update();
  
}


static void MENU_Sub_Update(void)
{
  //Show 4 lines of the sub menu and highlight the current cursor item
  uint8_t pos = MENU_var.sub.top;
  
  
  for(uint8_t i = 0; i < 4; i++)
  {
      DISP_Str(DISP_COL_CENTER, i, MENU_var.sub.ptrMenu[pos]->title, (pos == MENU_var.sub.cursor));
      pos++;
  }
}



static void MENU_Sub_Run(void)
{
  
  //Timeout
  MENU_var.sub.timeout++;
  if(MENU_var.sub.timeout >= MENU_SUB_TIMEOUT)
  {
    MENU_Init();
    return;
  }
  
  if(BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.sub.timeout=0;
    
    uint8_t cursor_pos = MENU_var.sub.cursor - MENU_var.sub.top;
    
    switch (cursor_pos)
    {
      //cursor in the 1st or in the 3rd line
    case 0:
    case 2:
      MENU_var.sub.cursor++;
      break;
      
      //cursor in the 2nd. line
    case 1:
      //Are more than 4 items ahead -> increase top position and cursor
      //if not only move cursor to 3rd line
      if((MENU_var.sub.top +4) < MENU_var.sub.items)
      {
        MENU_var.sub.top++;
      }
      MENU_var.sub.cursor++;
      break; 
   
    //cursor in the 4th line
    //case 3:
    default:  
      //Redraw menu from top
      MENU_var.sub.top=0;
      MENU_var.sub.cursor=0;
      break;
    }
      
    MENU_Sub_Update();
    
  }
  
   if (BUTTON_count == BUTTON_LONG_PRESSED)
   {
     current_menu = MENU_var.sub.ptrMenu[MENU_var.sub.cursor];
     current_menu->Init();
   }
  
 
}

//-- MENU_Load_Save_Memory -------------------------------------------------------------------------------------------------

static void MENU_Memory_Update(void)
{
  char str_index[3] = "0:";
  uint8_t index;
  union
  {
    tunemem_t tunemem;
    uint8_t bytes[sizeof(tunemem_t) +1]; //make the union one byte longer
  }memory;
  

  
  for(uint8_t i = 0; i < 3; i++)
  {
    index = MENU_var.memory.top + i;
    str_index[0] = '0' + index;
    DISP_Str(0, i+1,str_index,(index == MENU_var.memory.cursor));
    
    //Load item from eeprom
    EEPROM_Read((uint8_t)&ee_tunemem + (sizeof(tunemem_t) * index) , &memory, sizeof(tunemem_t));
    memory.bytes[sizeof(tunemem_t)] =0;     //Terminat string 
    DISP_Str(2,i+1,memory.tunemem.str_name, (index == MENU_var.memory.cursor));        
  }
}


static void MENU_Memory_Init(void)
{
  MENU_var.memory.cursor =0;
  MENU_var.memory.top=0;
  MENU_var.memory.timeout =0;
  MENU_Memory_Update();
  BUTTON_Reset();
}

static void MENU_MemoryLoad_Init(void)
{
  DISP_Clr();
  DISP_Str(0,0,str_MENU_Load,0);
  MENU_var.memory.save_mode =0;
  MENU_Memory_Init(); 
}

static void MENU_MemorySave_Init(void)
{
  DISP_Clr();
  DISP_Str(0,0,str_MENU_Save,0);
  MENU_var.memory.save_mode =1;
  MENU_Memory_Init(); 
}





static void MENU_Memory_Run(void)
{
  
  //Timeout
  MENU_var.memory.timeout++;
  if(MENU_var.memory.timeout >= MENU_MEMORY_TIMEOUT)
  {
    MENU_Init();
    return;
  }
  
  if(BUTTON_count == BUTTON_RELEASED)
  {
    MENU_var.memory.timeout=0;
    
    uint8_t cursor_pos = MENU_var.memory.cursor - MENU_var.memory.top;
    
    switch (cursor_pos)
    {
      //cursor in the 1st or in the 3rd line
    case 0:
      MENU_var.memory.cursor++;
      break;
      
      //cursor in the 2nd. line
    case 1:
      //Are more than 2 items ahead -> increase top position and cursor
      //if not only move cursor to 3rd line
      if((MENU_var.memory.top +3) < TUNEMEM_ITEMS)
      {
        MENU_var.memory.top++;
      }
      MENU_var.memory.cursor++;
      break; 
   
    //cursor in the 4th line
    //case 3:
    default:  
      //Redraw menu from top
      MENU_var.memory.top=0;
      MENU_var.memory.cursor=0;
      break;
    }
      
    MENU_Memory_Update();
    
  }

  if (BUTTON_count == BUTTON_LONG_PRESSED)
  {
    if (MENU_var.memory.save_mode == TRUE)
    {
      if(MENU_var.memory.cursor == 0) //Startup
      {
       current_menu = &menuStartupSave;

      }
      else
      {
       MENU_var.nameedit.mem_index = MENU_var.memory.cursor;
       current_menu = &menuNameEdit;
      }
      
      current_menu->Init();
      return;
    }
    else
    {
      //Load
      tunemem_t tunemem;
      EEPROM_Read((uint8_t) & ee_tunemem + (sizeof (tunemem_t) * MENU_var.memory.cursor) , &tunemem, sizeof (tunemem_t));
      UTILI_Set_LC_Relays(tunemem.relays);
      MENU_Init();
      return;
    }
  }
  
  
}

//-- MENU_NameEdit -------------------------------------------------------------------------------------------------

static void MENU_NameEdit_Update(void)
{
  uint8_t i;
  
  
  //name
  for(i=0; i<TUNEMEM_STRLEN; i++)
  {
    DISP_Char(2+i, 0, MENU_var.nameedit.tunemem.str_name[i],(MENU_var.nameedit.cursor == i));
  }
  
  
  //char list
  
  
  
  if(MENU_var.nameedit.cursor < TUNEMEM_STRLEN)
  {
    
    
    //find the position of the char under the cursor
    char c = MENU_var.nameedit.tunemem.str_name[MENU_var.nameedit.cursor];
    i=0;
    while( (c != NameEditChar[i]) && (i<sizeof(NameEditChar)) )
    {
      i++;
    }
    
    DISP_Char(2,1,c,1); //print char under cursor invert
    
    //print the following char
    for(uint8_t x=1; x< 8; x++)
    {
      i++;
      if (i >= (sizeof(NameEditChar)-1) )
      {
         i=0;
      }
      DISP_Char(2+x,1,NameEditChar[i],0);
    }
  }
  else //is cursor at save or ESC print empty line
  {
    DISP_Str(0, 1, str_SpaceLine,0);
  }
  
 
 
  
  
  
   DISP_Str(0,3,str_Save, (MENU_var.nameedit.cursor == TUNEMEM_STRLEN) );
   DISP_Str(7,3,str_Esc, (MENU_var.nameedit.cursor == TUNEMEM_STRLEN+1));
}


static void MENU_NameEdit_Init(void)
{
  
  //print memory slot
  char str_index[3] = "0:";
  str_index[0] = '0' + MENU_var.nameedit.mem_index;
  DISP_Clr();
  DISP_Str(0, 0,str_index,0);
  
  EEPROM_Read((uint8_t)&ee_tunemem + (sizeof(tunemem_t) * MENU_var.nameedit.mem_index) , &MENU_var.nameedit.tunemem, sizeof(tunemem_t));
  MENU_var.nameedit.cursor=0;
  MENU_var.nameedit.button_repeat_delay = BUTTON_LONG_REPEAT_DELAY_MAX;
  BUTTON_Reset();
  MENU_NameEdit_Update();
  
}

static void MENU_NameEdit_Run(void)
{
  
  if(BUTTON_count == BUTTON_IDLE)
  {
    MENU_var.nameedit.button_repeat_delay = BUTTON_LONG_REPEAT_DELAY_MAX;
  }
  
  
  if(BUTTON_count == BUTTON_RELEASED)
  {
    //move cursor
    MENU_var.nameedit.cursor++;
    if(MENU_var.nameedit.cursor > TUNEMEM_STRLEN+1)
    {
      MENU_var.nameedit.cursor=0;
    } 
    MENU_NameEdit_Update();
  }
  
   if (BUTTON_count == BUTTON_LONG_PRESSED)
   {
      BUTTON_count -= MENU_var.nameedit.button_repeat_delay;
     
      if(MENU_var.nameedit.button_repeat_delay > BUTTON_LONG_REPEAT_DELAY_MIN)
      {
        MENU_var.nameedit.button_repeat_delay -= BUTTON_LONG_REPEAT_DELAY_STEP;
      }
      
      
     if(MENU_var.nameedit.cursor < TUNEMEM_STRLEN)
     {
       
       char c = MENU_var.nameedit.tunemem.str_name[MENU_var.nameedit.cursor];
       
       uint8_t i=0;
       while( (c != NameEditChar[i]) && (i<sizeof(NameEditChar)) )
       {
         i++;
       }
       i++; //next char in list
       if (i >= (sizeof(NameEditChar)-1) )
       {
         i=0;
       }
       MENU_var.nameedit.tunemem.str_name[MENU_var.nameedit.cursor] = NameEditChar[i];
              
       MENU_NameEdit_Update();
     }
     
    
     
     //Save
     if(MENU_var.nameedit.cursor == TUNEMEM_STRLEN)
     {
       MENU_var.nameedit.tunemem.relays = UTILI_Get_LC_Relays(); //Save current relais
       EEPROM_Write((uint8_t)ee_tunemem + (sizeof(tunemem_t) * MENU_var.nameedit.mem_index) , &MENU_var.nameedit.tunemem, sizeof(tunemem_t));
       MENU_Init();
       return;
     }
     
     //Esc
     if(MENU_var.nameedit.cursor == TUNEMEM_STRLEN+1)
     {
       MENU_Init();
       return;
     }
             
             
   }
}



//-- MENU_StartupSave -------------------------------------------------------------------------------------------------

static void MENU_StartupSave_Update(void)
{
   DISP_Str(0,3,str_Save, (MENU_var.startupsave.cursor == 0));
   DISP_Str(7,3,str_Esc, (MENU_var.startupsave.cursor == 1));
}


static void MENU_StartupSave_Init(void)
{
  DISP_Clr();
  char str_index[3] = "0:";
  DISP_Str(0, 1,str_index,0);
  
  EEPROM_Read((uint8_t)&ee_tunemem , &MENU_var.startupsave.tunemem, sizeof(tunemem_t));
  MENU_var.startupsave.str_terminate=0; //Terminat string 
  DISP_Str(2, 1,MENU_var.startupsave.tunemem.str_name, 0);  
  
  MENU_var.startupsave.cursor=0; 
  BUTTON_Reset();
  MENU_StartupSave_Update();
}

static void MENU_StartupSave_Run(void)
{
  if(BUTTON_count == BUTTON_RELEASED)
  {
    //move cursor
    MENU_var.startupsave.cursor++;
    if(MENU_var.startupsave.cursor > 1)
    {
      MENU_var.startupsave.cursor=0;
    } 
    MENU_StartupSave_Update();
  }
  
   if (BUTTON_count == BUTTON_LONG_PRESSED)
   {
     //Save
     if(MENU_var.startupsave.cursor == 0)
     {
       MENU_var.startupsave.tunemem.relays = UTILI_Get_LC_Relays(); //Save current relais      
       EEPROM_Write((uint8_t)ee_tunemem , &MENU_var.startupsave.tunemem, sizeof(tunemem_t));
     }
      
     MENU_Init();
     return;
   }
}



//-- MENU About --------------------------------------------------------------------------------------------

static void MENU_About_Init(void)
{
  DISP_Clr();
  DISP_Str(DISP_COL_CENTER,0,str_Hardware,0);
  DISP_Str(DISP_COL_CENTER,1,str_designed,0);
  DISP_Str(DISP_COL_CENTER,2,str_by,0);
  DISP_Str(DISP_COL_CENTER,3,str_N7DCC,0);
  BUTTON_Reset();
  MENU_var.about.step=0;
}

static void MENU_About_Run(void)
{

  if(BUTTON_count == BUTTON_SHORT_PRESSED)
  {
    if(MENU_var.about.step ==0)
    {
      MENU_var.about.step++;
      DISP_Clr();
      DISP_Str(DISP_COL_CENTER,0,str_YetAnother,0);
      DISP_Str(DISP_COL_CENTER,1,str_Firmware,0);
      DISP_Str(DISP_COL_CENTER,2,str_by,0);
      DISP_Str(DISP_COL_CENTER,3,str_DG4SN,0);
    }
    else
    {
      MENU_Init();
      return;
    }
  }
}  
  

//-- MENU  -------------------------------------------------------------------------------------------------
void MENU_Init(void)
{
  current_menu = (menu_t*) & menuMain;
  MENU_Main_Init();
}

void MENU_Run(void)
{
  if(current_menu->Run != NULL)
  {
    current_menu->Run();
  }
}

