#ifndef _srf05_h_
#define _srf05_h_

#include "stm8s.h"

int SRF05_Init(void);
int SRF05_AutoPoolEnable(void);
int SRF05_AutoPoolDisable(void);
float SRF05_GetDistance(void);
void SRF05_ProcessTrigger(void);

#endif
