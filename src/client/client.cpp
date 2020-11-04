#include <bits/stdc++.h>

#include "client.h"

#include "packet.h"
#include "logs.h"

using namespace std;


void client::send(packet pack) 
{
	pack.sender_address=address; 
	int key = (connections.count(pack.receiver_address) && connections[pack.receiver_address].status_code!=0)? connections[pack.receiver_address].key : 0;
	send_packet(pack, key);
}


client::client() 
{	
	check_packets();

	srand(time(0)); 
	while(true)
	{
		address=rand()%999+1;
		
		if(!(active_brokers.count(address) || active_brokers.count(address) )) 
		{
			send(packet{.type="join", .payload={"client"}});
			cout<<"*** CLIENT SESSION STARTED AT ADDRESS "<<address<<" ***"<<endl<<endl; 
			break;
		}
	}
}
client::~client() {send(packet{.type="left", .payload={"client"}});}


void client::check_packets()
{
	vector<packet> new_packets = read_buffer();

	for(auto& pack : new_packets) if(pack.receiver_address == address || pack.receiver_address == 0)
	{
		int broker_address = pack.sender_address;
		int key = (connections.count(broker_address) && connections[broker_address].status_code==200)? connections[broker_address].key : 0;

		pack.decrypt(key);
//		if(!pack.authentic()) continue;

		if(pack.type == "cres") read_cres(pack);
		if(pack.type == "sres") read_sres(pack);
		if(pack.type == "publ") read_publ(pack);

		if(pack.type == "join") read_join(pack);
		if(pack.type == "left") read_left(pack);
		if(pack.type == "ping") read_ping(pack);
	}
}




void client::send_creq(int broker_address)
{
	if(    connections.count(broker_address)) {cout<<"Already connected to broker " << broker_address <<endl<<endl; return;}
	if(!active_brokers.count(broker_address)) {cout<<"No active broker at address " << broker_address <<endl<<endl; return;}

	int pre_key = rand()%100+1;
	packet pack = {.receiver_address = broker_address, .type = "creq", .payload = {to_string(Diffie_Hellman(pre_key))} };
	send(pack);

	connections[broker_address] = connection{.address=broker_address, .time_stamp=time(0), .key=pre_key};
}

void client::send_creq()
{
	if(active_brokers.size()==0) {cout<<"no active broker in the network" <<endl; return;}
	if(active_brokers.size()==1) { send_creq( active_brokers.begin() ->  first ); return;}

	cout<<"You must specify broker address. Currently active brokers addresses are:"<<endl;
	for(auto it : active_brokers) cout<<it.first<<endl;
	cout<<endl;
}

void client::read_cres(packet pack)
{
	int broker_address = pack.sender_address;
	int status_code = stoi(pack.payload[0]);

	if(status_code==200)	//Status OK
	{
		connections[broker_address].key = Diffie_Hellman(connections[broker_address].key, stoi(pack.payload[1]));
		connections[broker_address].status_code = 200;
		cout<<"Successfully connected to broker "<<broker_address<<" using key "<<connections[broker_address].key<<endl<<endl;
		return;
	} 	
	cout<<"Broker "<<broker_address<<" sent error response (status_code "<<status_code<<")"<<endl;		//Bad connection

	if(status_code==509) cout<<"Server band width limit exceeded"<<endl;

	cout<<endl;
}





void client::send_sreq(string topic, int broker_address)
{
	if(!active_brokers.count(broker_address)) {cout<<"No active broker at address " << broker_address <<endl; return;}
	if(!   connections.count(broker_address)) {cout<<"Not connected to broker "     << broker_address <<endl; return;}
	
	if(subscriptions.count(topic) && any_of(subscriptions[topic].begin(), subscriptions[topic].end(), [=](const subscription& s){return s.address==broker_address;} ) ) 
	{cout<<"Already subscribed to broker " << broker_address << " on topic "<< topic <<endl; return;}

	send(packet{.receiver_address = broker_address, .type = "sreq", .payload = {topic} });

	subscriptions[topic].push_back(subscription{.topic = topic, .address = broker_address, .time_stamp = time(0)});
}

void client::send_sreq(string topic)
{
	if(connections.size()==0) {cout << "Not connected to any broker" <<endl; return;}
	if(connections.size()==1) {send_sreq(topic, connections.begin()->first); return;}

	cout<<"You must specify broker address. Currently connected brokers addresses are:"<<endl;
	for(auto it : connections) cout<<it.first<<endl;
	cout<<endl;
}

void client::read_sres(packet pack)
{
	int broker_address = pack.sender_address;
	int status_code = stoi(pack.payload[0]);
	string topic = pack.payload[1];

	if(status_code==200)	//Status OK
	{
		for(auto& s : subscriptions[topic]) if(s.address==broker_address) {s.status_code=200; break;} ;
 
		cout<<"Successfully subscribed to broker "<<broker_address<<" on topic "<< topic <<endl<<endl;
		return;
	} 	
	cout<<"Broker "<<broker_address<<" sent error response (status_code "<<status_code<<")"<<" about subscription on topic "<< topic <<endl<<endl;		//Bad connection
}




void client::send_publ(entry pair, int broker_address)
{
	if(!connections.count(broker_address)) cout<<"Not connected to broker " << broker_address <<endl;

	else send(packet{.receiver_address = broker_address, .type = "publ", .payload = {pair.topic, to_string(pair.val)} });
}


void client::send_publ(entry pair)
{
	for(auto& it : connections) send_publ(pair, it.first);
}

void client::read_publ(packet pack)
{
	entry pair = {.topic = pack.payload[0], .val = stod(pack.payload[1])};

	update(pair);
	if(listening) cout<<"Update: "<<pair.topic<<" -> "<<data[pair.topic]<<endl<<endl;
}


void client::update(entry pair) {data[pair.topic] = pair.val;}




void client::read_join(packet pack)
{
	if(pack.payload[0]=="broker") active_brokers[pack.sender_address] = pack.time_stamp;
	if(pack.payload[0]=="client") active_clients[pack.sender_address] = pack.time_stamp;
}

void client::read_left(packet pack)
{
	if(pack.payload[0]=="broker") {active_brokers.erase(pack.sender_address); connections.erase(pack.sender_address);}
	if(pack.payload[0]=="client")  active_clients.erase(pack.sender_address);
}





void client::check_pings() { long int t0=time(0); for(auto& it : connections) if(it.second.last_ping < t0-30) disconnect(it.second.address, true); }

void client::send_ping(int add) { send(packet {.receiver_address = add, .type = "ping"}); }

void client::send_ping()        { for(auto& it : connections) send_ping(it.first); }

void client::read_ping(packet pack)
{
	int add = pack.sender_address;

	if(connections.count(add)) connections[add].last_ping = time(0);
	else cout<<"Ping from not connected device ("<< add <<") ignored" <<endl<<endl;
}


void client::read_disc(packet pack) { disconnect(pack.sender_address); }

void client::disconnect(int add, bool notify)
{
	if(!connections.count(add)) {cout<<"Cannot disconnect not conncted device ("<< add <<")"<<endl<<endl; return;}

	connections.erase(add);
	for(auto& it : subscriptions) remove_if(it.second.begin(), it.second.end(), [=](subscription s) {return s.address==add;} );

	if(notify) send(packet {.receiver_address = add, .type = "disc"});
}