/********************************************
 *
 *  Name: Byung Do Lee
 *  Section: W: 3:30 - 4:50
 *  Final Project: Speedtrap
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "serial.h"

void serial_init(unsigned short ubrr_value)
{
	// Enable serial interface data transmission
    UBRR0 = ubrr_value;     
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
    UCSR0B |= (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);

    // enable tri-state
    DDRC |= (1 << 5);
    PORTC &= ~(1 << 5);
}

void serial_txchar(char ch)
{
    while ((UCSR0A & (1 << UDRE0)) == 0) {}
    UDR0 = ch;
}

void serial_stringout(int speed_return)
{
    int i = 0;
    char out[10];
    snprintf(out, 10, "@%d$", speed_return); // try pointer the in speed_return and comment out snprintf

    // Call serial_txchar in loop to send numbers
   while(out[i] != '\0'){
    serial_txchar(out[i]);
    i++;
    }
}

ISR(USART_RX_vect)
{
	char ch;
	ch = UDR0;
	if (ch == '@')
	{
		rx_start = 1;
		num = 0;
		received = 0;
	}
	else if (rx_start == 1)
	{
		if (ch <= '9' && ch >= '0' && num < 4)
		{
			received_string[num] = ch;
			num++;
		}
		else if (ch == '$' && num > 0)
		{
			received_string[num] = '\0';
			received = 1;
			rx_start = 0;
		}
		else	// If no valid input, reset transmitted data
		{
			rx_start = 0;
		}
	}
}