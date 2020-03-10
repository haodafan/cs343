#pragma once 

#include "MPRNG.h" // rng

_Monitor Printer;
MPRNG mprng;

#if defined( MC )                    // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
    uOwnerLock ownerlock; 
    uCondLock bargeLock; // Lock for bargers 
    uCondLock tourLock; // Lock for tour group fill wait

#elif defined( SEM )                // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( BAR )                // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    unsigned int voters; 
    unsigned int group; 
    Printer & printer; 

    unsigned int votesP = 0; 
    unsigned int votesS = 0; 
    unsigned int votesG = 0; 
    unsigned int voterCount = 0;
    unsigned int currgroup = 0
  public:                            // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer )
        : voters(voters), group(group), printer(printer) {} 
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum TourKind { Picture = 'p', Statue = 's', GiftShop = 'g' };
    struct Tour { TourKind tourkind; unsigned int groupno; };
    Tour vote( unsigned int id, Ballot ballot );
    void done(
    #if defined( BAR )               // barrier solution
        unsigned int id
    #endif
    );
};

_Task Voter {
    unsigned int id; 
    unsigned int nvotes; 
    TallyVotes & voteTallier; 
    Printer & printer;

    int seed; // for random number generator

    void main();
    TallyVotes::Ballot cast() {        // cast 3-way vote (do not implement)
        // O(1) random selection of 3 items without replacement using divide and conquer.
        static const unsigned int voting[3][2][2] = { { {2,1}, {1,2} }, { {0,2}, {2,0} }, { {0,1}, {1,0} } };
        unsigned int picture = mprng( 2 ), statue = mprng( 1 );
        return (TallyVotes::Ballot){ picture, voting[picture][statue][0], voting[picture][statue][1] };
    }
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
          Done = 'D', Complete = 'C', Going = 'G', Failed = 'X', Terminated = 'T' };
    Voter( unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer, int seed ) 
        : id(id), nvotes(nvotes), voteTallier(voteTallier), printer(printer), seed(seed) {}
};
