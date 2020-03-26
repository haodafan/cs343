#include "q2printer.h"

/*
S         start
V p,s,g   vote with ballot containing 3 rankings
B n       block during voting, n voters waiting (including self)
U n       unblock after group reached, n voters still waiting (not including self)
b n       block barging task, n waiting for signalled tasks to unblock (avoidance) (including self)
D         wake to check for done (EXT/TASK only)
C t       complete group and voting result is t (p/s/g)
G t g     go on tour, t (p/s/g) in tour group number g
X         failed to form a group
T         voter terminates (after call to done)
*/


void Printer::print( unsigned int id, Voter::States state )
{

}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour )
{

}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot )
{

}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked )
{
    
}