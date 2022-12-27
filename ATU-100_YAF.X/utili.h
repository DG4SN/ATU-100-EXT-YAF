/* 
 * File:   utili.h
 * Author: sven
 *
 * Created on 22. März 2022, 16:45
 */

#ifndef UTILI_H
#define	UTILI_H

#ifdef	__cplusplus
extern "C"
{
#endif

void UTILI_Delay_ms(const unsigned int time_in_ms);
//uint8_t UTILI_Bcd2Dec(uint8_t bcdnum);
//uint8_t UTILI_Dec2Bcd(uint8_t decnum);
void UTILI_Int2Str(int16_t number, char *output, uint8_t size);
int16_t UTILI_deciWatt_to_centiVolt(int16_t deciWatt);
int16_t UTILI_centiVolt_to_deciWatt(int16_t centiVolt);
void UTILI_CalPWR(void);
void UTILI_CalSWR(void);

void UTILI_SetRelays(void);

void UTILI_Set_LC_Relays(uint16_t relais);
uint16_t UTILI_Get_LC_Relays(void);

int16_t UTILI_GetCapValue(void);
int16_t UTILI_GetIndValue(void);

void UTILI_GetCapValueStr(char *str, uint8_t str_size);
void UTILI_GetIndValueStr(char *str, uint8_t str_size);

//float UTILI_sqrt(float number);



#ifdef	__cplusplus
}
#endif

#endif	/* UTILI_H */

