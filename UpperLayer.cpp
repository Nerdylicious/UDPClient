#include<stdlib.h>
#include"UpperLayer.h"

UpperLayer::UpperLayer(int sec)
{
	seconds = sec;
	t1 = time(NULL);
	avail = 0;
}
int UpperLayer::isDataAvailable()
{
	time_t t2;
	double elapsed;
	int retVal;

	t2 = time(NULL);
	elapsed = difftime(t2,t1);

	if (avail == 1)
		retVal = 1;
	else if (elapsed >= seconds)
	{
		avail = 1;
		retVal = 1;
	}
	else
	{
		retVal = 0;
	}
	return retVal;
}

int32_t UpperLayer::retreiveData()
{
	avail = 0;			//data read, no longer available
	t1 = time(NULL);	

	return random() % 1000;
}


