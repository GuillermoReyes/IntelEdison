#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mraa/aio.h>
#include <sys/time.h>
#include <sys/select.h>
int freq = 3;
int cent_flag = 0;
int run_flag = 1;
int id;
int sleep_flag = 0; 

float convert_decoding(float S){
	int B = 4275;
	float R = 1023.0/((float)S) -1.0;
	R = 100000.0*R;
	float temp = 1.0/(log(R/100000.0)/B +1/298.15)-273.15;
	return temp;

}
int process_cmd(char* cmd){
	if(strcmp(cmd,"OFF")==0)
		run_flag=0;
	
	else if(strcmp(cmd,"START")==0)
		sleep_flag=0;
	else if(strcmp(cmd,"STOP")==0)
		sleep_flag = 1;
	else if(strcmp(cmd,"SCALE=C")==0)
		cent_flag=1;
	else if(strcmp(cmd,"SCALE=F")==0)
		cent_flag=0;
	else if(strncmp(cmd,"FREQ=",5)==0){
		//printf("%s\n",cmd+5);
		freq = atoi(cmd+5);
		//printf("%d\n",freq);	
		return 1;
	}

	//TODO:OFF command, completely stop recording temperature
	//TODO: STOP command, dormant mode
	//TODO: START, start taking commnds
	//TODO: SCALE=[F/C]
	//TODO: FREQ=freq"
	//TODO: DISP=[Y/N]
	
	return 1;
	 


}
int main(){
	mraa_aio_context adcPin; 
	float adcValue;
       	mraa_init();	
	adcPin = mraa_aio_init(0); //temperature pin
	
	//server code 
	
	struct hostent* serv_info = gethostbyname("lever.cs.ucla.edu");
	struct in_addr* serv_ip = serv_info->h_addr_list[0];
	
	int client_sock = socket(PF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in serv_add;
	serv_add.sin_family = AF_INET; 
	serv_add.sin_port = htons(16000);
	serv_add.sin_addr = *serv_ip;
	
	int status = connect(client_sock,(struct sockaddr*)& serv_add,sizeof(serv_add));
	if (status < 0){
		fprintf(stderr, "error in connecting to lever.cs.ucla.edu\n");


	}
	char *msg  = "Port request 404671456";
	char resp[1024];
	int buf;;
	char msg_buffer[1024];
	int count = send(client_sock, msg,strlen(msg),0);
	count = read(client_sock,&buf,4);
	close(client_sock);

	client_sock = socket(PF_INET,SOCK_STREAM,0);
	 
	int new_sock = buf;
	//connect to new port
	serv_add.sin_port = htons(new_sock);
	status  = connect(client_sock,(struct sockaddr*)& serv_add,sizeof(serv_add));
	if(status<0){
		printf("you goofed up\n");
	}
	//printf("%d\n",new_sock);
	//set frequency - default is 3
	fd_set sel;  
	struct timeval waittime;
	waittime.tv_sec = 3;
	waittime.tv_usec = 0;
	
	//loop 
	while(run_flag){
		if(!sleep_flag){
			waittime.tv_sec = freq;
			
			adcValue = mraa_aio_read(adcPin);
			char time_string[9];
			time_t current_time;
			struct tm* time_info;
			time(&current_time);
			//TODO: change time zone somehow
			time_info = localtime(&current_time);				
			strftime(time_string,sizeof(time_string),"%H:%M:%S",time_info);
			//printf("%s %0.1f\n",time_string,convert_decoding(adcValue));
			if(cent_flag){
				sprintf(msg_buffer,"%s %0.1f C\n",time_string,convert_decoding(adcValue));
			}
			if(cent_flag==0){

				sprintf(msg_buffer,"%s %0.1f F\n",time_string,convert_decoding(adcValue)*9.0/5.0 + 32.0);
			}
			printf("%s", msg_buffer);
			//bzero(resp,1024);
			//recv(client_sock,resp,1024,0);
			//printf("cmd %s\n",resp);		
			int count = 0; 
			count += send(client_sock, msg_buffer,strlen(msg_buffer),0);
			FD_ZERO(&sel);
			FD_SET(client_sock,&sel);
			status = select(client_sock+1,&sel,NULL,NULL,&waittime);			if(status < 0)
				printf("error in selecting\n");
		}
		//then process comands
			if(FD_ISSET(client_sock,&sel)){

				bzero(resp,1024);
				recv(client_sock,resp,1024,0);
				printf("command %s\n",resp);
				if(!process_cmd(resp)){
					exit(0);
				}

	
			}
		




	}//end while	
	exit(0);

}
