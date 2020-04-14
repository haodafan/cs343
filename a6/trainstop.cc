#include "trainstop.h"

#include "global.h"
#include "student.h"
#include "nameserver.h"
#include "watcard.h"
#include "printer.h"

#include <iostream> //debugging
using namespace std; // debugging 
/*
_Task TrainStop {
	class PImpl; // *** Replace these two lines with your own implementation.
	PImpl *pimpl;

	void main();
  public:
	_Event Funds {										// Thrown when there are insufficient funds on the card to buy a ticket.
	  public:
		unsigned int amount;
		Funds( unsigned int amt );
	};

	TrainStop( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost );
	~TrainStop();
	_Nomutex unsigned int getId() const;

	// Student uses card to buy a ticket for numStops stops.
	void buy( unsigned int numStops, WATCard & card );
	
	// Student waits for a train traveling in the specified direction.
	Train * wait( unsigned int studentId, Train::Direction direction );
	
	// Student getting off the train at this stop "disembarks" at this stop.				
	void disembark( unsigned int studentId );
	
	// Timer notifies stop that timer ticked.	  
	void tick();

	// Train tells stop in which direction it is traveling, and the maximum number of students it can take.
	// It then blocks until the timer ticks over. Returns the minimum of what it can take, and the number
	// of students waiting.
	unsigned int arrive( unsigned int trainId, Train::Direction direction, unsigned int maxNumStudents );
};
*/

class TrainStop::PImpl{
    public: 

    // Saved parameters
    Printer & prt;
    NameServer & nameServer;
    unsigned int id;
    unsigned int stopCost;

    // Condition Locks
    uCondition students[2]; 
    uCondition stoppedTrains; 

    // Tracks 
    struct Track 
    {
        bool occupied = false;
        unsigned int waiting = 0;
        Train * train = NULL;
    };
    Track tracks[2];

    unsigned int studentCount[2];

    PImpl( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost )
            : prt(prt), nameServer(nameServer), id(id), stopCost(stopCost) 
    {
        studentCount[0] = 0;
        studentCount[1] = 1;
    }
    ~PImpl()
    {
    }
};

// A TrainStop's function is to act as a sync point between train and students.

TrainStop::TrainStop( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost )
{
    pimpl = new PImpl( prt, nameServer, id, stopCost );
    pimpl->prt.print( Printer::TrainStop , pimpl->id, 'S');

}

TrainStop::~TrainStop()
{
    pimpl->prt.print( Printer::TrainStop , pimpl->id, 'F');
    delete pimpl; // pop!
}

TrainStop::Funds::Funds(unsigned int amt) : amount(amt) {} 

unsigned int TrainStop::getId() const
{
    return pimpl->id;
}

// Student uses card to buy a ticket for numStops stops.
void TrainStop::buy( unsigned int numStops, WATCard & card )
{
    //cout << "TrainStop::buy() called" << endl; // DEBUGGING
    // Student initially calls the buy method, passing in how many stops they are travelling and their card

    unsigned int balance = card.getBalance();
    unsigned int charge = numStops * pimpl->stopCost;

    if (balance < charge)
    {
        // Raise Funds exception, containing the missing amount
        throw Funds(charge - balance);
    }
    else 
    {
        // If there is enough, then the card is debited and marked internally as paid 
        pimpl->prt.print( Printer::TrainStop , pimpl->id, 'B', charge);
        card.withdraw(charge);
        // Note: the card will set POP when withdraw is called
    }
}


// Student waits for a train traveling in the specified direction.
Train * TrainStop::wait( unsigned int studentId, Train::Direction direction )
{  
    //cout << "TrainStop::wait() called by student " << studentId << endl; // DEBUGGING
    // Students call wait, which blocks the student at the stop until train arrives in the correct direction 
    int index = direction == Train::Clockwise ? 0 : 1; // index: 0 - clockwise, 1 - counterclockwise

    pimpl->prt.print( Printer::TrainStop , pimpl->id ,'W', studentId, direction == Train::Clockwise ? '<' : '>');

    //while (pimpl->tracks[index].occupied == false)
    //{
        pimpl->tracks[index].waiting++;
        pimpl->students[index].wait();
        pimpl->tracks[index].waiting--; 
    //    cout << "Student woke, checking if a train exists in the right direction ... " << pimpl->tracks[index].occupied << endl;
    //}
    return pimpl->tracks[index].train;
}

// Student getting off the train at this stop "disembarks" at this stop.				
void TrainStop::disembark( unsigned int studentId )
{
    //cout << "TrainStop::disembark() called" << endl; // DEBUGGING
    pimpl->prt.print( Printer::TrainStop , 'D' , studentId);
}

// Timer notifies stop that timer ticked.	  
void TrainStop::tick()
{
    //cout << "TrainStop::tick() called" << endl; // DEBUGGING
    pimpl->prt.print( Printer::TrainStop , pimpl->id, 't');

    // Timer calls tick to advance the system clock, waking any trains blocked at this stop
    //while (!pimpl->stoppedTrains.empty())
    //    pimpl->stoppedTrains.signal();
}

// Train tells stop in which direction it is traveling, and the maximum number of students it can take.
// It then blocks until the timer ticks over. Returns the minimum of what it can take, and the number
// of students waiting.
unsigned int TrainStop::arrive( unsigned int trainId, Train::Direction direction, unsigned int maxNumStudents )
{
    //cout << "TrainStop::arrive() called" << endl; // DEBUGGING
    //Train calls arrive, TrainStop unblocks the appropriate number of students 

    int index = direction == Train::Clockwise ? 0 : 1; // index: 0 - clockwise, 1 - counterclockwise

    pimpl->prt.print( Printer::TrainStop, pimpl->id, 'A', trainId, maxNumStudents, pimpl->tracks[index].waiting);

    // Update track information
    if (pimpl->tracks[index].train == NULL) pimpl->tracks[index].train = static_cast<Train*>(&uThisTask()); 
    pimpl->tracks[index].occupied = true;

    // Unblock the correct amount of students
    // This is assuming maxNumStudents takes into account students already on the train
    for (unsigned int i = 0; i < maxNumStudents; i++)
    {
        //cout << "Trainstop::arrive(): train " << trainId << " signalling student on track " << index << endl; 
        pimpl->students[index].signal();
    }

    // Train blocks until tick is called, accepts any embarkations
    //pimpl->stoppedTrains.wait();
    _Accept( tick );

    // (ensure you dont return an overflow)
    return pimpl->tracks[index].waiting < maxNumStudents ? pimpl->tracks[index].waiting : maxNumStudents; 
}

void TrainStop::main()
{
    //cout << "TrainStop::main() called" << endl; // DEBUGGING
    // Remember to register into the nameserver!
    pimpl->nameServer.registerStop( pimpl->id );

    while (true)
    {
        try 
        {
            _Accept( buy , wait , arrive , disembark , tick ) {}
            or _Accept( ~TrainStop )
            {
                break;
            }
        }
        catch( uMutexFailure::RendezvousFailure & ) {}
    }
}