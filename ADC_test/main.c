/*
 * ADC_test.c
 *
 * Created: 6/11/2021 12:38:28
 * Author : blue
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
unsigned char results[10];
unsigned char erase = 0;

int main(void)
{
	// -- USART0 SETUP --//
	//	set RX(PD0) as input, TX(PD1) as output and high
	DDRD &= ~(1 << DDRD0);
	DDRD |= (1 << DDRD1);
	PORTD |= (1 << PORTD1);
	//	normal speed
	UCSR0A &= ~(1 << U2X0);
	//	set baud rate to 9600 (23 for the register)
	UBRR0 = 23;
	//	mode select: asynchronous USART
	UCSR0C &= ~((1 << UMSEL00) | (1 << UMSEL01));
	//	No parity
	UCSR0C &= ~((1 << UPM00) | (1 << UPM01));
	//	1 stop bit
	UCSR0C &= ~(1 << USBS0);
	//	8 databits
	UCSR0B &= ~(1 << UCSZ02);
	UCSR0C |=((1 << UCSZ00) | (1 << UCSZ01));
	//	we want to trigger an interrupt when the recv buffer is full
	UCSR0B |= (1 << RXCIE0);
	//	enable receive and transmit port
	UCSR0B |= ((1 << RXEN0) | (1 << TXEN0));
	
	/*
	// -- TIMER0 SETUP --//
	//	timer0 -> CTC mode
	TCCR0A |= (1 << WGM01);
	TCCR0A &= ~(1 << WGM00);
	TCCR0B &= ~(1 << WGM02);
	//	prescaler at 1024
	TCCR0B |= ((1 << CS00) | (1 << CS02));
	TCCR0B &= ~(1 << CS01);
	//	compare match every 50ms
	OCR0A = 199;
	*/
	
	//	-- ADC SETUP -- //
    //	use AVCC external voltage (VCC will be attached to it)
	ADMUX &= ~((1 << REFS1));
	ADMUX |= (1 << REFS0);
	//	Left-adjusted result
	ADMUX |= (1 << ADLAR);
	//	use single ended input ADC0
	ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3) | (1 << MUX4));
	//	Prescaler for the speed of the conversion. 
	ADCSRA |= ((1 << ADPS0) | (1 << ADPS2));
	ADCSRA &= ~(1 << ADPS1);
	//	enable freerunning
	ADCSRA &= ~(1 << ADATE);
	//	disable the input buffer on the pin we want to use for ADC (pina0)
	DIDR0 |= (1 << ADC0D);
	//	enable adc
	ADCSRA |= (1 << ADEN);


	UDR0 = 1;

	//	enable GIE
	sei();
	
	while (1) 
    {
		if(ADCSRA & (1 << ADSC)){
			while(ADCSRA & (1 << ADSC));
			while(!(UCSR0A & (1 << UDRE0)));
			UDR0 = ADCH;
			
		}
		
    }
}



ISR(USART0_RX_vect){
	
	erase = UDR0;
	ADCSRA |= (1 << ADSC);
}

