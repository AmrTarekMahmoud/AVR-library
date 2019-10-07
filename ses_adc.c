#include <avr/io.h>
#include "ses_common.h"
#include "ses_adc.h"
//A temperature sensor at pin 2 of port F
//A light sensor at pin 4 of port F
//The joy stick at pin 5 of port F
//A microphone connected differentially to the pins 0 and and 1 of port F
#define TEMP_SENSOR_PIN 2
#define LIGHT_SENSOR_PIN 4
#define JOYSTICK_PIN 5
#define MICROPHONE_PIN_1 0
#define MICROPHONE_PIN_2 1
#define ADC_JOYSTICK_CH 6
#define ADC_VREF_SRC 1
#define ADC_PRESCALE 128

void adc_init(void){
	//setting data directory register for port F to output
	DDRF &= ~(1<<TEMP_SENSOR_PIN) | (1<<LIGHT_SENSOR_PIN) | (1<<JOYSTICK_PIN) | (1<<MICROPHONE_PIN_1) | (1<<MICROPHONE_PIN_2);
	//deactivate internal pull up resistors
	PORTF &= ~(1<<TEMP_SENSOR_PIN) | (1<<LIGHT_SENSOR_PIN) | (1<<JOYSTICK_PIN) | (1<<MICROPHONE_PIN_1) | (1<<MICROPHONE_PIN_2);
	//Disable power reduction mode for the ADC module
	PRR0 &= ~(1<<PRADC);
	//set reference voltage
	ADMUX |= (ADC_VREF_SRC << REFS0) | (ADC_VREF_SRC<<REFS1);
	//set the ADC right adjust
	ADMUX &= ~(1 << ADLAR);
	//set prescaler , F_ADC=F_CPU/pre-scaler. taking in account that we need the fastest possible operation
	// we choose a prescaler that will achieve the max number of cycles to use to get the max resolution
	// we choose prescaler 128 that will give 125kHz of ADC frequency
	// then we enable the ADPS0 and ADPS1 and clear ADPS2. we also set the ADEN all in the ADCSRA register.
	ADCSRA |=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

}
uint16_t adc_read(uint8_t adc_channel){
	// we will create a switch case for the input of the function to determine which channel will be open
	// for conversion
	/*switch(adc_channel){
	case ADC_MIC_NEG_CH:
		ADMUX |=(1<<MICROPHONE_PIN_1);
		break;
	case ADC_MIC_POS_CH:
		ADMUX |=(1<<MICROPHONE_PIN_2);
		break;
	case ADC_TEMP_CH:
		ADMUX |=(1<<TEMP_SENSOR_PIN);
		break;
	case ADC_LIGHT_CH:
		ADMUX |=(1<<LIGHT_SENSOR_PIN);
		break;
	case ADC_JOYSTICK_CH:
		ADMUX |=(1<<JOYSTICK_PIN);
		break;
	default:
		return (ADC_INVALID_CHANNEL);
	}*/
	ADMUX |=(1<<2);
	// hold in while loop until conversion is completed
	while ((ADCSRA & (1 << ADSC)) != 0);
	return(ADC);
}


