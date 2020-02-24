#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid

// question 1a, i)

struct Er1 { short int code; };
struct Er2 { int code; };
struct Er3 { long int code; };

int eperiod = 10000;                                    // error period

// rtn1 returns the argument, has a random chancce to throw an exception
//double rtn1( double i ) {
//    if ( rand() % eperiod == 0 ) throw Er1{ (short int)rand() };
//    return i;
//}

// rtn2 returns rtn1 + the argument, has a random chance to throw an exception
//double rtn2( double i  ) {
//    if ( rand() % eperiod == 0 ) throw Er2{ rand() };
//    return rtn1( i ) + i;
//}

// rtn3 returs rtn2 + the argument, has a random chance to throw an exception 
//double rtn3( double i  ) {
//    if ( rand() % eperiod == 0 ) throw Er3{ rand() };
//    return rtn2( i ) + i;
//}
//

// global flags

struct Er1 flag_er1; 
struct Er2 flag_er2; 
struct Er3 flag_er3;

double rtn1 ( double i )
{
    if ( rand() % eperiod == 0 )
    {
        flag_er1.code = (short int) rand();
        return 0;
    }
    return i;
}

double rtn2( double i ) 
{
    if ( rand() % eperiod == 0)
    {
        flag_er2.code = (int) rand();
        return 0;
    }
    double rtn1val = rtn1(i); 
    if ( flag_er1.code != 0 ) // check Error1's code 
        return 0; // ??????????

    return rtn1val + i;
}

double rtn3( double i )
{
    if ( rand() % eperiod == 0 )
    {
        flag_er3.code = (long int) rand(); 
        return 0;
    }

    double rtn2val = rtn2(i); 
    if (flag_er2.code != 0) // Check Error2's code
        return 0; // exit? 

    return rtn2val + i;
}

int main( int argc, char * argv[] ) {
    int times = 100000000, seed = getpid();             // default values
    try {
        switch ( argc ) {
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                eperiod = stoi( argv[2] ); if ( eperiod <= 0 ) throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                times = stoi( argv[1] ); if ( times <= 0 ) throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    srand( seed );

    double rv = 0.0;
    int ev1 = 0, ev2 = 0, ev3 = 0;
    int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;
    
    // initialize error flags 
    flag_er1.code = 0; 
    flag_er2.code = 0;
    flag_er3.code = 0;

    for ( int i = 0; i < times; i += 1 ) {
        // old code
        //try { rv += rtn3( i ); rc += 1; }
        //// analyse error
        //catch( Er1 ev ) { ev1 += ev.code; ec1 += 1; }
        //catch( Er2 ev ) { ev2 += ev.code; ec2 += 1; }
        //catch( Er3 ev ) { ev3 += ev.code; ec3 += 1; }
    
    // new code
    double newval = rtn3 ( i ); 
    if (flag_er1.code != 0) 
    {
            ev1 += flag_er1.code; 
        flag_er1.code = 0;
        ec1 += 1;
        //newval = 0;
    }
    else if (flag_er2.code != 0)
        {
        ev2 += flag_er2.code;
        flag_er2.code = 0;
        ec2 += 1;
        //newval = 0;
    }
    else if (flag_er3.code != 0)
        {
            ev3 += flag_er3.code; 
        flag_er3.code = 0;
        ec3 += 1;
        //newval = 0;
        }
    else 
        {
            rv += newval;
        rc += 1;
        }
    
    } // for
    cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
    cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
