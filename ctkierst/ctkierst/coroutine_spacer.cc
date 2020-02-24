#include <iostream>
#include <string>
using namespace std;

_Coroutine Spacer {
    char ch;
    void main() {
	for ( ;; ) {
	    for ( int i = 0; i < 4; i += 1 ) {
		for ( int j = 0; j < 5; j += 1 ) {
		    cout << ch;
		    suspend();
		} // for
		cout << "\t";
	    } // for
	    cout << endl;
	} // for
    } // main

  public:
    void next( char ch ) {
	this->ch = ch;
	resume();
    } // next
};

int main() {
    string s = "thequickbrownfoxjumpedoverthelazydogkeepstuffingthelineforabit\0";
    Spacer spacer;
    for ( int i = 0; i < s.size(); i += 1 ) {
	spacer.next( s[i] );
    } // for
} // main
