#include "tm4c123gh6pmRegisters.h"
#include "ADC.h"
#include "ADC_LCfg.h"
#include "MCU_hw.h"
#include "common_macros.h"
/***********************************************/
/*            Static Globals                   */
/***********************************************/
static uint8_t gu8InitFlag = FALSE;
/*
 * Fun----------: ERROR_STATUS ADC_Init(void);
 * Input--------: Nothing
 * Output-------: Nothing
 * Return-------: ERROR_STATUES
 * Description--: initializes the the ADC Module 
*/
ERROR_STATUS ADC_Init(void)
{
	uint8_t i=0,au8Sequencer_no;
	uint32_t ADC,ADCMUX,ADCControl;
	ERROR_STATUS ERR = E_NOK;
	//Check if the module is already initialized or not
	if(gu8InitFlag)
	{
		ERR =  E_MULTIPLE_INITIALIZION;
	}
	else
	{
		ERR =E_NOK;
		//Check on the ADC configuration that inputed by the user in ADC_LCfg file
		if(((ADC_Cfg.ADC_NUM != ADC_Num0)&&(ADC_Cfg.ADC_NUM != ADC_Num0))||//Invalid ADC Number
			  ((ADC_Cfg.ADC_Sequencer != ADC_Sequencer0)&&//Invalid sequencer
			  (ADC_Cfg.ADC_Sequencer != ADC_Sequencer1) &&//Invalid sequencer
			  (ADC_Cfg.ADC_Sequencer != ADC_Sequencer2) && //Invalid sequencer
			  (ADC_Cfg.ADC_Sequencer != ADC_Sequencer3))||//Invalid sequencer
				(ADC_Cfg.ADC_Sequencer == ADC_Sequencer0 && ADC_Cfg.NumoFChannels > SEQ0_MAX_CHANNELS)||//Invalid Num of channels
			  (ADC_Cfg.ADC_Sequencer == ADC_Sequencer1 && ADC_Cfg.NumoFChannels > SEQ1_MAX_CHANNELS)||//Invalid Num of channels
				(ADC_Cfg.ADC_Sequencer == ADC_Sequencer2 && ADC_Cfg.NumoFChannels > SEQ2_MAX_CHANNELS)||//Invalid Num of channels
			  (ADC_Cfg.ADC_Sequencer == ADC_Sequencer3 && ADC_Cfg.NumoFChannels > SEQ3_MAX_CHANNELS)||//Invalid Num of channels
       	(ADC_Cfg.ADC_IRQ != INTERRUPT_DISABLE && ADC_Cfg.ADC_IRQ != INTERRUPT_EN) ||//Invalid Interrupt mode
			  ((ADC_Cfg.ADC_InputMode != ADC_InputChannel)      &&//Invalid ADC_InputMode mode
        (ADC_Cfg.ADC_InputMode != ADC_InternalTempSensor) &&//Invalid ADC_InputMode mode
        (ADC_Cfg.ADC_InputMode != ADC_BOTH))              ||//Invalid ADC_InputMode mode
				(ADC_Cfg.DigitalOutputMode != SingleEnded && ADC_Cfg.DigitalOutputMode != Differential))//Invalid Digital output mode
			{
				ERR = E_INVALID_PARAMETER;
			}
		else
			{

				//Get Sequencer Number
				au8Sequencer_no = log2(ADC_Cfg.ADC_Sequencer);

				//Get the offset of the adcmux
				ADCMUX = ADCSSMUX0 + (ADC_OFFSET_STEP*au8Sequencer_no);

				//Get the offset of the adcCTL
				ADCControl = ADCSSCTL0 + (ADC_OFFSET_STEP*au8Sequencer_no);

				//Get the Base address of the adc
				ADC= ADC0+ADC_BASE_STEP*ADC_Cfg.ADC_NUM;

				//Disable the Sequencer
				CLR_BIT_MASK(ADCREG(ADC,ADCACTSS),ADC_Cfg.ADC_Sequencer);

				//Choose Software Trigger
				SET_BIT_MASK(ADCREG(ADC,ADCEMUX),(ADC_Cfg.ADC_Trigger << (REG_STEP*au8Sequencer_no)));

				//Set the sequencer interrupt mask
				(ADCREG(ADC,ADCIM)) |= ADC_Cfg.ADC_IRQ<<au8Sequencer_no;

				//Select the Input Channel
				for(i = 0 ; i < ADC_Cfg.NumoFChannels ; i++)
				{
					/*Select the MUX*/
					(ADCREG(ADC,ADCMUX)) |= ADC_Cfg.ADC_Channels[i];

					/*Adjust the Control register*/

					//Single or diffrential
					(ADCREG(ADC,ADCControl)) |= (ADC_Cfg.DigitalOutputMode << ((i*REG_STEP)+D_BIT)); //sift the must in case the sequencer is not =0

					//Check whether it's the final sample or not
					if(i == ADC_Cfg.NumoFChannels-1)
					{
						//SET THE END BIT
						SET_BIT((ADCREG(ADC,ADCControl)),(END_BIT+(i*REG_STEP)));
					}
					else
					{
						//CLR THE END BIT
						CLR_BIT((ADCREG(ADC,ADCControl)),(END_BIT+(i*REG_STEP)));
					}

					//Set the interrupt mask of the channel 
					(ADCREG(ADC,ADCControl)) |= (ADC_Cfg.ChannelsIRQ[i] << ((i*REG_STEP)+IE_BIT));

				}
				if(ADC_Cfg.ADC_InputMode == ADC_InternalTempSensor || ADC_Cfg.ADC_InputMode==ADC_BOTH)
				{
					//SET THE Temp Sensor bit
					SET_BIT((ADCREG(ADC,ADCControl)),(TS_BIT+(i*REG_STEP)));
				}
				else
				{
					//CLR THE Temp Sensor bit
					CLR_BIT((ADCREG(ADC,ADCControl)),(TS_BIT+(i*REG_STEP)));
				}

				//Enable the Sequencer
				SET_BIT_MASK(ADCREG(ADC,ADCACTSS),ADC_Cfg.ADC_Sequencer);
				gu8InitFlag  = TRUE;
				ERR = E_OK;
			}

		}
		return ERR;

}



