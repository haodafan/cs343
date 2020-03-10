#include "q2printer.h"
#include "q2tallyVotes.h"
#include <iostream>
#include <string>

using namespace std;

extern MPRNG mprng;

// Common Function Implementations 
void Voter::main()
{
    // Random obj
    mprng.set_seed(seed);

    // Yield 0-19 times to ensure tasks do not start simultaneously 
    yield(mprng(19));

    // Perform task nvotes times
    for (int count = 0; count < nvotes; count++)
    {
        // print start message 
        printer.print(id, Voter::Start);

        // yield 0-4 times 
        yield(mprng(4));

        // vote
        TallyVotes::Tour tour = voteTallier.vote(id, cast());

        // yield 0-4 times 
        yield(mprng(4)); 
    }

    // Call Done
    #if defined( BAR )
    voteTallier.done(id);
    #else 
    voteTallier.done();
    #endif

    // print termination message 
    printer.print(id, Voter::Terminated);
}

// Special Function Implementations 
// ======================================= TallyVotes::done ===============================================
#if defined( BAR )
#else 
void TallyVotes::done()
{
    uOwnerLock.acquire();
    voters--;
    if (voters < group)
    {
        throw Failed(); // is this right? // check the fucking assignment 
    }
    uOwnerLock.release();
}
#endif 
// ======================================= TallyVotes::vote ===============================================
#if defined( MC )
TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot )
{
    ownerlock.acquire(); 
    // Barging line
    if (voterCount == group)
    {
        bargeLock.wait(ownerlock);

        // Check if we can let another barger in, let them in if we can
        if (voterCount < group)
            bargeLock.signal(); 
    }

    voterCount++;

    // Add vote! 
    votesP += ballot.picture; 
    votesG += ballot.giftshop;
    votesS += ballot.statue;

    try 
    {
        // Check if we can leave ... 
        if (voterCount < group)
        {
            tourLock.wait(ownerlock);
        }
        
        // Go on tour!! :) 
        if (!tourLock.empty())
        {
            tourLock.broadcast(); 
        }
    }// try 
    catch (Failed f) // Quorum Failure
    {
        // If we have a quorum failure, notify everyone else of the failure
        tourLock.broadcast();
        throw f;
    }


    // Get Tour results, Default: G > P > S
    Tour t;
    try {
        if (votesG >= votesP && votesG >= votesS) t.tourkind = GiftShop;
        else if (votesP > votesG && votesP >= votesS) t.tourkind = Picture;
        else if (votesS > votesG && votesS > votesP) t.tourkind = Statue;
        else throw 1;
    }
    catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; return t; }

    t.groupno = currgroup;

    votesP -= ballot.picture;
    votesG -= ballot.giftshop;
    votesS -= ballot.statue; 
    voterCount--;

    if (voterCount == 0) // Last voter responsibilities...
    {
        // Update group number
        currgroup++;

        // All tour voters are gone, we can let others barge 
        bargeLock.signal(); 
    }
    return t;
    ownerlock.release(); 
}
#endif 

// Main function
int main( int argc, char *argv[] ) {
    // defaults
    int voters = 6;
    int group = 3; 
    int votes = 1; 
    int seed = getpid(); 
    int processors = 1;
    //vote [ voters | ’d’ [ group | ’d’ [ votes | ’d’ [ seed | ’d’ [ processors | ’d’ ] ] ] ] ]

    try {
        switch ( argc ) {
          case 6: processors = stoi( argv[5] ); if ( processors <= 0 ) throw 1;
          case 5: seed = stoi( argv[4] ); if ( seed <= 0 ) throw 1;
          case 4: votes = stoi( argv[3] ); if ( votes <= 0 ) throw 1;
          case 3: group = stoi( argv[2] ); if ( group <= 0 ) throw 1;
          case 2: voters = stoi( argv[1] ); if ( voters <= 0 ) throw 1;
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
    for (int i = 0; i < voters; i++)
    {
        v[i] = new Voter(i, votes, tv, printer, seed);
    }

    // Delete (terminate) voters
    for (int i = 0; i < voters; i++)
    {
        delete v[i];
    }

    // We're done here? 
    return 0; 
}