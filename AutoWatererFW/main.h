/*
 * main.h
 *
 * Created: 7/4/2019 11:38:11 AM
 *  Author: Shinji Ikari
 */ 


#ifndef MAIN_H_
#define MAIN_H_

//Scheduler variables
volatile uint8_t t10mSecCtr = 0;
volatile uint8_t t100mSecCtr = 0;
volatile uint8_t t500mSecCtr = 0;
volatile uint8_t t1SecCtr = 0;
volatile uint16_t Timer10msOld = 0;
volatile uint16_t Timer100msOld = 0;
volatile uint16_t Timer500msOld = 0;
volatile uint16_t Timer1sOld = 0;

//Pump timer variables
volatile uint32_t pumpSchedCtr = 0;
volatile uint32_t pumpRunCtr = 0;
volatile uint32_t pumpSchedTop = 0;

//Pump timer defines
#define oneHour	3600

//Bit definitions
#define pumpOvr				0
#define pumpReq				1

//IO Macros
#define Pump_OFF			PORTD &= ~(1 << PIND5);
#define Pump_ON				PORTD |= (1 << PIND5);

#define readButton1			!(PINB & (1 << PINB6))		//Active-low logic


#endif /* MAIN_H_ */