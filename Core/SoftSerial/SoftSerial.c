#include "SoftSerial.h"

uint16_t SoftSerial_RX_PIN[SoftSerial_Max] = { GPIO_PIN_7 };
GPIO_TypeDef *SoftSerial_RX_PORT[SoftSerial_Max] = { GPIOC };

struct {
	uint8_t RxStat[SoftSerial_Max], RxCh[SoftSerial_Max], RxBit[SoftSerial_Max];
	uint8_t RxData[SoftSerial_Max][SoftSerial_MaxBufer], RxIndex;
	uint16_t RxChbuf[SoftSerial_Max];
	uint64_t RxTimeH[SoftSerial_Max], RxTimeL[SoftSerial_Max];
	uint16_t RxTime[SoftSerial_Max], RxBitTime;
	uint16_t TimerOver;
} softSerial;

enum {
	StatHILOW = 1, StatBusy = 2, StatAdresPos = 2, StatAdresMask = 28
};

int debug[2][12] = { 0 }, index = 0;

void SoftSerial_GPIOCallBack() {
	int mask = EXTI->IMR;
	EXTI->PR = mask;

	for (int i = 0; i < SoftSerial_Max; i++) {
		if ((SoftSerial_RX_PIN[i] & mask) == SoftSerial_RX_PIN[i]) {
			SoftSerial_UpdateTime(i);
			SoftSerial_UpdateData(i);
		}
	}
}

void SoftSerial_TimerCallBack() {
	if ((SoftSerial_Timer->SR & TIM_SR_UIF) == TIM_SR_UIF) {
		SoftSerial_Timer->SR &= ~TIM_SR_UIF;
		softSerial.TimerOver++;
	}

	for (int i = 0; i < SoftSerial_Max; i++) {
		int max = SoftSerial_GetTime() + softSerial.RxBitTime * 20;
		if (softSerial.RxTimeH[i] <= max) {
			if (softSerial.RxStat[i] & StatBusy) {
				SoftSerial_MakeData(i, 255);
			}
			softSerial.RxStat[i] &= ~StatBusy;
		}
	}
}

uint64_t SoftSerial_GetTime() {
	uint64_t t = softSerial.TimerOver * 65536;
	t += SoftSerial_Timer->CNT;
	return t;
}

void SoftSerial_UpdateTime(uint8_t i) {
	if ((SoftSerial_RX_PORT[i]->IDR & SoftSerial_RX_PIN[i]) == SoftSerial_RX_PIN[i]) {
		if (!(softSerial.RxStat[i] & StatHILOW)) {
			softSerial.RxStat[i] |= StatHILOW;
			softSerial.RxTimeH[i] = SoftSerial_GetTime();
			softSerial.RxTime[i] = softSerial.RxTimeH[i] - softSerial.RxTimeL[i];
		}
	} else {
		if (softSerial.RxStat[i] & StatHILOW) {
			softSerial.RxStat[i] &= ~StatHILOW;
			softSerial.RxTimeL[i] = SoftSerial_GetTime();
			softSerial.RxTime[i] = softSerial.RxTimeL[i] - softSerial.RxTimeH[i];
		}
	}
}

void SoftSerial_UpdateData(uint8_t i) {

	if (softSerial.RxTime[i] >= softSerial.RxBitTime * 10) {
		if (softSerial.RxStat[i] & StatBusy) {
			SoftSerial_MakeData(i, 255);
		}
		softSerial.RxStat[i] &= ~StatBusy;
	}

	debug[0][index] = softSerial.RxTime[i];
	debug[1][index++] = softSerial.RxStat[i] & StatHILOW;

	if (!(softSerial.RxStat[i] & StatBusy)) {
		if (!(softSerial.RxStat[i] & StatHILOW)) {
			softSerial.RxStat[i] |= StatBusy;
			softSerial.RxBit[i] = 0;
			softSerial.RxChbuf[i] = 0;
			index = 0;
		}
	} else if (softSerial.RxStat[i] & StatBusy) {
		int t = softSerial.RxTime[i] / softSerial.RxBitTime;
		SoftSerial_MakeData(i, t);
	}
}

void SoftSerial_MakeData(uint8_t i, uint8_t t) {
	static char lastb = 0;
	char b = !(softSerial.RxStat[i] & StatHILOW);

	if (t == 255) {
		t = 9 - softSerial.RxBit[i];
		b = !lastb;
	}

	if (b) {
		for (int a = 0; a < t; a++)
			softSerial.RxChbuf[i] |= 1 << (softSerial.RxBit[i] + a);
	}
	lastb = b;
	softSerial.RxBit[i] += t;

	if (softSerial.RxBit[i] >= 9) {
		softSerial.RxCh[i] = softSerial.RxChbuf[i] >> 1;
		softSerial.RxStat[i] &= ~StatBusy;

		softSerial.RxData[i][softSerial.RxIndex++] = softSerial.RxCh[i];
		if (softSerial.RxIndex >= SoftSerial_MaxBufer)
			softSerial.RxIndex = 0;
	}
}

void SoftSerial_Init() {

	SoftSerial_Timer->DIER |= TIM_DIER_UIE;
	SoftSerial_Timer->CR1 |= TIM_CR1_CEN;

	softSerial.RxBitTime = (1000000 / (SoftSerial_Bund * SoftSerial_Timer_Invterval_us));
	for (int i = 0; i < SoftSerial_Max; i++) {
		SoftSerial_UpdateTime(i);
	}
}
