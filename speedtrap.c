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

#include "speedtrap.h"
#include "lcd.h"
#include "encoder.h"
#include "serial.h"


int main(void) { 

	DDRC |= (1 << 1) | (1 << 4); // Initialize outputs for the buzzer and the LED
	PORTC |= (1<<1) | (1<<4); // Pull-up resistor on the LED and the buzzer
	PORTC &= ~(1<<4); // LED initially off

	PCICR |= (1 << PCIE2);		 // Enable interrupt on photoresistor sensor (PIND)
    PCMSK2 |= (1 << PCINT19) | (1 << PCINT18); //Enable the interrupt for (D3 & D2) for photoresistor sensors

    flag = 0;
    state = 0;

    // Call the necessary functions
    lcd_init(); 
    timer1_init();
    encoder_init();
    serial_init(MYUBRR);
    sei(); // Enable global interrupt

    // Splash screen
    lcd_writecommand(1);


    unsigned char stored_speed = eeprom_read_byte((void *) 100);	// Store the value of max_speed and read from EEPROM address 100
	if (stored_speed >= 1 && stored_speed <= 99)
	{
		max_speed = stored_speed;
	}

	lcd_moveto(1,0);
	snprintf(MAX_speed_disp, 8, "MAX=%2d", max_speed);
	lcd_stringout(MAX_speed_disp); // Display the max_speed set by the rotary encoder 

    
    while (1) {                 // Loop forever

    	if(speed_change){	// When max_speed is changed by the rotary encoder
    		eeprom_update_byte((void *) 100, max_speed);	// Update max_speed when it's changed
			lcd_moveto(1,0);
			snprintf(MAX_speed_disp, 8, "MAX=%2d", max_speed);
			lcd_stringout(MAX_speed_disp);
			speed_change = 0;
    	}
 		
 		if(flag){     // When flag is triggered. 1.8 in. between the photoresitors. 1.8 in. = 45.72 mm. Speed = 45.72/time (mm/ms) = 45720/time (mm/s)
 			lcd_moveto(0,0);
       		snprintf(output_1, 10, "%4d ms =", time);
			lcd_stringout(output_1);
 			lcd_moveto(0,9);
 			speed = 45720/time; // mm/s
 			snprintf(output_2, 10, "%3d.%d", speed/10, speed%10); // Format the speed to display in cm/s
 			lcd_stringout(output_2); // Display the speed in cm/s
 			serial_stringout(speed); // Transmit the measured speed string
 			flag = 0;
 		}

 		if(received)	// When speed is received
		{
			int rx_speed;
			sscanf(received_string, "%d", &rx_speed); // convert received string to a int value. 
			lcd_moveto(1,9);
			snprintf(output_3, 10, "%3d.%d", rx_speed/10, rx_speed%10); 
			lcd_stringout(output_3);	// Display received speed
			if (rx_speed > (max_speed*10))	// Compare the received speed with maxspeed
			{
				play_note(253); // plays the buzzer if received speed greater than max_speed
			}
			num = 0;	// Reset the number of characters
			received = 0;	// Clear serial flag
		}
    }

    return 0;
}


/* 
  Interrupt to check the photoresistor sensor
*/
ISR(PCINT2_vect){

	a = PIND & (1<<3); // Initializing the starting photoresistor
    b = PIND & (1<<2); // Initializing the ending photoresistor
 

	 	if(!a){ // if the starting photoresistor blocked
	 		time = 0;
	 		PORTC |= (1<<4); // LED on
	 		state = 1; // Turn on Timer flag
	 	}

    
       else if(!b){ // if the ending photoresistor blocked
            PORTC &= ~(1<<4); // LED off
			flag = 1; // Turn on time and speed display flag
			state = 0; // Turn off Timer flag
        } 
}

/* 
  Set Timer interrupt
*/
ISR(TIMER1_COMPA_vect){

	if(state == 1){
		time++; // Counting in ms
	}

}

/*
  Initialize the Timer registers
*/
void timer1_init(void){
	
	TCCR1B &= 0xe0;      //clear bits
	TCCR1B |= (1<<WGM12); // Set to CTC mode
	TIMSK1 |= (1<<OCIE1A); // Enable Timer Interrupt
	OCR1A = 2000; // Load the MAX count
	TCCR1B |= (1<<CS11); // Prescalar = 8
}

/*
  Play a tone at the frequency specified for one second
*/
void play_note(unsigned short freq)
{
    unsigned long period;

    period = 1000000 / freq;      // Period of note in microseconds

    while (freq--) {
    // Make PB4 high
        PORTC |= (1 << 1);
    // Use variable_delay_us to delay for half the period
        variable_delay_us(period/2);
    // Make PB4 low
        PORTC &= ~(1 << 1);
    // Delay for half the period again
        variable_delay_us(period/2);
    }
}

/*
    variable_delay_us - Delay a variable number of microseconds
*/
void variable_delay_us(int delay){

    int i = (delay + 5) / 10;

    while (i--)
        _delay_us(10);
}

/* Implementing the buzzer in the second timer interrupt 
didn't work properly and ran out of time... Have mercy. 
Thanks to the TAS for their helpful tips and maintaining the patience 
to help me throughout the semester. Have a good winter break!
*/
