/**
 * STLQueue.cpp
 *
 */

#include "STLQueue.h"

//constructor
STLQueue::STLQueue(int max_wind_size)
{
	this->max_wind_size = max_wind_size;
}

/**
 * PURPOSE:	Push the sequence number and the data.
 */
void STLQueue::push(uint32_t seq_num, int32_t data){

	if((int)sequence_num.size() < max_wind_size){
		sequence_num.push(seq_num);
		data_content.push(data);
	}
}

/**
 * PURPOSE:	Pop the "oldest" item in the queue.
 */
void STLQueue::pop(){
	sequence_num.pop();
	data_content.pop();
}	

/**
 * PURPOSE:	Get the "oldest" sequence number in the queue.
 */
uint32_t STLQueue::get_first_seq(){
	return sequence_num.front();
}


/**
 * PURPOSE:	Get the "newest" sequence number in the queue.
 */
uint32_t STLQueue::get_last_seq(){
	return sequence_num.back();
}


/**
 * PURPOSE:	Get the "oldest" data in the queue.
 */
int32_t STLQueue::get_first_data(){
	return data_content.front();
}


/**
 * PURPOSE:	Get the size of the queue.
 */
int STLQueue::get_size(){
	return sequence_num.size();
}








