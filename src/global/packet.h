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

	int payload_code() 
	{
		int prod=1; 
		for(auto& elem : payload) for(int i=0; i<elem.length(); i++) prod = (prod * (int) elem[i]) % 1000;

		return(prod); 
	}

	bool authentic() { return(payload_code() == signature); }

	void sign() { signature = payload_code(); } 
};


void send_packet(packet pack, int key, bool signing = true)
{
	ofstream out("network", ios_base::app);

	pack.time_stamp=time(0);
	if(signing) pack.sign();	

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

	if(packet_str != "-")
	{
		// parameters must be ordered as in send_packet !
		iss >> pack.time_stamp
			>> pack.sender_address
			>> pack.receiver_address
			>> pack.type
			>> payload_str
			>> pack.signature
			;
		pack.payload.push_back(payload_str);
	}

//	cout<<"sender_address = "<<pack.sender_address<<endl;

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