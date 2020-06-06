#ifndef __ADC_H__
#define __ADC_H__



/*
 * Fun----------: ERROR_STATUS ADC_Init(void);
 * Input--------: Nothing
 * Output-------: Nothing
 * Return-------: ERROR_STATUES
 * Description--: initializes the the ADC Module 
*/
extern ERROR_STATUS ADC_Init(void);

/*
 * Fun----------: ERROR_STATUS ADC_READ(void);
 * Input--------: Nothing
 * Output-------: Reading Buffer that contains the readings of each sample respectively
 * Return-------: ERROR_STATUES
 * Description--: This function used to convert Analog value to Digital value for each sample 
 *                that is given by the user
*/
extern ERROR_STATUS ADC_READ(uint16_t * ReadingBuffer);

#endif