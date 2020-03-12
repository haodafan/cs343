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

// Special Function Implementations 
// ======================================= TallyVotes::done =======================================
#if defined( BAR )
void TallyVotes::done(unsigned int id)
{
    voters--;

    // Check if task was unblocked by quorum failure
    for (unsigned int i = 0; i <= f_i; i++)
    {
        if (failures[i] == id)
            return;
    }

    // Complete barrier for unfinished ones
    if (voters + barrierFillers < group)
    {
        barrierFillers++;
        block();
    }
    //voters--;
}
#elif defined( MC )
void TallyVotes::done()
{
    ownerlock.acquire();

    voters--;

    if (voters < group)
    {
        // quorum failure logic
        tourLock.broadcast();
        bargeLock.broadcast();
    }
    ownerlock.release();
}
#elif defined( SEM )
void TallyVotes::done()
{
    owner.P();
    voters--;
    if (voters < group)
    {
        // quorum failure logic
        if (voters > 0)
            bargeSync.V();
        if (voterCount > 0)
            tourSync.V();
    }
    owner.V();
}
#endif 
// ======================================= TallyVotes::vote ========================================
#if defined( MC ) // --------------------------------------------------------------
TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot )
{
    ownerlock.acquire(); 
    
    // First detect quorum failure? 
    if (voters < group)
    {
        // QUORUM FAILURE
        ownerlock.release();
        throw Failed(); 
    }

    // ---------- VOTE ----------
    PRINT(id, Voter::Vote, ballot);

    // Barging line
    if (voterCount == group || touring > 0)
    {
        bargeLockCount++;
        PRINT(id, Voter::Barging, bargeLockCount);
        bargeLock.wait(ownerlock);

        // Check if we can let another barger in, let them in if we can
        if (voterCount < group)
            bargeLock.signal(); 
        
        bargeLockCount--;
    }

    // Check for quorum failure
    if (voters < group)
    {
        // QUORUM FAILURE
        ownerlock.release();
        throw Failed();
    }

    // Add vote! 
    voterCount++;
    votesP += ballot.picture;
    votesG += ballot.giftshop;
    votesS += ballot.statue;

    // Check if we can leave ... 
    if (voterCount < group)
    {
        PRINT(id, Voter::Block, voterCount);
        tourLock.wait(ownerlock);
        PRINT(id, Voter::Unblock, voterCount-1);
    }
    // Check for quorum failure
    if (voters < group)
    {
        // QUORUM FAILURE
        ownerlock.release();
        throw Failed();
    }

    // Go on tour!! :) 
    Tour t;
    if (voterCount == group) // Only the first in tourlock decides the tour
    {
        touring = voterCount;
        try {
            // Get Tour results, Default: G > P > S
            if (votesG >= votesP && votesG >= votesS) currtk = 'g';
            else if (votesP > votesG && votesP >= votesS) currtk = 'p';
            else if (votesS > votesG && votesS > votesP) currtk = 's';
            else throw 1;
        }
        catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; return t; }
        
        // Voting result complete... 
        t.tourkind = (TourKind) currtk;
        t.groupno = groupid;
        PRINT(id, Voter::Complete, t);

        // Alert everyone in the tour group
        if (!tourLock.empty())
        {
            tourLock.broadcast(); 
            PRINT(id, Voter::Unblock, voterCount - 1);
        }

    }

    t.tourkind = (TourKind) currtk;
    t.groupno = groupid;

    PRINT(id, Voter::Going, t);

    // Leave for tour, update group data... 
    votesP -= ballot.picture;
    votesG -= ballot.giftshop;
    votesS -= ballot.statue; 
    voterCount--;
    touring--;

    if (voterCount == 0) // Last tourist on tour must signal bargers to enter
    {
        // Update group number
        groupid++;

        // All tour voters are gone, we can let others barge 
        bargeLock.signal(); 
    }
    ownerlock.release(); 
    return t;
}
#elif defined( SEM ) // --------------------------------------------------------------
TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot )
{
    owner.P();
    
    // First detect quorum failure? 
    if (voters < group)
    {
        if (voterCount > 0)
            tourSync.V();
        else if (voters > 0)
            bargeSync.V();

        // QUORUM FAILURE
        owner.V();
        throw Failed(); 
    }
    
    // Barging wait line 
    if (voterCount == group || touring > 0)
    {
        bargeLockCount++;
        PRINT(id, Voter::Barging, bargeLockCount);

        owner.V(); // release
        bargeSync.P(); // wait 
        owner.P(); // acquire

        // Check if we can let another barger in, let them in if we can
        if (voterCount < group)
            bargeSync.V(); // release 
        
        bargeLockCount--;
    }

    // ---------- VOTE ----------
    PRINT(id, Voter::Vote, ballot);

    // Check for quorum failure
    if (voters < group)
    {
        if (voterCount > 0)
            tourSync.V();
        else if (voters > 0)
            bargeSync.V();

        // QUORUM FAILURE
        owner.V();
        throw Failed();
    }

    // Add vote! 
    voterCount++;
    votesP += ballot.picture;
    votesG += ballot.giftshop;
    votesS += ballot.statue;

    // Check if we can leave ... 
    if (voterCount < group)
    {
        PRINT(id, Voter::Block, voterCount);
        owner.V(); // release
        tourSync.P(); // wait
        owner.P(); // acquire
    }

    // Check for quorum failure
    //cout << "voters: " << voters << " group: " << group << endl;
    if (voters < group)
    {
        // QUORUM FAILURE

        if (voterCount > 0)
            tourSync.V();
        else if (voters > 0)
            bargeSync.V();

        owner.V();
        throw Failed();
    }

    // Go on tour!! :) 
    Tour t;
    if (voterCount == group) // Only the first in tourlock decides the tour
    {
        touring = voterCount;
        try {
            // Get Tour results, Default: G > P > S
            if (votesG >= votesP && votesG >= votesS) currtk = 'g';
            else if (votesP > votesG && votesP >= votesS) currtk = 'p';
            else if (votesS > votesG && votesS > votesP) currtk = 's';
            else throw 1;
        }
        catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; owner.V(); return t; }
        
        // Voting result complete... 
        t.tourkind = (TourKind) currtk;
        t.groupno = groupid;
        PRINT(id, Voter::Complete, t);

    }

    t.tourkind = (TourKind) currtk;
    t.groupno = groupid;

    PRINT(id, Voter::Going, t);
    
    // Leave for tour, update group data... 
    votesP -= ballot.picture;
    votesG -= ballot.giftshop;
    votesS -= ballot.statue; 
    voterCount--;
    touring--;
    
    // Alert another individual in the tour group
    if (voterCount > 0) 
    {
        tourSync.V(); //release
    }

    if (voterCount == 0) // Last tourist on tour must signal bargers to enter
    {
        // Update group number
        groupid++;

        // All tour voters are gone, we can let others barge 
        if (voters != 0)
            bargeSync.V(); // release
    }
    owner.V(); // release
    return t;
}
#elif defined( BAR ) // --------------------------------------------------------------
TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot )
{    
    // First detect quorum failure? 
    if (voters < group)
    {
        // QUORUM FAILURE
        block();
        failures[f_i] = id;
        f_i++;
        throw Failed(); 
    }

    // ---------- VOTE ----------
    PRINT(id, Voter::Vote, ballot);

    // Add vote!
    votesP += ballot.picture;
    votesG += ballot.giftshop;
    votesS += ballot.statue; 
    voterCount++;

    // Ensure we have enough tourists to leave ... 
    PRINT(id, Voter::Block, voterCount);
    block();

    // Check for quorum failure
    if (voters < group)
    {
        // QUORUM FAILURE
        voterCount--;
        failures[f_i] = id;
        f_i++;
        throw Failed();
    }

    // Go on tour!! :) 
    Tour t;
    if (voterCount == group) // Only the first in tourlock decides the tour
    {
        touring = voterCount;
        try {
            // Get Tour results, Default: G > P > S
            if (votesG >= votesP && votesG >= votesS) currtk = 'g';
            else if (votesP > votesG && votesP >= votesS) currtk = 'p';
            else if (votesS > votesG && votesS > votesP) currtk = 's';
            else throw 1;
        }
        catch (...) { cerr << "YOUR LOGIC BAD!!!!!!!" << endl; return t; }
        
        // Voting result complete... 
        t.tourkind = (TourKind) currtk;
        t.groupno = groupid;
        PRINT(id, Voter::Complete, t);

        // reset votes
        votesG = 0;
        votesP = 0;
        votesS = 0;
        groupid++;
    }    
    voterCount--;
    t.tourkind = (TourKind) currtk;
    t.groupno = groupid - 1;

    PRINT(id, Voter::Unblock, touring - 1);
    // Leave for tour, update group data... 
    touring--;

    PRINT(id, Voter::Going, t);

    return t;
}
#endif 