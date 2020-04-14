#include "trainstop.h"

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

// A TrainStop's function is to act as a sync point between train and students.

TrainStop::TrainStop( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int stopCost )
        : prt(prt), nameServer(nameServer), id(id), stopCost(stopCost) 
{
    // Initialize arrays
    studentCount[0] = 0;
    studentCount[1] = 1;

    prt.print( Printer::TrainStop , id, 'S');

}

TrainStop::~TrainStop()
{
    prt.print( Printer::TrainStop , id, 'F');
}

TrainStop::Funds::Funds(unsigned int amt) : amount(amt) {} 

unsigned int TrainStop::getId() const
{
    return id;
}

// Student uses card to buy a ticket for numStops stops.
void TrainStop::buy( unsigned int numStops, WATCard & card )
{
    cout << "TrainStop::buy() called" << endl; // DEBUGGING
    // Student initially calls the buy method, passing in how many stops they are travelling and their card

    unsigned int balance = card.getBalance();
    unsigned int charge = numStops * stopCost;

    if (balance < charge)
    {
        // Raise Funds exception, containing the missing amount
        throw Funds(charge - balance);
    }
    else 
    {
        // If there is enough, then the card is debited and marked internally as paid 
        prt.print( Printer::TrainStop , id, 'B', charge);
        card.withdraw(charge);
        // Note: the card will set POP when withdraw is called
    }
}


// Student waits for a train traveling in the specified direction.
Train * TrainStop::wait( unsigned int studentId, Train::Direction direction )
{  
    cout << "TrainStop::wait() called" << endl; // DEBUGGING
    // Students call wait, which blocks the student at the stop until train arrives in the correct direction 
    int index = direction == Train::Clockwise ? 0 : 1; // index: 0 - clockwise, 1 - counterclockwise

    prt.print( Printer::TrainStop , id ,'W', studentId, direction == Train::Clockwise ? '<' : '>');

    while (tracks[index].occupied == false)
    {
        tracks[index].waiting++;
        cout << "wait(): Waiting at track # " << index << " are " << tracks[index].waiting << "students." << endl; // debugging
        students[index].wait();
        tracks[index].waiting--; 
    }
    cout << "wait(): train address: " << tracks[index].train << endl;
    return tracks[index].train;
}

// Student getting off the train at this stop "disembarks" at this stop.				
void TrainStop::disembark( unsigned int studentId )
{
    cout << "TrainStop::disembark() called" << endl; // DEBUGGING
    prt.print( Printer::TrainStop , 'D' , studentId);
}

// Timer notifies stop that timer ticked.	  
void TrainStop::tick()
{
    //cout << "TrainStop::tick() called" << endl; // DEBUGGING
    prt.print( Printer::TrainStop , id, 't');

    // Timer calls tick to advance the system clock, waking any trains blocked at this stop
    //while (!stoppedTrains.empty())
        stoppedTrains.signal();
        stoppedTrains.signal();

    // Tracks are no longer occupied (trains left)
    tracks[0].occupied = false;
    tracks[1].occupied = false;
}

// Train tells stop in which direction it is traveling, and the maximum number of students it can take.
// It then blocks until the timer ticks over. Returns the minimum of what it can take, and the number
// of students waiting.
unsigned int TrainStop::arrive( unsigned int trainId, Train::Direction direction, unsigned int maxNumStudents )
{
    //cout << "TrainStop::arrive() called" << endl; // DEBUGGING

    //Train calls arrive, TrainStop unblocks the appropriate number of students 

    int index = direction == Train::Clockwise ? 0 : 1; // index: 0 - clockwise, 1 - counterclockwise

    prt.print( Printer::TrainStop, id, 'A', trainId, maxNumStudents, tracks[index].waiting);

    // Update track information
    cout << "arrive(): Waiting at track # " << index << " are " << tracks[index].waiting << "students." << endl; // debugging
    if (tracks[index].train == NULL) tracks[index].train = static_cast<Train*>(&uThisTask()); 

    cout << "arrive(): train address: " << &uThisTask() << endl;

    tracks[index].occupied = true;

    // Unblock the correct amount of students
    // This is assuming maxNumStudents takes into account students already on the train
    for (unsigned int i = 0; i < maxNumStudents; i++)
    {
        //cout << "TrainStop::arrive(): SIGNALLING STUDENT!" << endl; //DEBUGGING
        students[index].signal();
    }

    //cout << "TrainStop::arrive(): time to wait!" << endl; //DEBUGGING

    // Train blocks until tick is called
    stoppedTrains.wait(); // deadlock is occuring here somehow

    // (ensure you dont return an overflow)
    return tracks[index].waiting > maxNumStudents ? tracks[index].waiting - maxNumStudents : 0; 
}

void TrainStop::main()
{
    cout << "TrainStop::main() called" << endl; // DEBUGGING
    //cout << "TrainStop::main() started" << endl;
    //cout << "Attempting to register into nameserver..." << endl;
    // Remember to register into the nameserver!
    nameServer.registerStop( id );
    //cout << "Registration success!" << endl;


    while (true)
    {
        _Accept( buy , wait , arrive , disembark , tick ) {}
        or _Accept( ~TrainStop )
        {
            break;
        }
    }
}