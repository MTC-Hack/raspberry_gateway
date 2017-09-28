#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <termios.h> 
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "mqtt_funct.h"
#include "lora.h"

//#define DEBUG

float get_float_from_array(char * buf){
	float result;
	memcpy(&result, buf, 4);
	return result;
}

void setup() {
	mqtt_init();
	startConfig(3);
}


void loop() {
	char buf[50];
	int addr;
	if (get_data_from_end_device(buf, &addr)){
		mqtt_publish("mts_hardware_get_info_chanel", buf);
	}
	//mqtt_publish("mts_hardware_get_info_chanel", "fignya");
	fflush(stdout);
#ifdef DEBUG
	printf("end loop \n");
#endif
}

int main (int argc, char *argv[]){
	setup();
	while(1)
		loop();
	/*while(1){
		send_data_to_device("sdfsdf", 0);
	}*/
	return (0);
}
