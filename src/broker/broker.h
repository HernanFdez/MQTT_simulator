#include <bits/stdc++.h>

using namespace std;


class packet;
class connection;
class subscription;
class entry;


class broker
{
	int address=0;
	int num_connections=0, max_connections=100;
	map<int, int> active_brokers, active_clients;

	map<int, connection> connections;
	map<string, vector<subscription> > subscriptions;
	map<string, double> data;

	bool listening=true;

public:
	broker();
	~broker();


	void check_packets();
	void check_pings();
	void toggle_listening() {listening=!listening;}

	void send(packet);

	void send_cres(int client_address, int status_code);
	void send_sres(int client_address, int status_code, string topic);
	void send_publ(entry pair, int client_address);

	void read_creq(packet);
	void read_sreq(packet);
	void read_publ(packet);

	void update   (entry pair);

	void read_join (packet);
	void read_left (packet);


	void send_ping(int add);
	void send_ping();
	void read_ping(packet);

	void disconnect(int add, bool notify = true);
	void disconnect();
	void read_disc(packet);
};