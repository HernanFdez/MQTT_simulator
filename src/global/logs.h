#include <bits/stdc++.h>




struct connection
{
	int address=0;
	int status_code=0;
	long int time_stamp=0;
	int key=0;
	string description="";
	long int last_ping;
};



struct subscription
{
	string topic;
	int address;
	int status_code=0;
	long int time_stamp=0;
	string description="";
};

struct entry
{
	string topic;
	double val;
};
