#include "train.h"

#include "global.h"
#include "nameserver.h"
#include "trainstop.h"
#include "printer.h"

#include <iostream>
using namespace std; // debugging

/*
_Task Train {
  public:
	enum Direction { Clockwise, CounterClockwise };
	_Event Ejected {};						// Exception raised at student without ticket

  private:
  	class PImpl;
  	PImpl *pimpl;

	void main();
  public:
	Train( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops );
	~Train();
	_Nomutex unsigned int getId() const;
	TrainStop * embark( unsigned int studentId, unsigned int destStop, WATCard& card ); // Student waits until train reaches destination stop.
	void scanPassengers(); // called by conductor
};
*/

// PIMPL IDIOM
class Train::PImpl
{   
    public: 
	struct Passenger 
	{
		bool occupied = false;
		bool ejected = false;
		unsigned int studentId;
		unsigned int destStop;
		WATCard* card;
		uCondition student;

		Passenger() {} 
		~Passenger() {}
	};

	unsigned int findSeat(bool occupied)
	{
		for (unsigned int i = 0; i < maxNumStudents; i++)
		{
			if (seats[i].occupied == occupied)
			{
				return i;
			}
		}
		return NULL; // No available seats
	}

	unsigned int countSeats(bool occupied)
	{
		unsigned int acc = 0;
		for (unsigned int i = 0; i < maxNumStudents; i++)
		{
			if (seats[i].occupied == occupied)
			{
				acc++;
			}
		}
		return acc;
	}

	// Saved data
	Passenger * seats;

	// Saved parameters
	Printer & prt;
	NameServer & nameServer;
	unsigned int id;
	unsigned int maxNumStudents;
	unsigned int numStops;

    PImpl( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops )
            : prt(prt), nameServer(nameServer), id(id), maxNumStudents(maxNumStudents), numStops(numStops)
    {
		seats = new Passenger[maxNumStudents];
    }
    ~PImpl()
    {
		delete [] seats;
    }
};

Train::Train( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int maxNumStudents, unsigned int numStops )
{
	pimpl = new PImpl( prt, nameServer, id, maxNumStudents, numStops );
}

Train::~Train()
{
	delete pimpl;
}

_Nomutex unsigned int Train::getId() const
{
	return pimpl->id;
}

// Student waits until train reaches destination stop.
TrainStop * Train::embark( unsigned int studentId, unsigned int destStop, WATCard& card )
{
	//cout << "Train::embark(): Student " << studentId << " travelling to " << destStop << endl;
	
	// Assuming every student calling embark should have a seat available for them 
	unsigned int seatNumber = pimpl->findSeat(false); // find empty seat

	// Fill information
	pimpl->seats[seatNumber].occupied = true;
	pimpl->seats[seatNumber].studentId = studentId;
	pimpl->seats[seatNumber].destStop = destStop;
	pimpl->seats[seatNumber].card = &card;
	pimpl->seats[seatNumber].ejected = false; // We're not ejected yet ;)

	pimpl->prt.print( Printer::Train , pimpl->id, 'E', studentId, destStop );

	//cout << "Train::embark(): Student sitting at seat number " << seatNumber << endl; //DEBUGGING
	// Sleep! 
	pimpl->seats[seatNumber].student.wait();

	//cout << "Train::embark(): STUDENT WOKEN!!" << endl;

	// Check if we've been ejected :(
	if (pimpl->seats[seatNumber].ejected)
	{
		pimpl->seats[seatNumber].occupied = false;
		throw Ejected(); 
	}
	return pimpl->nameServer.getStopList()[destStop]; // ???? What are we supposed to return
}

// called by conductor
void Train::scanPassengers()
{
	for (unsigned int i = 0; i < pimpl->maxNumStudents; i++)
	{
		if (pimpl->seats[i].occupied)
		{
			// EJECTED!!!!!!!!!!!!!!!!!!!!!!!!!!
			pimpl->seats[i].ejected = true;
			pimpl->prt.print( Printer::Conductor , pimpl->id , 'E' , pimpl->seats[i].studentId );
			pimpl->seats[i].student.signal(); // WAKE UP AND GTFO 
		}
	}
}

void Train::main() 
{

	// Train's function is to first determine its direction of travel where train0 travels clockwise, 1 counterclockwise
	Direction dir = pimpl->id == 0 ? Clockwise : CounterClockwise;

	// MAIN LOOP 
	unsigned int currentStop = pimpl->id == 0 ? 0 : (pimpl->numStops / 2);
	TrainStop ** stops = pimpl->nameServer.getStopList(pimpl->id);

	pimpl->prt.print( Printer::Train , pimpl->id, 'S' , currentStop, pimpl->id == 0 ? '<' : '>');

	while (true)
	{
		_Accept( ~Train )
		{
			break;
		}
		or _Accept( scanPassengers ) {}
		_Else 
		{
			// At each stop ... 

			// Unblocks students who have reached their destination 
			for (unsigned int i = 0; i < pimpl->maxNumStudents; i++)
			{
				if (pimpl->seats[i].occupied && pimpl->seats[i].destStop == currentStop)
				{					
					// Remove student from premises! 
					pimpl->seats[i].occupied = false;

					//cout << "Train::main(): Student " << pimpl->seats[i].studentId << " in seat "
					//	<< i << " reached destination " << currentStop << endl;
					pimpl->seats[i].student.signalBlock(); // signal student to leave

				}
			}

			// At every stop it takes on waiting passengers through embark until it reaches its limit ... 
			pimpl->prt.print( Printer::Train , pimpl->id, 'A', currentStop, pimpl->countSeats(false), pimpl->countSeats(true) );

			unsigned int numStudents = stops[currentStop]->arrive( pimpl->id, dir, pimpl->countSeats(false) );
			//cout << "Train::main(): number of students waiting = " << numStudents << endl;
			for (unsigned int i = 0; i < numStudents; i++)
			{
				//cout << "Train::main(): Waiting for embark....." << endl;
				_Accept ( embark ) {} 
				or _Accept( ~Train ) { break; }
			}

			//cout << "Train::main(): Arrive awoken, currently carrying: " << pimpl->countSeats(true) << " passengers!" << endl; //DEBUGGING

			// Now, let's go to the next stop!
			if (dir == Clockwise)
			{
				currentStop++; 
				if (currentStop == pimpl->numStops) currentStop = 0;
			}
			else 
			{
				currentStop--; 
				if (currentStop == UINT_MAX) currentStop = (pimpl->numStops - 1);
			}
		}
	}

	pimpl->prt.print( Printer::Train , pimpl->id, 'F' );
}