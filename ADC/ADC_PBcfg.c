#include "ADC_LCfg.h"
#include "tm4c123gh6pmRegisters.h.h"
#include "common_macros.h"

stADC_Cfg_t ADC_Cfg =
{
	ADC_Num0,
	ADC_Sequencer3,
	ADC_TRIGGER_PROCESSOR,
	1,//One Channel
	{AIN1_E2},
	INTERRUPT_DISABLE,
	{INTERRUPT_EN},
	ADC_InputChannel,
	SingleEnded
}; 