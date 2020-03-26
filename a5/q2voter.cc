#include "q2voter.h"
#include "q2printer.h"

#ifdef NOOUTPUT
#define PRINT( args... ) 
#else
#define PRINT( args... ) printer.print( args )
#endif // NOOUTPUT

using namespace std;

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
        PRINT(id, Voter::Start);

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
            PRINT(id, Voter::Failed);
            break;
        }

        // yield 0-4 times 
        yield(mprng(4)); 
    }
    // Call Done
    voteTallier.done();
    
    // print termination message 
    PRINT(id, Voter::Terminated);
}
