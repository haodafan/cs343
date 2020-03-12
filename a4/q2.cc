
#include "MPRNG.h"
#include "q2Voter.h"
#include "q2tallyVotes.h"
#include "q2printer.h"

#include <iostream>
#include <string>

#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

using namespace std;
MPRNG mprng;
// Main function
int main( int argc, char *argv[] ) {
    // defaults
    unsigned int voters = 6;
    unsigned int group = 3; 
    unsigned int votes = 1; 
    int seed = getpid(); 
    int processors = 1;
    //vote [ voters | ’d’ [ group | ’d’ [ votes | ’d’ [ seed | ’d’ [ processors | ’d’ ] ] ] ] ]

    try {
        switch ( argc ) {
          case 6: processors = (*argv[5] == 'd') ? processors : stoi( argv[5] ); if ( processors <= 0 ) throw 1;
          case 5: seed = (*argv[4] == 'd') ? seed : stoi( argv[4] ); if ( seed <= 0 ) throw 1;
          case 4: votes = (*argv[3] == 'd') ? votes : stoi( argv[3] ); if ( votes <= 0 ) throw 1;
          case 3: group = (*argv[2] == 'd') ? group : stoi( argv[2] ); if ( group <= 0 ) throw 1;
          case 2: voters = (*argv[1] == 'd') ? voters : stoi( argv[1] ); if ( voters <= 0 ) throw 1;
          case 1: break;                                // use defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cout << "Usage: " << argv[0] << " [ voters (> 0)| ’d’ [ group (> 0)| ’d’ [ votes (> 0) | ’d’ [ seed (> 0)| ’d’ [ processors (> 0) | ’d’ ] ] ] ] ]" << endl;
        exit( 1 );
    } // try

    // enable multiprocessing
    uProcessor p[processors - 1];

    // Create printer
    Printer printer(voters);
    // Create vote tallier 
    TallyVotes tv(voters, group, printer);

    // Create voters
    Voter * v[voters];
    for (unsigned int i = 0; i < voters; i++)
    {
        v[i] = new Voter(i, votes, tv, printer, seed);
    }

    // Delete (terminate) voters
    for (unsigned int i = 0; i < voters; i++)
    {
        delete v[i];
    }

    // We're done here? 
    return 0; 
}
