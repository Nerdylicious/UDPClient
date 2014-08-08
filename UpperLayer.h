
#include<time.h>
#include<sys/types.h>

class UpperLayer
{
private:
	time_t t1;
	int seconds;
	int avail;
public:	
	UpperLayer(int);
	int isDataAvailable();
	int32_t retreiveData();

};