ERROR_STATUS ADC_READ(uint16_t * ReadingBuffer)
{

	uint8_t i=0,au8Sequencer_no;
	uint32_t ADCFFIFO,ADC;
	ERROR_STATUS ERR =E_NOK;
	//Check Whether the module is initialized
	if(gu8InitFlag)
	{
		//Check Whether on the NULL PTR
		if(ReadingBuffer == NULL)
		{
			ERR =E_NULL_PTR;
		}
		else
		{
			//Get Sequencer Number
			au8Sequencer_no = log2(ADC_Cfg.ADC_Sequencer);

			//Get the offset of the adcFIFO
			ADCFFIFO = ADCSSFIFO0 + (ADC_OFFSET_STEP*au8Sequencer_no);

			//Get the Base address of the adc
			ADC= ADC0+ADC_BASE_STEP*ADC_Cfg.ADC_NUM;


			for(i=0 ; i<ADC_Cfg.NumoFChannels;i++)
			{
				ADCREG(ADC,ADCPSSI) |= ADC_Cfg.ADC_Sequencer;
				//Start Conversion and Wait
				while(!(ADCREG(ADC,ADCRIS) & ADC_Cfg.ADC_Sequencer));
				//Save the Reading in the Buffer
				ReadingBuffer[i] = ADCREG(ADC,ADCFFIFO);
				(ADCREG(ADC,ADCISC)) = ADC_Cfg.ADC_Sequencer;
			}
			ERR = E_OK;
		}
	}
	else
	{
		ERR =E_UNINITIALIZED_MODULE;
	}
	return ERR;
} 