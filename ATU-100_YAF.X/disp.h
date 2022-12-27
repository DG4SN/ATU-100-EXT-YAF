/* 
 * File:   disp.h
 * Author: sven
 *
 * Created on 23. März 2022, 15:38
 */

#ifndef DISP_H
#define	DISP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define DISP_COL_CENTER   0x80
  

void DISP_Init(uint8_t i2_addr, uint8_t rotate);
void DISP_Str(uint8_t col, uint8_t row, const char *str, uint8_t invert);
void DISP_Char(uint8_t col, uint8_t row, char c, uint8_t invert);
void DISP_PWR(uint8_t col, uint8_t row, int16_t deciWatt, uint8_t invert);
void DISP_SWR(uint8_t col, uint8_t row, int16_t centiSWR, uint8_t invert);
void DISP_On(void);
void DISP_Off(void);
void DISP_Clr(void);


#ifdef	__cplusplus
}
#endif

#endif	/* DISP_H */

