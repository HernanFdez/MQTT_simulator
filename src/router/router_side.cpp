#include <bits/stdc++.h>
#include <thread>

#include "router.h"

using namespace std;


router rt;
bool active = true;


void print_help()
{
	cout << "----------------------------------------------------"	<< endl
		 << "| help                  -> show this system help   |"	<< endl
		 << "| exit                  -> exit session            |" << endl
		 << "----------------------------------------------------"	<< endl << endl;
}



int main(int argc, char const *argv[])
{	
	print_help();

    thread uploader   { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(1)); rt.check_uploads  ();} } };
    thread downloader { [=]() { while(active) {this_thread::sleep_for(chrono::seconds(1)); rt.check_downloads();} } };


    while(true)
    {
    	string input, word;
    	vector<string> cmd;
    	getline(cin, input);	
    	istringstream iss(input);
    	while(!iss.eof()) {iss>>word; cmd.push_back(word);}

    	if(cmd[0] == "help") {print_help();}
    	if(cmd[0] == "exit") {active=false; break;}



    }


    uploader.join();
    downloader.join();

	return 0;
}