#include <bits/stdc++.h>

#include "router.h"
#include "requests.h"


using namespace std;


router::router() {}
router::~router() {}






int main(int argc, char const *argv[])
{

//	post_packet( packet{ .time_stamp = time(0), .sender_address = 1, .receiver_address = 2, .type = "join", .payload = {"it","works"}, .signature = 7 } );

	send_packet(get_packet(0), 0);
	
	return 0;
}

