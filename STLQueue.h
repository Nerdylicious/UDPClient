/**
 * STLQueue.h
 *
 * PURPOSE: 	An STLQueue class. This class is used as the buffer that holds unacknowledged
 *			segments.
 *			It has 2 queues that correspond in parallel, one queue for sequence numbers and
 *			the other for data content.
 */

#include <queue>
#include <netinet/in.h>
using namespace std;

class STLQueue
{
private:
	int max_wind_size;
	queue <uint32_t> sequence_num;
	queue <int32_t> data_content;
	
public:	
	STLQueue(int max_wind_size);
	void push(uint32_t seq_num, int32_t data);
	void pop();
	uint32_t get_first_seq();
	uint32_t get_last_seq();
	int32_t get_first_data();
	int get_size();


};
