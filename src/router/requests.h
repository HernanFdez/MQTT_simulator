#include <bits/stdc++.h>

#include "embeddedRest/UrlRequest.hpp"
#include "packet.h"

using namespace std;


string cloud  = "www.wolframcloud.com";
string user   = "hernan.fg96" ;
string input  = "MQTT/input";
string output = "MQTT/output";

string  in_dir = "/obj/" + user + "/" + input;
string out_dir = "/obj/" + user + "/" + output;


string URLRead(string URL, string dir, vector<UrlRequest::GetParameter> params)
{
	UrlRequest request;
	request.host(URL);
	request.uri(dir, params );
	request.addHeader("Content-Type: application/json");
	auto response = move(request.perform());

	if(response.statusCode() == 200)
	{
	  istringstream resp(response.body());
	  string body;
	  getline(resp, body); getline(resp, body);
	  return body;
	}

	else { cout<<"Master router sent error status code ("<<response.statusCode()<<"), with description: "<<response.statusDescription()<<endl; }
	return "-";
}

void post_packet(packet pack)
{
	string payload_str = "-";
	for(auto& elem : pack.payload) payload_str += elem + "-";

	vector<UrlRequest::GetParameter> params = 	
	{
	    {"ts", pack.time_stamp},
	    {"sa", to_string(pack.sender_address)},
	    {"ra", to_string(pack.receiver_address)},
	    {"type", pack.type},
	    {"payload", payload_str},
	    {"sig", pack.signature}
	};	

	URLRead(cloud, in_dir, params);
}

packet get_packet(int net)
{
	vector<UrlRequest::GetParameter> params = { {"net", to_string(net)}	};	

	string packet_str = URLRead(cloud, out_dir, params);

	return read_packet(packet_str);
}


