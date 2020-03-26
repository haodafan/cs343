#include "q2tallyvotes.h"
#include "q2printer.h"
#include <iostream>
#include <string>

#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

using namespace std;

// Semi-Special Implementations 
void TallyVotes::done()
{
    voters--;
    
    if (voters < group)
    {
        Ballot b;
        vote(0, b); // Vote again to release the remaining voters
    }
}

// Special Implementations 
#if defined( EXT )                           // external scheduling monitor solution
TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot )
{

    // First detect quorum failure? (bad quorum detection logic?)
    if (voters < group)
    {
        // QUORUM FAILURE
        throw Failed(); 
    }

    // Barging wait line 
    if (count == group)
    {
        barging++; 
        _Accept( done ); // When everyone is "done", then the next portion may continue 
        PRINT(id, Voter::Barging, bargeLockCount);
        
        bargeLockCount--;
    }

    // Check quorum failure again 
    if (voters < group)
    {
        // QUORUM FAILURE
        throw Failed(); 
    }

    // ---------- VOTE ----------
    PRINT(id, Voter::Vote, ballot);

    // Add vote!
    votesP += ballot.picture;
    votesG += ballot.giftshop;
    votesS += ballot.statue; 
    count++;

    // Wait for group... 
    if (count < group)
    {
        _Accept( vote );
    }

    // Quorum check

    // Go on tour!! :) 
    Tour savedtour;
    if (count == group) // Only the first in tourlock decides the tour
    {
        try {
            t.groupno = groupno;
            // Get Tour results, Default: G > P > S
            if (votesG >= votesP && votesG >= votesS) t.tourkind = (TourKind) 'g';
            else if (votesP > votesG && votesP >= votesS) t.tourkind = (TourKind) 'p';
            else if (votesS > votesG && votesS > votesP) t.tourkind = (TourKind) 's';
            else throw 1;
        }
        catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; owner.V(); return t; }

        PRINT(id, Voter::Complete, t);
    }

    PRINT(id, Voter::Going, t);

    savedtour = t; // save this information even after _Accept()
    count--;
    if (count > 0)
    {
        _Accept( done );
    }

    return savedtour;
}

#endif 


#if defined( INTB )    
// Provided implementations
void TallyVotes::wait() {
    bench.wait();                            // wait until signalled
    while ( rand() % 2 == 0 ) {              // multiple bargers allowed
        try {
            _Accept( vote, done ) {          // accept barging callers
            } _Else {                        // do not wait if no callers
            } // _Accept
        } catch( uMutexFailure::RendezvousFailure & ) {}
    } // while
}

void TallyVotes::signalAll() {               // also useful
    while ( ! bench.empty() ) bench.signal();// drain the condition
}
#endif 