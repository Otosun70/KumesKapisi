/*
 * system.c
 *
 * Created: 19.07.2019 15:29:36
 *  Author: tosun_n
 */ 
#include "include.h"

void sistem_init()
{
	devre_init();
	motor2_init();
	motor1_init();
	ADC_init();
	switch_Acik_Kapali_init();
	led_init();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
// 	timer1_PWM_init();
	timer0_init();
	optik_Encoder_init();
	optik_kapi_emniyeti_init();
	powerSaveControl();

	sei();
	
}

void powerSaveControl()
{
	ACSR |= (1<<7);//Anolog Comparator disable yap�ld�.
	MCUCR |=(1<<2);//ADC noise reduction enable yap�ld�.
	MCUCR |=(1<<5);//Sleep enable yap�ld�.
}

void devre_init()
{
	isikSeviyesi=0;
	sayacGece=0;
	sayacGunduz=0;
	sayac_motorAdim=0;
	gunduzDurumu=true;
	calismaModu='O';
	optik_kapi_emniyeti_disable();
}

void timer0_init()
{
	TCCR0 |=(1<<CS02)|(1<<CS00);
	TIMSK  |= (1<<TOIE0);
}

void timer1_PWM_init()	//Fast PWM 8bit
{
	TCCR1A |= (1<<COM1A1)|(1<<WGM10);
	TCCR1B |= (1<<WGM12)|(1<<CS10);
}

void timer2_init()
{
	TCCR2 |=(1<<CS22)|(1<<CS20);
	TIMSK  |= (1<<TOIE2);
}

ISR(TIMER0_OVF_vect)
{
	sayactimer0++;
	if (sayactimer0>ZAMANLI_ISLEMLER_SURESI)
	{
		sayactimer0=0;
		if (durum0)
		{
			durum0=false;
		} 
		else
		{
			durum0=true;
		}
	}
	if (sayacTest>0)
	{
		sayacTest--;
	}
 
}

