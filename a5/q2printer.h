#pragma once

#include "q2tallyvotes.h"
#include "q2voter.h"
#if defined( EXT ) || defined( INT )
_Monitor 
#else 
_Cormonitor Printer {             // chose one of the two kinds of type constructor
  public:
    Printer( unsigned int voters );
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
#endif