#pragma once

#include "voter.h"
#include "tallyVotes.h"

_Monitor Printer {
    const unsigned int Voters, Undefined;
    unsigned int prev;					// previous inserter into the buffer
    unsigned int cnt;					// number of filled buffer elements

    // Defines information that needs to be tracked.
    struct Items {
	unsigned int id;
	Voter::States state;
	union {
	    TallyVotes::Ballot ballot;
	    unsigned int numBlocked;
	    TallyVotes::Tour tour;
	};
    } curr, *columns;                                   // curr is the current incoming information, columns is the stored information.
  public:
    Printer( unsigned int voters );
    ~Printer();
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot vote );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
}; // Printer

// Local Variables: //
// mode: c++ //
// compile-command: "make vote IMPL=EXT" //
// End: //
