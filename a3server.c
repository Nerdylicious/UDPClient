/*A Test receiver server for assignment 3*/
/*The server needs to be restarted for each execution of the sender*/
/*If the server receives a segment with the seq # it is expected, then it will delay (between 1 and 3) seconds for sending acknowledge with 80% success probability.

If a segment arrives with a seq # that the receiver is not expecting, then it simplies acknowledges the last acknowledge seq #
*/

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<time.h>

#define SERV_PORT 10000
#define MAX_SIZE 256

struct SegmentMsg
{
	uint32_t seqNo;
	uint32_t dataLength;	
	int32_t data;
};


void printSeg(struct SegmentMsg *ptrSeg)
{
	fprintf(stderr,"Seq #:%u\n",ptrSeg->seqNo);
	fprintf(stderr,"Data length:%u\n",ptrSeg->dataLength);
	fprintf(stderr,"Data:%d\n",ptrSeg->data);
}


int main(int argc, char **argv)
{
    int sockFd;
	long int r;
    struct sockaddr_in servAddr, clientAddr;
    socklen_t len;
    int n;
    char data[MAX_SIZE];
	struct SegmentMsg *ptrSeg;
	uint32_t ackNo;
	time_t tReceived, tSent;
	int expectedSeqNum = 0;
	uint32_t tempAckNo;

	if (argc !=2)
	{
		fprintf(stderr,"Usage a3server port\n");
		exit(EXIT_FAILURE);
	}
	
    if ((sockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error in socket():");
        exit(EXIT_FAILURE);
    }


    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(sockFd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    {
        perror("Error in bind():");
        exit(EXIT_FAILURE);
    }

	srandom(1);

    for (;;)
    {
        len = sizeof(clientAddr);
		n = recvfrom(sockFd, data, MAX_SIZE-1, 0, (struct sockaddr*)&clientAddr, &len);
		tReceived = time(NULL);
		fprintf(stderr,"Packet received at:%s",ctime(&tReceived));
        if (n > 0)
        {

			ptrSeg	= (struct SegmentMsg*)data;
			ackNo = ptrSeg->seqNo;
		
			//fprintf(stderr,"ackNo = %u\n",ackNo);
			ptrSeg->seqNo = ntohl(ptrSeg->seqNo);
			ptrSeg->dataLength = ntohl(ptrSeg->dataLength);
			ptrSeg->data = ntohl(ptrSeg->data);
			printSeg(ptrSeg);
			sleep((random()%3)+1);
			tSent = time(NULL);
			if (ptrSeg->seqNo == expectedSeqNum) //seq # received matches what receiver is expecting
			{
				r = random();
				//with probability 1/5, don't send ack back to sender.  This simulates a lost ack
//!!!!!!!!!!!!!!!!MODIFICATION MADE HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				if (r  !=0)	//send ack for this segment	
				{
					fprintf(stderr,"Acknowledgment # %u SENT\n",ptrSeg->seqNo);
					fprintf(stderr,"Acknowledgment sent:%s\n",ctime(&tSent));	
			        sendto(sockFd, &ackNo ,sizeof(uint32_t),0, (struct sockaddr*)&clientAddr, len);
					expectedSeqNum++;
				}
				else  //don't ack!
				{
					fprintf(stderr,"Packet with seq # %u DROPPED\n\n",ptrSeg->seqNo);
				}
			}
			else
			{
					//send ack of latest acked seq #.
					if (expectedSeqNum > 0)
					{
						tempAckNo = expectedSeqNum - 1;
						fprintf(stderr,"(Duplicate) Acknowledgment # %u SENT\n",tempAckNo);
						fprintf(stderr,"Acknowledgment sent:%s\n",ctime(&tSent));	
						tempAckNo = htonl(tempAckNo);
					    sendto(sockFd, &tempAckNo ,sizeof(uint32_t),0, (struct sockaddr*)&clientAddr, len);
					}
			}
        }

    }
    return 0;

}
