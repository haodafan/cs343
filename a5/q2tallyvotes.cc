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
        PRINT(id, Voter::Barging, barging);
        
        barging--;
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

    // Check quorum failure again 
    if (voters < group)
    {
        // QUORUM FAILURE
        throw Failed(); 
    }

    // Go on tour!! :) 
    Tour savedtour;
    if (count == group) // Only the first in tourlock decides the tour
    {
        try {
            // Get Tour results, Default: G > P > S
            if (votesG >= votesP && votesG >= votesS) currtk = 'g';
            else if (votesP > votesG && votesP >= votesS) currtk = 'p';
            else if (votesS > votesG && votesS > votesP) currtk = 's';
            else throw 1;
        }
        catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; return savedtour; }

        // Voting result complete... 
        savedtour.tourkind = (TourKind) currtk;
        savedtour.groupno = groupid;
        PRINT(id, Voter::Complete, savedtour);

        // Get rid of this round's results
        votesP = 0;
        votesG = 0;
        votesS = 0; 
    }

    // for those that missed it ... 
    savedtour.tourkind = (TourKind) currtk;
    savedtour.groupno = groupid;

    PRINT(id, Voter::Going, savedtour);

    count--;
    if (count > 0)
    {
        _Accept( done ); // Wait until all of the current ones are done before proceeding 
    }

    // Check quorum failure again 
    if (voters < group)
    {
        // QUORUM FAILURE
        throw Failed(); 
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