#include <iostream>
using namespace std;

_Event T {};

void f( int level ) {
    cout << "f in: level( " << level << ")" << endl;
    if ( level > 0 ) {
        f( level - 1);
    } else {
        cout << "resume T" << endl;
        _Resume T();
	    // resumption continues after the throw/raise
        cout << "after resuming T" << endl;
    } // if
    cout << "f out: level( " << level << " )" << endl;
} // f
    
int main() {
  try {  // retry continues from start of guard block
      cout << "before call to f" << endl;
      f( 3 );
      cout << "after call to f" << endl;
  } _CatchResume( T ) {
    cout << "Caught T" << endl;
  } // try
  cout << "after catch" << endl;  // termination model continues from here
}
