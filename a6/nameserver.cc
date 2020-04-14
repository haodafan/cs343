#include "nameserver.h"

#include "global.h"
#include "printer.h"
#include "trainstop.h"

/*
_Task NameServer {
	class PImpl;
	PImpl * pimpl;
	
	void main();
  public:
	NameServer( Printer & prt, unsigned int numStops, unsigned int numStudents );
	~NameServer();
	void registerStop( unsigned int trainStopId );
	TrainStop * getStop( unsigned int studentId, unsigned int trainStopId );
	TrainStop ** getStopList();  // called by Timer
	TrainStop ** getStopList( unsigned int trainId );	// called by Train
	unsigned int getNumStops();
};
*/

class NameServer::PImpl
{   
    public: 
    TrainStop ** stopList; //array of pointers

    Printer & prt;
    unsigned int numStops; 
    unsigned int numStudents;

    PImpl( Printer & prt, unsigned int numStops, unsigned int numStudents )
            : prt(prt) , numStops(numStops), numStudents(numStudents)
    {
        stopList = new TrainStop*[numStops];
    }
    ~PImpl()
    {
        delete [] stopList;
    }
};

NameServer::NameServer( Printer & prt, unsigned int numStops, unsigned int numStudents )
{
    pimpl = new PImpl(prt, numStops, numStudents);
}

NameServer::~NameServer()
{
    pimpl->prt.print( Printer::NameServer, 'F' );
    delete pimpl; // i wish i could delete my pimpls irl
}

void NameServer::main()
{
    pimpl->prt.print( Printer::NameServer, 'S' );

    // Each Trainstop MUST register itself upon creation, 
    // the SIMULATION CANNOT START UNTIL THIS HAPPENS
    for (unsigned int i = 0; i < pimpl->numStops; i++)
    {
        _Accept( registerStop ) {} 
        or _Accept ( ~NameServer )
        {
            break;
        }
    }
    // The simulation may begin ... nameserver stops running upon deletion
    while (true)
    {
        _Accept ( ~NameServer )
        {
            break;
        }
        or _Accept ( getStop , getStopList , getNumStops ) {}
    }
}

// PUBLIC FUNCTIONS =============================================================================

void NameServer::registerStop( unsigned int trainStopId )
{
    pimpl->prt.print( Printer::NameServer, 'R', trainStopId );
    pimpl->stopList[trainStopId] = static_cast<TrainStop*>(&uThisTask()); // from piazza @371
}

TrainStop * NameServer::getStop( unsigned int studentId, unsigned int trainStopId )
{
    pimpl->prt.print( Printer::NameServer, 'T', studentId, trainStopId );
    return pimpl->stopList[trainStopId]; 
}

TrainStop ** NameServer::getStopList()
{
    //called by timer
    pimpl->prt.print( Printer::NameServer, 'L' );
    return pimpl->stopList;
}

TrainStop ** NameServer::getStopList( unsigned int trainId )
{
    //called by train
    pimpl->prt.print( Printer::NameServer, 'L' , trainId );
    return pimpl->stopList;
}

unsigned int NameServer::getNumStops()
{
    return pimpl->numStops;
}