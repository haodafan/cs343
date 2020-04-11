#include "groupoff.h"

#include "MPRNG.h"
#include <assert>


MPRNG mprng;

// Direct Communication 
// Groupoff is a server which provides the client with a future giftcard (which eventually becomes a WATCard giftcard)

// Note: typedef Future_ISM<WATCard *> FWATCard;	

Groupoff::Groupoff( Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay )
    : prt(prt) , numStudents(numStudents) , maxTripCost(maxTripCost) , groupoffDelay(groupoffDelay)
{
    // Initialize array
    requests = new Work[numStudents];
    
    // Initialize randomization
    int seed = getpid(); 
    mprng.set_seed(seed);
}

Groupoff::~Groupoff()
{
    delete [] requests;
}

int Groupoff::getRandomizedRequestedWorkIndex(int numStudentsLeft)
{
    int nthchoice = mprng(numStudentsLeft);
    int index = 0; 

    int limit = nthchoice * numStudents + 1; 

    // We will take the nth requested piece of work
    while (true) 
    {
        if (requests[index].status == WorkStatus::Requested)
        {
            nthchoice--;
            if (nthchoice == 0) break;
        }
        index++; 
        if (index == numStudents) index = 0;

        // To ensure we are not infinite looping
        if (limit == 0) 
        {
            throw _Event("ERROR: NO MORE WORK NEEDS COMPLETING");
        }
        limit--; 
    }
    return index;
}


void Groupoff::main()
{
    printer.print( Printer::Groupoff, 'S' );

    // Groupoff task begins by accepting a call from each student to obtain a future giftcard 
    for (unsigned int i = 0; i < numStudents; i++)
    {
        _Accept( giftcard ) {} 
        or _Accept( ~Groupoff ) // stop when destructor is called
        {
            printer.print( Printer::Groupoff , 'F');
            return;
        }
    }

    // Periodically puts a real WATCard w value maxTripCost into a random future giftcard 
    // Before each giftcard is assigned a real WATCard, groupoff yields groupoffDelay times
    for (unsigned int i = 0; i < numStudents; i++)
    {
        // Loops until all future giftcards are assigned or destructor is called
        _Accept( ~Groupoff )
        {
            break; 
        }
        _Else // must not block on destructor call
        {
            // Before each giftcard is assigned, groupoff yields groupoffDelay times
            yield(groupoffDelay);

            int selection = getRandomizedRequestedWorkIndex(numStudents - i);
            assert(requests[selection].status != WorkStatus::Completed)

            requests[selection].card = new WATCard();
            requests[selection].card->deposit(maxTripCost);
            requests[selection].status = WorkStatus::Completed;
        }
    }
    printer.print( Printer::Groupoff , 'F');
    return;

}

// Called by clients
WATCard::FWATCard Groupoff::giftCard()
{   
    requests[numRequests].status = WorkStatus::Requested; 
    numRequests++; 
    return requests[numRequests].card;
}