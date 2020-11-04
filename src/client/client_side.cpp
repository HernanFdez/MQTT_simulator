#include <bits/stdc++.h>
#include <thread>

#include "client.h"
#include "logs.h"

using namespace std;


client cl;
bool active = true;


void print_help()
{
	cout << "-----------------------------------------------------"	<< endl
		 << "| help                  -> shows this system help   |"	<< endl
		 << "| exit                  -> exits session            |" << endl
		 << "| disc $address         -> disconnects from broker  |"	<< endl
		 << "-----------------------------------------------------"	<< endl << endl;
}



int main(int argc, char const *argv[])
{	
	print_help();

    thread checker { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(1)); cl.check_packets();} } };


    while(true)
    {
    	string input, word;
    	vector<string> cmd;
    	getline(cin, input);	
    	istringstream iss(input);
    	while(!iss.eof()) {iss>>word; cmd.push_back(word);}

    	if(cmd[0] == "help") {print_help();}
    	if(cmd[0] == "exit") {active=false; break;}
//    	if(cmd[0] == "tgll") {cl.toggle_listening();}

    	if(cmd[0] == "conn")
		{
			if(cmd.size()==1) cl.send_creq();
			if(cmd.size()==2) cl.send_creq(stoi(cmd[1]));
		}

		if(cmd[0] == "subs")
		{
			if(cmd.size()==2) cl.send_sreq(cmd[1]              );
			if(cmd.size()==3) cl.send_sreq(cmd[1], stoi(cmd[2]));
		}

		if(cmd[0] == "publ")
		{
			if(cmd.size()==3) cl.send_publ(entry {cmd[1], stod(cmd[2])}               );
			if(cmd.size()==4) cl.send_publ(entry {cmd[1], stod(cmd[2])}, stoi(cmd[3]) );
		}

    }


    checker.join();
	return 0;
}