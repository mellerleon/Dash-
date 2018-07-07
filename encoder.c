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

#include "encoder.h"


int encoder_init(void) {

    PORTC |= ((1<<2) | (1<<3));  //Pull-up resistors for the rotatry encoder
    PCICR |= (1 << PCIE1);		 // Enable interrupt on PORTC
    PCMSK1 |= ((1 << PCINT10) | (1 << PCINT11)); // Enable the interrupt on PC2 & PC3

    speed_change = 0;

	return 0;  
}

//Interrupt function
ISR(PCINT1_vect){

    c = PINC;
    a = c & (1<<2);
    b = c & (1<<3);


	if (old_state == 0) { // Handle A and B inputs for state 0

		if(a){
			new_state = 1;
			max_speed++;
		}
		else if(b){
			new_state = 3;
			max_speed--;
		}
	}
		 
	else if (old_state == 1) { // Handle A and B inputs for state 1

	    if(!a){
			new_state = 0;
			max_speed--;
		}
		else if(b){
			new_state = 2;
			max_speed++;	
		}
	}

	else if (old_state == 2) { // Handle A and B inputs for state 2

	    if(!a){
			new_state = 3;
			max_speed++;	
		}
		else if(!b){
			new_state = 1;
			max_speed--;
		}
	}

	else {  // Handle A and B inputs for state 3

	    if(a){
			new_state = 2;
		    max_speed--;
		}
		else if(!b){
			new_state = 0;
			max_speed++;
		}
	}

	if(max_speed > 99){
		max_speed = 99;
	}
	else if(max_speed < 1){
		max_speed = 1;
	}
		old_state = new_state; // Update the current state
		speed_change = 1; //Turn on flag for speed change
}

