#pragma once

#include "watcard.h"
#include "printer.h"

_Task Groupoff {
	//class PImpl;
	//PImpl * pimpl;

	// Save parameters
	Printer & prt;
	unsigned int numStudents;
	unsigned int maxTripCost;
	unsigned int groupoffDelay;

	unsigned int numRequests = 0; // Used to keep track of requests

	enum WorkStatus { Unrequested, Requested, Completed };

	// Work : Used to keep track of future giftcards 
	//        Each piece of work contains a status, which identifies if it has been requested or finished 
	struct Work 
	{
		WorkStatus status;
		WATCard::FWATCard card; 
		Work() { status = WorkStatus::Unrequested; }
	};

	Work* requests; // An array of size numStudents, **assuming each student requests once**

	unsigned int getRandomizedRequestedWorkIndex(int numStudentsLeft);
	
	void main();
  public:
	Groupoff( Printer & prt, unsigned int numStudents, unsigned int maxTripCost, unsigned int groupoffDelay );
	~Groupoff();
	WATCard::FWATCard giftCard();
}; // Groupoff

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
