/* 
 * File:   uart.h
 * Author: DG4SN
 *
 * Created on 12. März 2022, 13:06
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C"
{
#endif

#ifdef DEBUG_UART  

#define UART_BUFFER_SIZE    256
  
  
void UART_Init(void);  
void UART_WriteChar(char c); 
void UART_WriteLn(void);
void UART_WriteStr(char *str);
void UART_WriteStrLn(char *str);  
void UART_WriteInt(int16_t value);
uint8_t UART_Busy(void);

void UART_Run(void);

#endif



#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

