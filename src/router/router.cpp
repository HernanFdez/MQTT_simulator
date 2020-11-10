#include <bits/stdc++.h>

#include "router.h"
#include "requests.h"


using namespace std;


router:: router()
{
	srand(time(0)); 
	gate = rand()%999+1; 
	cout<<"*** ROUTER SESSION STARTED AT GATE "<<gate<<" ***"<<endl<<endl;
}
router::~router() {}



void router::check_uploads()
{
	vector<packet> new_packets = read_buffer();

	for(auto& pack : new_packets) if( pack.sender_address < 1000 && (pack.receiver_address > 1000 || pack.receiver_address == 0) )
	{
		pack.sender_address += gate*1000;
		post_packet(pack);
	}
}


void router::check_downloads()
{
	vector<packet> new_packets;

	while(true)
	{
		packet pack = get_packet(gate);
		if(pack.time_stamp == 0) break;
		new_packets.push_back(pack);
	}

	while(true)
	{
		packet pack = get_packet(0);
		if(pack.time_stamp == 0) break;
		new_packets.push_back(pack);
	}

	for(auto& pack : new_packets) 
	{
		pack.receiver_address = pack.receiver_address % 1000;
		send_packet(pack, 0);
	}
}



