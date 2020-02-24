#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
#include <variant>                                      // access: variant
using namespace std;
#include <unistd.h>                                     // access: getpid

// question 1a, ii)

struct Er1 { short int code; };
struct Er2 { int code; };
struct Er3 { long int code; };

struct RetCode 
{
    variant<short int, int, long int> code; 
    int type; // type can be 0, 1, 2, or 3
};

int eperiod = 10000;                                    // error period

double rtn1 ( double i, struct RetCode &err )
{
    if ( rand() % eperiod == 0 )
    {
        err.code = (short int) rand();
        err.type = 1;
        return 0;
    }
    return i;
}

double rtn2( double i , struct RetCode &err ) 
{
    if ( rand() % eperiod == 0)
    {
        err.code = (int) rand();
        err.type = 2;
        return 0;
    }
    double rtn1val = rtn1(i, err); 
    if ( err.type != 0 ) // check Error1's code 
        return 0; // ??????????

    return rtn1val + i;
}

double rtn3( double i , struct RetCode &err )
{
    if ( rand() % eperiod == 0 )
    {
        err.code = (long int) rand(); 
        err.type = 3;
        return 0;
    }

    double rtn2val = rtn2(i, err); 
    if ( err.type != 0 ) // Check Error2's code
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

    for ( int i = 0; i < times; i += 1 ) {
        // old code
        //try { rv += rtn3( i ); rc += 1; }
        //// analyse error
        //catch( Er1 ev ) { ev1 += ev.code; ec1 += 1; }
        //catch( Er2 ev ) { ev2 += ev.code; ec2 += 1; }
        //catch( Er3 ev ) { ev3 += ev.code; ec3 += 1; }
    
    // new code
        struct RetCode err; 
        err.code = 0;
        err.type = 0; 
        
    double newval = rtn3 ( i , err ); 
    if (err.type == 1) 
    {
            ev1 += get<short int>(err.code); 
        ec1 += 1;
        //newval = 0;
    }
    else if (err.type == 2)
        {
        ev2 += get<int>(err.code);
        ec2 += 1;
        //newval = 0;
    }
    else if (err.type == 3)
        {
            ev3 += get<long int>(err.code);
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
