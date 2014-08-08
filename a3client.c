/**
 * a3client.c
 *
 * PURPOSE: 	A half-duplex sender (UDP client) adhering to the reliable data transfer
 *			protocol.
 *
 * Need the files a3client.c, UpperLayer.cpp, UpperLayer.h, STLQueue.cpp, STLQueue.h in same directory.
 * To compile: g++ -Wall a3client.c UpperLayer.cpp STLQueue.cpp -o prog
 * Type in the command line: ./prog <ip_address_receiver> <port_num_receiver> <send_window_size> <timer_duration> <freq_data_upperlayer>
 *
 * More information in readme.txt
 *
 */


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "UpperLayer.h"
#include "STLQueue.h"

#define SEGMENT_SIZE 12
#define MAX_SIZE 256


enum BOOLEAN{
	FALSE = 0,
	TRUE = 1
};

typedef enum BOOLEAN boolean;

struct SegmentMsg
{
	uint32_t seqNo;
	uint32_t dataLength;	
	int32_t data;
};

//had no choice other than to use global variables because timeout
//does not accept any arguments

//STATE = 0 means get the data from the application and send the segment
//STATE = 1 means retransmit previously transmitted data segment with smallest ack number
int STATE = 0;
boolean is_timer_running = FALSE;
int timer_duration = 0;


/**
 * PURPOSE:	The handler to handle the signal
 *			This handler doesn't seem to work if I don't put int sig as parameter
 */
void timeout(int sig){

	time_t tTimeout;
	tTimeout = time(NULL);
	fprintf(stderr,"\nTimeout occured at %s", ctime(&tTimeout));
	is_timer_running = FALSE;
	STATE = 1;
}

/**
 * PURPOSE:	Starts and restarts the alarm using timer_duration
 *			specified by user
 */
void start_timer(){

	is_timer_running = TRUE;
	alarm(0);
	alarm(timer_duration);
}



int main(int argc, char *argv[]) {

	int sockfd;
	struct sockaddr_in address;
	int sequence_num = 0;
	int result;
	time_t tReceived, tRetrieved, tSent, tTimeout;

	
	if(argc != 6){
	
	   fprintf(stderr,"USAGE: a3client <receiver_ip_address> <receiver_port> <send_window_size> <timer_duration> <freq_data_upperlayer>\n");
	   exit(EXIT_FAILURE);
	}
	
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
	
	   perror("Error in socket():");
	   exit(EXIT_FAILURE);
	}
	
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &address.sin_addr);
	
		
	//do nonblocking reads
	int flags;
	if((flags = fcntl(sockfd, F_GETFL, 0)) < 0){
		perror("Error in fcntl():");
		exit(EXIT_FAILURE);
	}
	flags = flags | O_NONBLOCK;

	if(fcntl(sockfd, F_SETFL, flags) < 0){
		perror("Error in fcntl():");
		exit(EXIT_FAILURE);
	}

	//make an instance of the buffer to hold unacked segments
	int max_wind_size = atoi(argv[3]);
	STLQueue *window = new STLQueue(max_wind_size);
	
	timer_duration = atoi(argv[4]);

	//retrieve data from application
	int timer_upperlayer = atoi(argv[5]);
	UpperLayer *upperlayer = new UpperLayer(timer_upperlayer);
	
	struct SegmentMsg segment;
	int data_avail;
	int32_t retrieve_data;
	signal(SIGALRM, timeout);
	
	//infinite loop
	while(1){
	
		//retransmit previously transmitted data segment with smallest ack number
		if(STATE == 1){
			
			//create TCP segment
			segment.seqNo = htonl(window->get_first_seq());
			segment.dataLength = htonl(4);
			segment.data = htonl(window->get_first_data());
		
			fprintf(stderr, "\nSeq #: %d", ntohl(segment.seqNo));
			fprintf(stderr, "\nData length: %d", ntohl(segment.dataLength));
			fprintf(stderr, "\nData: %d", ntohl(segment.data));

			result = sendto(sockfd, &segment, SEGMENT_SIZE, 0, (struct sockaddr*)&address, sizeof(address));
			tSent = time(NULL);
			fprintf(stderr, "\nRetransmitting (seq # %d) sent at %s", ntohl(segment.seqNo), ctime(&tSent));
			
			//restart timer after retransmission
			tTimeout = time(NULL);
			fprintf(stderr, "\nRetransmit timer started (base segment) %s", ctime(&tTimeout));
			start_timer();
		
	
			STATE = 0;
		}

		//get data from application and send segment
		if(STATE == 0){
			//only retrieve data if window is not full
			//only send data if window is not full
			if(window->get_size() < max_wind_size){
			
				data_avail = upperlayer->isDataAvailable();

				//data is available, then retrieve the data
				if(data_avail == 1){
	
					//RETRIEVE IS MISPELLED IN retreiveData() IN UPPERLAYER.CPP
					retrieve_data = upperlayer->retreiveData();
					tRetrieved = time(NULL);
					fprintf(stderr, "\nData retrieved at %s", ctime(&tRetrieved));

					segment.seqNo = htonl(sequence_num);
					segment.dataLength = htonl(4);
					segment.data = htonl(retrieve_data);
			
					fprintf(stderr, "\nSeq #: %d", ntohl(segment.seqNo));
					fprintf(stderr, "\nData length: %d", ntohl(segment.dataLength));
					fprintf(stderr, "\nData: %d", ntohl(segment.data));

					sequence_num ++;
					
					//pass the segment to server
					result = sendto(sockfd, &segment, SEGMENT_SIZE, 0, (struct sockaddr*)&address, sizeof(address));
					tSent = time(NULL);
					fprintf(stderr, "\nData (seq # %d) sent at %s", ntohl(segment.seqNo), ctime(&tSent));
			
					//start timer for base segment if timer isn't on already
					if(is_timer_running == FALSE){
						start_timer();
						tTimeout = time(NULL);
						fprintf(stderr, "\nTimer started (base segment) %s", ctime(&tTimeout));
					}
					
					
					//put the sequence number of segment sent into the buffer
					window->push(ntohl(segment.seqNo), ntohl(segment.data));

				}
				//data is not available
				else if(data_avail == 0){
				
					//perror("Data from upperlayer not available");
				}
				
			}
		}
		
		
		//don't need state for receiving ACKs
		//receive acknowledgements
		uint32_t ackNo;
		int n = recvfrom(sockfd, &ackNo, sizeof(uint32_t), 0, NULL, NULL);
		ackNo = ntohl(ackNo);
	
		//if data is available
		if(n > 0){

			tReceived = time(NULL);
			fprintf(stderr,"\nAck # %d received at %s", ackNo, ctime(&tReceived));
			
			boolean was_ack_success = FALSE;
			//this while loop will do a cumulative ack (it adjusts the buffer cumulatively)
			while(window->get_size() > 0 && window->get_first_seq() <= ackNo){

				window->pop();
				was_ack_success = TRUE;
			}
			//restart timer if we got a new ack
			if(was_ack_success == TRUE){

				start_timer();
				tTimeout = time(NULL);
				fprintf(stderr, "\nRetransmit timer started (base segment) %s", ctime(&tTimeout));
			}

		}

	
	}//end infinite loop
	













	printf("\n\nProcessing complete\n\n");
  	exit(0);

}






