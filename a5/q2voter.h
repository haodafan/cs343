#pragma once
#include "q2tallyvotes.h"
#include "MPRNG.h"
#include <iostream>
#include <string>

//class TallyVotes;
#if defined( EXT ) || ( INT )
_Monitor Printer;
#else 
_Cormonitor Printer;
#endif 

extern MPRNG mprng;

_Task Voter {

    unsigned int id; 
    unsigned int nvotes; 
    TallyVotes & voteTallier; 
    Printer & printer;

    int seed; // for random number generator

    void main();

    TallyVotes::Ballot cast() {              // cast 3-way vote
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