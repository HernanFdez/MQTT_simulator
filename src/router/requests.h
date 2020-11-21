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


int num_digits = 4;


string to_url_form(string str)
{
	string form="";
    for(auto& c : str) 
    {
    	string elem = to_string((int) c);
    	while(elem.length()<num_digits) elem = "0" + elem;
    	form += elem; 
    }  

    return form;
}

string from_url_form(string str)
{
	string form="";
	istringstream iss(str);
	char c;

    for(int i=0; i<str.length()/num_digits; i++)
    {
    	string elem;
    	for(int i=0; i<num_digits; i++) {iss >> c; elem+=c;}
    	form += char(stoi(elem));
    }  

    return form;
}

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

	else { cout<<"Server sent error status code ("<<response.statusCode()<<"), with description: "<<response.statusDescription()<<endl; }
	return "-";
}




void post_packet(packet pack)
{
//	cout<<"preencoded = "<<pack.payload[0]<<endl; cout<<"   encoded = "<<to_url_form(pack.payload[0])<<endl;

	vector<UrlRequest::GetParameter> params = 	
	{
	    {"ts", pack.time_stamp},
	    {"sa", to_string(pack.sender_address)},
	    {"ra", to_string(pack.receiver_address)},
	    {"type", pack.type},
	    {"payload", to_url_form(pack.payload[0])},
	    {"sig", pack.signature}
	};	

	URLRead(cloud, in_dir, params);
}

packet get_packet(int net)
{
	vector<UrlRequest::GetParameter> params = { {"net", to_string(net)}	};	

	string packet_str = URLRead(cloud, out_dir, params);

	packet pack = read_packet(packet_str);

	if(pack.time_stamp)
	{
//		cout<<"predecoded = "<<pack.payload[0]<<endl;	cout<<"   decoded = "<<from_url_form(pack.payload[0])<<endl;

		pack.payload[0] = from_url_form(pack.payload[0]);
		pack.decrypt(0);
	}

	return pack;
}


