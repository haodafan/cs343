#pragma once 

#if defined( SEM )
#include <uSemaphore.h>
#endif

#if defined( BAR )
#include <uBarrier.h>
#endif

_Monitor Printer;

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
    uSemaphore owner; 
    uSemaphore bargeSync; 
    uSemaphore tourSync; 
#elif defined( BAR )                // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
    int barrierFillers = 0;
    unsigned int* failures;
    unsigned int f_i; // index of failures
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
    unsigned int touring = 0; // prevent barging
    unsigned int bargeLockCount = 0;
    unsigned int groupid = 0;
    char currtk;

  public:                            // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer )
        #if defined( BAR )
        : uBarrier(group), voters(voters), group(group), printer(printer) {
            failures = new unsigned int[group];
            f_i = 0;
        #else
        : voters(voters), group(group), printer(printer) {
        #endif 
            #if defined( SEM )
                bargeSync.P(); 
                tourSync.P(); // P() to add to not full group, V() to subtract from full group
            #endif 
        } 
    #if defined( BAR )
    ~TallyVotes() 
    {
        delete [] failures;
    }
    #endif
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
