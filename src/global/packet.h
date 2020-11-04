#include <bits/stdc++.h>

#include "encryption.h"

using namespace std;


struct packet
{
	long int time_stamp = 0;
	int sender_address = 0;
	int receiver_address = 0;
	string type = "-";
	vector<string> payload;
	int signature = 0;

	void decrypt(int key) {payload = read_payload(payload[0], key);}
};


void send_packet(packet pack, int key)
{
	ofstream out("network", ios_base::app);

	pack.time_stamp=time(0);	

	// parameters must be ordered as in read_packet !
	out << pack.time_stamp << "\t"
		<< pack.sender_address << "\t"
		<< pack.receiver_address << "\t"
		<< pack.type << "\t"
		<< write_payload(pack.payload, key) << "\t"
		<< pack.signature << "\t"
		<< endl;
}


packet read_packet(string packet_str)
{
	istringstream iss(packet_str);
	packet pack;
	string payload_str;

	// parameters must be ordered as in send_packet !
	iss >> pack.time_stamp
		>> pack.sender_address
		>> pack.receiver_address
		>> pack.type
		>> payload_str
		>> pack.signature
		;

	pack.payload.push_back(payload_str);
	return pack;
}


vector<packet> read_buffer()
{
    static int start = 0;

	vector<packet> new_packets;

    ifstream in("network"); 
 	if(in) 
 	{
		in.seekg(0, in.end);
		int end = in.tellg();  
		in.seekg(start, in.beg); 
		start = end;

		string packet_str;
		while(getline(in, packet_str)) new_packets.push_back(read_packet(packet_str));
 	}

    return new_packets;
}