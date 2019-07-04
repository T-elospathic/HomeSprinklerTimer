/*
 * AutoWatererFW.c
 *
 * Created: 7/4/2019 11:37:27 AM
 * Author : Shinji Ikari
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include "Calibrations.h"
#include "main.h"
#include <util/delay.h>
#include <avr/interrupt.h>

//Local variables
volatile uint8_t pumpRequests = 0;

void Init_IO(){
	//Initialize IO ports
	DDRB |= 0b00010000;
	DDRC |= 0b00000000;
	DDRD |= 0b00100000;
	
	PORTB |= 0b01000000;
	PORTC |= 0b00000000;
	PORTD |= 0b00000000;
	
	//Interrupts
	//PCICR |= (1 << PCIE2);		//Enable PCINT 2, for buttons
	//PCMSK2 |= (1 << PCINT16) | (1 << PCINT17);		//Enable interrupts for both buttons
}
void Init_Timer()
{
	//Initialize timer
	
	//1ms timer
	TCCR1A |= (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (0 << WGM10);	//Non-inverting PWM, fast PWM mode
	TCCR1B |= (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (1 << CS10);			//ICR1 = TOP, 8x prescaler
	TIMSK1 |= (1 << TOIE1);
	ICR1 = 249;		//249 = 1kHz, 1ms period
	
}
void Init_ADC()
{
	//Initialize the ADC
	ADMUX |= 0x41;	//Set AVCC as reference and ADC1 as input
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (0 << ADPS1) | (1 << ADPS0);		//Enable ADC, start conversion, enable auto triggering, clk/2 prescaler
	ADCSRB = 0x00;
}
void checkTimerKnob()
{
	//Check timer knob ADC and adjust frequency of sprays
	//86,400 = 1 day/24 hours
	//3600 = 1 hour
	if ((pumpSchedTop < adcScaler - 10) || (pumpSchedTop > adcScaler + 10))
	{
		//Knob has moved, reset timer and modify top and ensure pump is off (in case of change while pump running
		pumpSchedTop = adcScaler;
		pumpRequests &= ~(1 << pumpReq);
		pumpSchedCtr = 0;
		pumpRunCtr = 0;
	}
	else
	{
		//Knob has not moved enough, do nothing
	}
}
void pumpScheduler()
{
	//Pump scheduler, run every 1s
	if (pumpSchedCtr < pumpSchedTop)
	{
		pumpSchedCtr++;
	}
	else
	{
		//Timer overflow, run spray cycle
		if (pumpRunCtr <= sprayTimeSec)
		{
			pumpRunCtr++;
			pumpRequests |= (1 << pumpReq);
		}
		else
		{
			pumpRequests &= ~(1 << pumpReq);
			pumpSchedCtr = 0;
			pumpRunCtr = 0;
		}
	}
}
void controlPump()
{
	//Control pump based on global variables
	if ((pumpRequests & (1 << pumpOvr)) || (pumpRequests & (1 << pumpReq)))
	{
		Pump_ON;
	}
	else
	{
		Pump_OFF;
	}
}

int main(void)
{
	Init_IO();
	Init_Timer();
	Init_ADC();
	sei();
	while (1) 
	{
		//Scheduler
		if (t10mSecCtr)
		{
			//Do 10msec tasks and reset scheduler
			controlPump();
			//Reset scheduler tick
			t10mSecCtr = 0;
		}//if (t10mSecCtr)
		
		if (t100mSecCtr)
		{
			//Do 100msec tasks and reset scheduler
			checkTimerKnob();
			//Turn pump on with button
			if (readButton1)
			{
				pumpRequests |= (1 << pumpOvr);
			}
			else pumpRequests &= ~(1 << pumpOvr);
			
			//Reset scheduler tick
			t100mSecCtr = 0;
		} //if (t100mSecCtr)
		
		if (t500mSecCtr)
		{
			//Do 500msec tasks and reset scheduler
			t500mSecCtr = 0;
		}//if (t500mSecCtr)
		
		if (t1SecCtr)
		{
			//Do 1sec tasks and reset scheduler
			pumpScheduler();
			t1SecCtr = 0;
		}//if (t1SecCtr)
	}
}

ISR(TIMER1_OVF_vect){
	//Executes every 1ms
	
	//Scheduler
	if (Timer10msOld < 10)
	{
		Timer10msOld++;
	}
	else
	{
		t10mSecCtr = 1;
		Timer10msOld = 0;
	}
	
	if (Timer100msOld < 100)
	{
		Timer100msOld++;
	}
	else
	{
		t100mSecCtr = 1;
		Timer100msOld = 0;
	}
	
	if (Timer500msOld < 500)
	{
		Timer500msOld++;
	}
	else
	{
		t500mSecCtr = 1;
		Timer500msOld = 0;
	}
	
	if (Timer1sOld < 1000)
	{
		Timer1sOld++;
	}
	else
	{
		t1SecCtr = 1;
		Timer1sOld = 0;
	}
}