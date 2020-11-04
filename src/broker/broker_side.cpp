#include <bits/stdc++.h>
#include <thread>

#include "broker.h"

using namespace std;

broker brok;
bool active = true;


void print_help()
{
	cout << "-------------------------------------------------------" << endl
		 << "| help             -> shows this system help          |" << endl
		 << "| exit             -> exits session                   |" << endl
		 << "| disc $address    -> disconnects client or publisher |" << endl
		 << "| tgll             -> toggles listening               |" << endl
		 << "-------------------------------------------------------" << endl << endl;
}



int main(int argc, char const *argv[])
{	
	print_help();

    thread packs_checker { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(1) ); brok.check_packets();} } };
    thread pings_checker { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(5) ); brok.check_pings  ();} } };
    thread pings_sender  { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(10)); brok.send_ping    ();} } };


    while(true)
    {
    	string input, word;
    	vector<string> cmd;
    	getline(cin, input);
    	istringstream iss(input);
    	while(!iss.eof()) {iss>>word; cmd.push_back(word);}

    	if(cmd[0] == "help") print_help();
    	if(cmd[0] == "exit") {active=false; break;}
    	if(cmd[0] == "tgll") brok.toggle_listening();
    	if(cmd[0] == "disc") brok.disconnect(stoi(cmd[1]));
    }


    packs_checker.join();
    pings_checker.join();

	return 0;
}