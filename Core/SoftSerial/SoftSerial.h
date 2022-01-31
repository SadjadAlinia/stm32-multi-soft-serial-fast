#include "main.h"

#define SoftSerial_Max							1
#define SoftSerial_MaxBufer						200
#define SoftSerial_Timer						TIM1
#define SoftSerial_Timer_Invterval_us			10
#define SoftSerial_Bund							1200

void SoftSerial_GPIOCallBack();
void SoftSerial_TimerCallBack();
uint64_t SoftSerial_GetTime();
void SoftSerial_UpdateTime(uint8_t i);
void SoftSerial_UpdateData(uint8_t i);
void SoftSerial_MakeData(uint8_t i, uint8_t t);
void SoftSerial_Init();
