#include <stdio.h>
#include <stdlib.h>                                      // access: rand, srand
#include <string.h>                                      // access: strcmp
#include <stdbool.h>                                     // access: bool
#include <unistd.h>                                     // access: getpid

// question 1a, iii)

struct Er1 { short int code; };
struct Er2 { int code; };
struct Er3 { long int code; };

struct RetUnion 
{
    double value;
    short int code1;
    int code2;
    long int code3;
};

struct RetUnion createDefaultRetUnion()
{
    struct RetUnion ret;
    ret.value = 0;
    ret.code1 = 0;
    ret.code2 = 0;
    ret.code3 = 0;
    return ret;
}

int eperiod = 10000;                                    // error period

struct RetUnion rtn1 ( double i )
{
    struct RetUnion ret = createDefaultRetUnion(); 

    if ( rand() % eperiod == 0 )
    {
        ret.code1 = (short int) rand();
        return ret;
    }
    ret.value = i;
    return ret;
}

struct RetUnion rtn2( double i ) 
{
    struct RetUnion ret = createDefaultRetUnion();

    if ( rand() % eperiod == 0)
    {
        ret.code2 = (int) rand();
        return ret;
    }
    ret = rtn1(i); 
    if ( ret.code1 != 0 ) // check Error1's code 
        return ret; 

    ret.value += i;
    return ret;
}

struct RetUnion rtn3( double i )
{
    struct RetUnion ret = createDefaultRetUnion();

    if ( rand() % eperiod == 0 )
    {
        ret.code3 = (long int) rand(); 
        return ret;
    }

    ret = rtn2(i); 
    if ( ret.code1 != 0 || ret.code2 != 0 ) // Check Error2's code
        return ret; // exit? 

    ret.value += i;
    return ret;
}

int main( int argc, char * argv[] ) {
    int times = 100000000, seed = getpid();             // default values

    bool badinput = 0;
    //try {
        switch ( argc ) {
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = atoi( argv[3] ); if ( seed <= 0 ) badinput = 1;//throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                eperiod = atoi( argv[2] ); if ( eperiod <= 0 ) badinput = 1;//throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                times = atoi( argv[1] ); if ( times <= 0 ) badinput = 1;//throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: badinput = 1;//throw 1;
        } // switch
    //} catch( ... ) {
    if (badinput) {
        //cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
        //exit( EXIT_FAILURE );
        fprintf(stderr, "Usage: %s [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]\n", argv[0]); 
    exit( EXIT_FAILURE );
    } //if
    //} // try
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
        struct RetUnion ret = createDefaultRetUnion();
        
    ret = rtn3 ( i ); 
    if (ret.code1 != 0) 
    {
            ev1 += ret.code1; 
        ec1 += 1;
        //newval = 0;
    }
    else if (ret.code2 != 0)
        {
        ev2 += ret.code2;
        ec2 += 1;
        //newval = 0;
    }
    else if (ret.code3 != 0)
        {
            ev3 += ret.code3;
        ec3 += 1;
        //newval = 0;
        }
    else 
        {
            rv += ret.value;
        rc += 1;
        }
    
    } // for
    //cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
    fprintf(stdout, "normal result %g exception results %d %d %d \n", rv, ev1, ev2, ev3);
    //cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
    fprintf(stdout, "calls %d exceptions %d %d %d \n", rc, ec1, ec2, ec3);
}
