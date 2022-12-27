#include "defines.h"

#ifdef DEBUG_UART

//  the CPU clock is 4 Mhz, so to convert to microseconds, there are
//  4 clocks per usec

#define BD9600      400
#define BD38400     84
#define BD115200    18
#define BIT_DELAY() _delay((unsigned long)(BD115200))  



#define ESC_CLR_SCREEN    "\033[2J"
#define ESC_CURSOR_HOME   "\033[H"

static char uart_buffer[UART_BUFFER_SIZE];
static uint8_t uart_write_pos;
static uint8_t uart_read_pos;
static uint8_t uart_step;
static uint8_t uart_cap_sw;
static uint8_t uart_coarse_cap;
static uint8_t uart_coarse_ind;

void UART_Init(void)
{
  UART_PIN_W = 1;
  UART_PIN_DIR = DIR_OUTPUT;
  uart_read_pos=0;
  uart_write_pos=0;
  uart_step=0;
}

uint8_t UART_Busy(void)
{
  return(uart_write_pos);
}


void UART_SendChar(char c)
{
  unsigned char i;
  INTCONbits.GIE = 0; // Disable Global Interrupt
  UART_PIN_W = 0; // Start bit
  BIT_DELAY();
  for (i = 8; i != 0; --i)
  {
    if (c & 0x01)
    {
      UART_PIN_W = 1; // Begin with LSB
    } else
    {
      UART_PIN_W = 0;
    }
    c >>= 1;
    BIT_DELAY();
  }
  UART_PIN_W = 1; // Stop bit
  INTCONbits.GIE = 1; // Enable Global Interrupt
  BIT_DELAY();
  BIT_DELAY(); //  add some extra delays after the stop bit

}

void UART_WriteChar(char c)
{
  if (uart_write_pos < (UART_BUFFER_SIZE - 1))
  {
    uart_buffer[uart_write_pos++] = c;
  }
}

void UART_WriteStr(char *str)
{
  while (*str != 0)
  {
    UART_WriteChar(*str++);
  }
}

void UART_WriteStrLn(char *str)
{
  UART_WriteStr(str);
  UART_WriteChar(0x0d); // send a CR
  UART_WriteChar(0x0a); // send a LF
}



void UART_WriteLn(void)
{
  UART_WriteChar(0x0d); // send a CR
  UART_WriteChar(0x0a); // send a LF
}


void UART_WriteInt(int16_t value)
{
  char buffer[5];
  UTILI_Int2Str(value, buffer, sizeof(buffer));
  UART_WriteStr(buffer);
}



void UART_Run(void)
{
  if (uart_read_pos != uart_write_pos)
  {
    UART_SendChar(uart_buffer[uart_read_pos++]);
  }
  else
  {
    uart_read_pos=0;
    uart_write_pos=0;
  }
}


//void UART_Run(void)
//{
//  if (uart_read_pos != uart_write_pos)
//  {
//    UART_SendChar(uart_buffer[uart_read_pos++]);
//  }
//  else
//  {
//    uart_read_pos=0;
//    uart_write_pos=0;
//
//    switch (uart_step) {
//
//    case 0:
//      UART_WriteStr(ESC_CLR_SCREEN);
//      uart_step=1;
//      break;
//
//    case 1:
//      uart_cap_sw =0;
//      uart_coarse_cap =0;
//      uart_coarse_ind =0;
//      UART_WriteStr(ESC_CURSOR_HOME);
//      uart_step=2;
//      break;
//
//    case 2:
//      UTILI_Int2Str(TUNE_results[uart_cap_sw][uart_coarse_ind][uart_coarse_cap], uart_buffer, 5);
//      uart_write_pos=5;
//
//      uart_coarse_cap++;
//      if (uart_coarse_cap >= COARSE_STEPS)
//      {
//        uart_coarse_cap=0;
//        uart_coarse_ind++;
//        UART_WriteChar(0x0d); // send a CR
//        UART_WriteChar(0x0a); // send a LF
//
//        if (uart_coarse_ind >= COARSE_STEPS)
//        {
//          uart_coarse_ind=0;
//          UART_WriteChar(0x0d); // send a CR
//          UART_WriteChar(0x0a); // send a LF
//          if (uart_cap_sw == 0)
//          {
//            uart_cap_sw = 1;
//          } else
//          {
//            uart_step=1;
//          }
//
//        }
//      }
//      break;
//
//    default:
//      uart_step=0;
//      break;
//
//    }
//  }
//}

#endif