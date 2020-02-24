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

// new Ackermann
jmp_buf buf;
int state;

long int Ackermann( long int m, long int n)
{
    
    jmp_buf prevbuf;  
    memcpy(prevbuf, buf, sizeof(jmp_buf));

    calls += 1; 
    if ( m == 0 ) { 
        if ( rand() % eperiod == 0 ) { 
            PRT( T t; ) 
            excepts += 1;
            //PRT( t.~T(); )
                   longjmp(buf, 1); }
        return n + 1;
    } else if ( n == 0 ) {
        state = setjmp(buf); 
        // catch statement from nested function
        if ( state == 1 ) {
            PRT ( cout << "E1 " << m << " " << n << endl );
            if ( rand() % eperiod == 0 ) { 
                memcpy(buf, prevbuf, sizeof(jmp_buf));
                PRT ( T t; ) excepts += 1; 
                //PRT( t.~T(); )
                       longjmp(buf, 1);
                   }
        }
        else { //try
            int result = Ackermann( m - 1, 1 );     
            memcpy(buf, prevbuf, sizeof(jmp_buf));
            return result;
        }    
    }
    else 
    {
        state = setjmp(buf); 
        // catch nested function
        if ( state == 1 ) {
            PRT ( cout << "E2 " << m << " " << n << endl );
            if ( rand() % eperiod == 0 ) { 
                memcpy(buf, prevbuf, sizeof(jmp_buf));
                PRT ( T t; )
                excepts += 1;
                //PRT( t.~T(); ) //this the thing that's different?
                longjmp(buf, 1);
            }
        }
        else {
            //cout << "else return Ackermann(" << m << "-1" << ",Ackermann(" << m << "," << n <<"-1" << ") );" << endl;
            int result = Ackermann ( m - 1, Ackermann( m, n - 1 ) ); 
            memcpy(buf, prevbuf, sizeof(jmp_buf)); 
            return result;
        } 
    } //if

    return 0;

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
    if (state == 0) {
        PRT( cout << m << " " << n << " " << seed << " " << eperiod << endl );
        long int val = Ackermann( m, n );
        PRT( cout << val << endl );
    }
    //} catch( E ) {
    else if (state != 0) {
        PRT( cout << "E3" << endl );
    }
    
    //} // try
    cout << "calls " << calls << ' ' << " exceptions " << excepts << endl;
}
