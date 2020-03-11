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
    for (unsigned int count = 0; count < nvotes; count++)
    {
        // print start message 
        printer.print(id, Voter::Start);

        // yield 0-4 times 
        yield(mprng(4));

        // vote
        try 
        {
            voteTallier.vote(id, cast());
        }
        catch (TallyVotes::Failed & f)
        {
            // Quorum failure
            printer.print(id, Voter::Failed);
            break;
        }

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
    printer.print(id, Voter::Vote, ballot);

    // Barging line
    if (voterCount == group || touring > 0)
    {
        bargeLockCount++;
        printer.print(id, Voter::Barging, bargeLockCount);
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
        printer.print(id, Voter::Block, voterCount);
        tourLock.wait(ownerlock);
        printer.print(id, Voter::Unblock, voterCount-1);
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
        printer.print(id, Voter::Complete, t);

        // Alert everyone in the tour group
        if (!tourLock.empty())
        {
            tourLock.broadcast(); 
            printer.print(id, Voter::Unblock, voterCount - 1);
        }

    }

    t.tourkind = (TourKind) currtk;
    t.groupno = groupid;

    printer.print(id, Voter::Going, t);

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
        printer.print(id, Voter::Barging, bargeLockCount);

        owner.V(); // release
        bargeSync.P(); // wait 
        owner.P(); // acquire

        // Check if we can let another barger in, let them in if we can
        if (voterCount < group)
            bargeSync.V(); // release 
        
        bargeLockCount--;
    }

    // ---------- VOTE ----------
    printer.print(id, Voter::Vote, ballot);

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
        printer.print(id, Voter::Block, voterCount);
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
        printer.print(id, Voter::Complete, t);

    }

    t.tourkind = (TourKind) currtk;
    t.groupno = groupid;

    printer.print(id, Voter::Going, t);
    
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
    printer.print(id, Voter::Vote, ballot);

    // Add vote!
    votesP += ballot.picture;
    votesG += ballot.giftshop;
    votesS += ballot.statue; 
    voterCount++;

    // Ensure we have enough tourists to leave ... 
    printer.print(id, Voter::Block, voterCount);
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
        printer.print(id, Voter::Complete, t);

        // reset votes
        votesG = 0;
        votesP = 0;
        votesS = 0;
        groupid++;
    }    
    voterCount--;
    t.tourkind = (TourKind) currtk;
    t.groupno = groupid - 1;

    printer.print(id, Voter::Unblock, touring - 1);
    // Leave for tour, update group data... 
    touring--;

    printer.print(id, Voter::Going, t);

    return t;
}
#endif 

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
