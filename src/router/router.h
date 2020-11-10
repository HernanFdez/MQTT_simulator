#include <bits/stdc++.h>

using namespace std;




class router
{
	int gate=0;
	bool connected=false;
public:
	router();
	~router();

	void check_uploads();
	void check_downloads();
	
};