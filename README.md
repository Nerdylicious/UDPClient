Instructions:

Need the files a3client.c, UpperLayer.cpp, UpperLayer.h, STLQueue.cpp, STLQueue.h in same directory.

To compile:	
    g++ -Wall a3client.c UpperLayer.cpp STLQueue.cpp -o prog

To run: 
    ./prog <ip_address_receiver> <port_num_receiver> <send_window_size> <timer_duration> <freq_data_upperlayer>

Note: send_window_size is in segments, timer_duration is in seconds, freq_data_upperlayer in seconds

Example:	
    ./prog 127.0.0.1 10000 5 10 3

Here window size is 5 segments, timer duration is 10 seconds, frequency to get from upperlayer is 3 seconds
