#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid
#include <setjmp.h>                                     // access: setjmp, longjmp

#ifdef NOOUTPUT
#define PRT( stmt )
#else
#define PRT( stmt ) stmt
#endif // NOOUTPUT

struct E {};                                            // exception type
PRT( struct T { ~T() { cout << "~"; } }; )
long int eperiod = 100, excepts = 0, calls = 0;        // exception period

// old Ackermann
///*
long int Ackermann( long int m, long int n ) {
    calls += 1;
    if ( m == 0 ) {
        if ( rand() % eperiod == 0 ) { PRT( T t; ) excepts += 1; throw E(); }
        return n + 1;
    } else if ( n == 0 ) {
        try { return Ackermann( m - 1, 1 );
        } catch( E ) {
            PRT( cout << "E1 " << m << " " << n << endl );
            if ( rand() % eperiod == 0 ) { PRT( T t; ) excepts += 1; throw E(); }
        } // try
    } else {
        try { return Ackermann( m - 1, Ackermann( m, n - 1 ) );
        } catch( E ) {
            PRT( cout << "E2 " << m << " " << n << endl );
            if ( rand() % eperiod == 0 ) { PRT( T t; ) excepts += 1; throw E(); }
        } // try
    } // if
    return 0;                                           // recover by returning 0
}
//*/
// new Ackermann
jmp_buf buf;
jmp_buf buf2; 
jmp_buf buf3; 
int state;
int state2; 
int state3; 

jmp_buf current_buf; // how do we know which stack we are in? :( 
int current_state;
// man what the fuck
long int Ackermann( long int m, long int n)
{
	calls += 1; 
	//int state = setjmp(buf);
	
	
}

int main( int argc, char * argv[] ) {
    long int m = 4, n = 6, seed = getpid();             // default values
    try {                                               // process command-line arguments
        switch ( argc ) {
          case 5: if ( strcmp( argv[4], "d" ) != 0 ) {  // default ?
                eperiod = stoi( argv[4] ); if ( eperiod <= 0 ) throw 1;
            } // if
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                n = stoi( argv[2] ); if ( n < 0 ) throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                m = stoi( argv[1] ); if ( m < 0 ) throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ m (>= 0) | d [ n (>= 0) | d"
             " [ seed (> 0) | d [ eperiod (> 0) | d ] ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    srand( seed );                                      // seed random number
    //try {                                             // begin program
    state = 0;
    state = setjmp(buf); // return point     
	// catch( E ) 
    if (state != 0)
    {
        PRT( cout << "E3" << endl );
    }
    else // main statement 
    {
        PRT( cout << m << " " << n << " " << seed << " " << eperiod << endl );
        long int val = Ackermann( m, n );
        PRT( cout << val << endl );
    }
    
    cout << "calls " << calls << ' ' << " exceptions " << excepts << endl;
}
