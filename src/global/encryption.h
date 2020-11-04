#include <bits/stdc++.h>

using namespace std;


char delimiter='-';


int mod_pow(int x, int y, int p)  
{  
    long long int res = 1;    
    x = x%p; 
   
    if(x==0) return 0; 
  
    while(y>0)  
    {   
        if(y & 1) res = (res*x)%p;  
   
        y = y>>1; 
        x = (x*x) % p;  
    }  

    return res;  
} 


// simpliflied version of the Diffie-Hellman key exchange protocol 
// with fixed (prime) values for base (g) and cycle length (N)
int Diffie_Hellman(int a, int g = 541) {int N = 7919; return mod_pow(g, a, N);}


// simple encryption-decryption protocol based on adding-substracting
// a previously agreed integer (key) to every character in the string (payload)
string encrypt(string str, int key) {for(auto& c : str) c+=key; return str;}
string decrypt(string str, int key) {for(auto& c : str) c-=key; return str;}


string write_payload(vector<string> payload, int key)
{
	string payload_str = "-";
	for(auto& elem : payload) payload_str += encrypt(elem, key) + delimiter;
	return payload_str;
}

string write_payload(string elem, int key)
{
	vector<string> payload = {elem};
	return write_payload(payload, key);
}


vector<string> read_payload(string payload_str, int key)
{
	vector<string> payload;
	istringstream iss(payload_str);
	string elem;

	getline(iss, elem, delimiter);
	while(getline(iss, elem, delimiter)) payload.push_back(decrypt(elem, key));

	return payload;
}







int rand_prime(int N) 
{  
   	static vector<int> primes = {2};

   	int biggest = primes.back();
   	if(biggest < N)
   	{
      	srand(time(0));
      	vector<bool> check_list(N-biggest, true);  
      	int m = sqrt(N); 

      	for(auto& p : primes		  )	  {	if(p>m) break;   	     	for(int i=p*2; i<=N; i+=p)   if(i>biggest) 	check_list[i-biggest-1]=false; }

      	for(int p=biggest+1; p<=m; p++)   	if(check_list[p-biggest-1]) for(int i=p*2; i<=N; i+=p)              	check_list[i-biggest-1]=false; 

      	for(int p=biggest+1; p<=N; p++)  	if(check_list[p-biggest-1]) 											primes.push_back(p); 
   	}

//	for(auto& p : primes) cout<<p<<endl;

   	return primes[rand()%primes.size()];
} 


