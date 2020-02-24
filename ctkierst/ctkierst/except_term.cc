#include <iostream>
using namespace std;

_Event T {};

void f( int level ) {
    cout << "f in: level( " << level << ")" << endl;
    if ( level > 0 ) {
		f( level - 1 );
    } else {
		cout << "throw T" << endl;
		throw T();
		cout << "after throwing T" << endl;
    } // if
    cout << "f out: level( " << level << ")" << endl;
} // f
    
int main() {
    try {
		f( 3 );
    } catch( T ) {
		cout << "Caught T" << endl;
    } // try
} // main
