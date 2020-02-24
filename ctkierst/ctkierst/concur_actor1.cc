#include <uActor.h>
#include <string>
#include <iostream>
using std::string;

struct StrMsg : public uActor::Message {
	string val;						// string message
	StrMsg( string val ) : val{val} {}
};

_Actor Hello {
	Allocation receive( Message & msg ) { // receive base type
		Case( StrMsg, msg ) {		// discriminate derived message
			std::cout << msg_d->val << std::endl;
		} 
		return Finished;
	}
};

int main() { // like COBEGIN / COEND
	uActorStart(); // start actor system
	Hello hellos[2];
	StrMsg hello( "hello" ), bonjour( "bonjour" );
	hellos[0] | hello;
	hellos[1] | bonjour;
	uActorStop();
}