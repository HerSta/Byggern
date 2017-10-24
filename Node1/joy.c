#define F_CPU 4915200

#include <avr/io.h>
#include <util/delay.h>

#include "joy.h"
#include "adc.h"

int x;
int y;
int z;
int xmean;
int ymean;
int xmax = 0;
int xmin = 255;
int ymax = 0;
int ymin = 255;

void joy_init(){
	clear_bit(DDRB,PB2);	//Set joystick button pin to input
	set_bit(PORTB,PB2);		//Set the internal pull-up resistor needed for the button
	printf("Calibrating...\r\n");
	joy_calibrate();
	printf("Calibrated.\r\n");
}

void joy_calibrate(){
	xmean = adc_read('x');
	ymean = adc_read('y');
	while (test_bit(PINB, PB2)) //perform until joystick is pressed
	{
		x = adc_read('x');
		y = adc_read('y');
		if (x > xmax){
			xmax = x;
			printf("%d \r\n", xmax);
		}
		if (y > ymax){
			ymax = y;
			printf("%d \r\n", ymax);
		}
		if (x < xmin){
			xmin = x;
			printf("%d \r\n", xmin);
		}
		if (y < ymin){
			ymin = y;
			printf("%d \r\n", ymin);
		}
	}
}

Position joy_get_position(){
		x = adc_read('x');
		y = adc_read('y');
		z = !test_bit(PINB, PB2);
		
		if (x > xmean){
			x = ((x - xmean) * 100) / (xmax - xmean);
		}
		else{
			x = ((x - xmean) * 100) / (xmean - xmin);
		}
		if (y > ymean){
			y = ((y - ymean) * 100) / (ymax - ymean);
		}
		else{
			y = ((y - ymean)  * 100) / (ymean - ymin);
		}
		
		Position pos;
		pos.x = x;
		pos.y = y;
		pos.z = z;
		return pos;
}

Direction joy_get_direction(){ 
	Position_polar pos_pol  = joy_get_position_polar();
	
	if(pos_pol.amplitude > 80){
		if (pos_pol.angle > 135){
			return LEFT;
		}
		else if(pos_pol.angle > 45){
			return UP;
		}
		else if(pos_pol.angle > -45){
			return RIGHT;
		}
		else if(pos_pol.angle > -135){
			return DOWN;
		}
		else{
			return LEFT;
		}
	}
	return NEUTRAL;
} 

Position_polar joy_get_position_polar(){
	Position pos_cart = joy_get_position();
	Position_polar pos_pol;
	pos_pol.angle = atan2(pos_cart.y,pos_cart.x) * (180.0/ M_PI);
	pos_pol.amplitude = sqrt((uint16_t)(pos_cart.y*pos_cart.y) + (uint16_t)(pos_cart.x*pos_cart.x));
	return pos_pol; 
}

void joy_print(int all){
	if(all){
		printf("x = ");
		printf("%d  ", adc_read('x'));
		
		printf("y = ");
		printf("%d  ", adc_read('y'));
	}
	
	Position pos = joy_get_position();
	Position_polar pos_pol = joy_get_position_polar();
	
	printf("X = ");
	printf("%d  ", pos.x);
	
	printf("Y = ");
	printf("%d  ", pos.y);
	
	printf("A = ");
	printf("%d  ", pos_pol.amplitude);
	
	printf("theta = ");
	printf("%d  ", pos_pol.angle);
	
	printf("Direction = ");
	printf("%d  ", joy_get_direction());
	
	printf("Middle button= ");
	printf("%d  ", pos.z);
	
}