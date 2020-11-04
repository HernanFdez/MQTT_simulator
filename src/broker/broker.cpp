#include <bits/stdc++.h>

#include "broker.h"

#include "packet.h"
#include "logs.h"

using namespace std;


void broker::send(packet pack) 
{
	pack.sender_address=address; 
	int key = (connections.count(pack.receiver_address) && connections[pack.receiver_address].status_code!=0)? connections[pack.receiver_address].key : 0;
	send_packet(pack, key);
}


broker::broker() 
{	
	check_packets();

	srand(time(0)); 
	while(true)
	{
		address=rand()%999+1;
		
		if(!(active_brokers.count(address) || active_brokers.count(address) )) 
		{
			send(packet{.type="join", .payload={"broker"}});
			cout<<"*** BROKER SESSION STARTED AT ADDRESS "<<address<<" ***"<<endl<<endl; 
			break;
		}
	}
}
broker::~broker() { send(packet{.type="left", .payload={"broker"}}); }


void broker::check_packets()
{
	vector<packet> new_packets = read_buffer();

	for(auto& pack : new_packets) if(pack.receiver_address == address)
	{
		int client_address = pack.sender_address;
		int key = (connections.count(client_address) && connections[client_address].status_code==200)? connections[client_address].key : 0;

		pack.decrypt(key);
//		if(!pack.authentic()) continue;

		if(pack.type == "creq") read_creq(pack);
		if(pack.type == "sreq") read_sreq(pack);
		if(pack.type == "publ") read_publ(pack);

		if(pack.type == "join") read_join(pack);
		if(pack.type == "left") read_left(pack);
		if(pack.type == "ping") read_ping(pack);
		if(pack.type == "disc") read_disc(pack);
	}
}



void broker::read_creq(packet pack)
{
	int client_address = pack.sender_address;
	if(num_connections == max_connections) 	//Server band width limit exceeded
	{
		send_cres(pack.sender_address, 409); 
		return;
	}	

	{	//Status OK
		send_cres(pack.sender_address, 200); 
		connections[client_address].key = Diffie_Hellman(connections[client_address].key, stoi(pack.payload[0]));
		connections[client_address].status_code = 200;
		num_connections++;
		cout<<"Successfully connected to client "<<client_address<<" using key "<<connections[client_address].key<<endl<<endl;
	}
}

void broker::send_cres(int client_address, int status_code)
{
	packet pack = {.receiver_address=client_address, .type = "cres", .payload = {to_string(status_code)} };

	if(status_code==200) 
	{
		int pre_key = rand()%100+1;
		pack.payload.push_back( to_string(Diffie_Hellman(pre_key)) );
		connections[client_address] = connection {.address = client_address, .key = pre_key};
	}

	send(pack);
}



void broker::read_sreq(packet pack)
{
	int client_address = pack.sender_address;
	string topic = pack.payload[0];
	if(!connections.count(client_address)) 
	{
		send_sres(client_address, 401, topic);
		cout<<"subscription request from not connected client "<<pack.sender_address<<" ignored"<<endl<<endl;
		return;
	}

	if(count_if(subscriptions[topic].begin(), subscriptions[topic].end(), [=](subscription s) {return s.address==client_address;})) 
	{
		cout<<"resent subscription from client "<<client_address<<" ignored"<<endl<<endl;
		return;
	}

	{
		send_sres(client_address, 200, topic);
		subscriptions[topic].push_back(subscription {.topic = topic, .address = client_address, .status_code = 200, .time_stamp = time(0)});

		if(data.count(topic)) send_publ(entry {.topic = topic, .val = data[topic]}, client_address);
	}
}

void broker::send_sres(int client_address, int status_code, string topic)
{
	send(packet{.receiver_address = client_address, .type = "sres", .payload = {to_string(status_code), topic}});
}


void broker::read_publ(packet pack)
{
	entry pair = {.topic = pack.payload[0], .val = stod(pack.payload[1])};
	
	update(pair);
	if(listening) cout<<"Update: "<<pair.topic<<" -> "<<data[pair.topic]<<endl<<endl;
	for(auto& s : subscriptions[pair.topic]) send_publ(pair, s.address);
}

void broker::send_publ(entry pair, int client_address)
{
	if(!connections.count(client_address)) cout<<" Cannot publish to not connected to broker (" << client_address <<")" <<endl<<endl;

	else send(packet{.receiver_address = client_address, .type = "publ", .payload = {pair.topic, to_string(pair.val)} });
}

void broker::update(entry pair) {data[pair.topic] = pair.val;}



void broker::read_join(packet pack)
{
	if(pack.payload[0]=="broker") active_brokers[pack.sender_address] = pack.time_stamp;
	if(pack.payload[0]=="client") active_clients[pack.sender_address] = pack.time_stamp;
}

void broker::read_left(packet pack)
{
	if(pack.payload[0]=="broker") {active_brokers.erase(pack.sender_address); connections.erase(pack.sender_address);}
	if(pack.payload[0]=="client")  active_clients.erase(pack.sender_address);
}




void broker::check_pings() {long int t0=time(0); for(auto& it : connections) if(it.second.last_ping < t0-30) disconnect(it.second.address, true); }

void broker::send_ping(int add) { send(packet {.receiver_address = add, .type = "ping"}); }

void broker::send_ping()        { for(auto& it : connections) send_ping(it.first); }

void broker::read_ping(packet pack)
{
	int add = pack.sender_address;

	if(connections.count(add)) connections[add].last_ping = time(0);
	else cout<<"Ping from not connected device ("<< add <<") ignored" <<endl<<endl;
}


void broker::read_disc(packet pack) { disconnect(pack.sender_address); }

void broker::disconnect(int add, bool notify)
{
	if(!connections.count(add)) {cout<<"Cannot disconnect not conncted device ("<< add <<")"<<endl<<endl; return;}

	connections.erase(add);
	for(auto& it : subscriptions) remove_if(it.second.begin(), it.second.end(), [=](subscription s) {return s.address==add;} );

	if(notify) send(packet {.receiver_address = add, .type = "disc"});
}

