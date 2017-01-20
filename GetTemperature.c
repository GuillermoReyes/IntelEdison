#include <stdio.h>
#include <mraa/aio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



float convert_decoding(float S){
//function to 
	int B = 4275; 
	float R = 1023.0/((float)S)-1.0;
	R = 100000*R; 
	float temp = 1.0/(log(R/100000.0)/B +1/298.15)-273.15;
	return temp; 

}
void get_time(char* temp){
//function to get the temp
	time_t current_time;
	struct tm* time_info;
	char timeString[9];
	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString,sizeof(timeString),"%H:%M:%S",time_info);
	//printf("%s",timeString);
	// 
	
       		
}

int main() {
	 
	mraa_aio_context adcPin;
       	float adcValue; 
	mraa_init();
	adcPin = mraa_aio_init(0);
	adcValue = mraa_aio_read(adcPin);	
	char time_string[9];
	time_t current_time;
       	struct tm* time_info;
	time(&current_time);
	time_info = localtime(&current_time);

	strftime(time_string,sizeof(time_string),"%H:%M:%S",time_info);	
	//get_time(time_string);
	printf("%s %0.1f\n",time_string, convert_decoding(adcValue)*9.0/5.0+32.0);


	exit(0); 	

}
