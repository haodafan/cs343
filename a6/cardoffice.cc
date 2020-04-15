/*_Task WATCardOffice {
	class PImpl;
	PImpl * pimpl;

	void main();
  public:
	_Event Lost {};										// lost WATCard
	WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers );
	~WATCardOffice();
	WATCard::FWATCard create( unsigned int sid, unsigned int amount ); // called by student
	WATCard::FWATCard transfer( unsigned int sid, unsigned int amount, WATCard * card ); // called by student
	Job * requestWork();								// called by courier to request/return work
}; // WATCardOffice */

#include "cardoffice.h"

#include "bank.h"
#include "watcard.h"
#include "printer.h"
#include <queue>
#include <vector>

#include "MPRNG.h"
extern MPRNG mprng;

#include <iostream>
using namespace std;

struct Job { // marshalled arguments and return future
    unsigned int funds; 
    unsigned int studentId;
    WATCard::FWATCard fwatCard; // return future
    WATCard * watCard;
    Job( unsigned int studentId, unsigned int funds ) : funds( funds ) , studentId( studentId ) 
    { watCard = nullptr; }
    Job( unsigned int studentId, unsigned int funds , WATCard * watCard ) 
    : funds( funds ) , studentId( studentId ) , watCard( watCard ) {}
};

class WATCardOffice::PImpl
{   
    public:

    _Task Courier { 
        unsigned int id;

        Printer & prt;
        Bank & bank;
        WATCardOffice & office;

        public: 
        Courier( unsigned int id, Printer & prt, Bank & bank , WATCardOffice & office ) 
            : id(id) , prt(prt) , bank(bank) , office(office) {} 
        void main(); 
    }; // communicates with bank

    // Saved parameters
    Printer & prt;
    Bank & bank;
    WATCardOffice & office;
    unsigned int numCouriers;

    // Couriers 
    vector<Courier* > couriers;

    // Jobs
    queue<Job *> jobs;

    // Condition lock for couriers
    uCondition bench;

    PImpl( Printer & prt, Bank & bank, WATCardOffice & office, unsigned int numCouriers )
        : prt(prt) , bank(bank) , office( office ), numCouriers(numCouriers) 
    {
    }
    ~PImpl()
    {
    }
};

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers )
{
    pimpl = new PImpl( prt, bank, *this, numCouriers );
}
WATCardOffice::~WATCardOffice()
{
    // Destroy!
    delete pimpl;
}

WATCard::FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount )
{
    // Async call from student
    Job * job = new Job( sid , amount ); // create new work request
    pimpl->jobs.push(job); // add to list of requests
    pimpl->prt.print( Printer::WATCardOffice, 'C' , sid , amount );
    return job->fwatCard; // return future in request
}

WATCard::FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard * card )
{
    Job * job = new Job( sid , amount , card ); // create new work request
    pimpl->jobs.push(job); // add to list of requests
    pimpl->prt.print( Printer::WATCardOffice, 'T' , sid , amount );
    return job->fwatCard; // return future in request
}

Job * WATCardOffice::requestWork()
{
    // Called by Courier
    // If there is a job immediately available, we will perform it. If not, we will block
    if (pimpl->jobs.empty()) pimpl->bench.wait();

    // Upon awake, check if there is a job available.
    if (pimpl->jobs.empty()) return nullptr; // If there is no job, returning nullptr will terminate Courier's main loop

    Job * currentJob = pimpl->jobs.front(); 
    pimpl->jobs.pop(); 
    pimpl->prt.print( Printer::WATCardOffice, 'W' );
    return currentJob; 
}

void WATCardOffice::main() 
{
    pimpl->prt.print( Printer::WATCardOffice , 'S' );

    for (unsigned int i = 0; i < pimpl->numCouriers; i++)
    {
        pimpl->couriers.push_back(new PImpl::Courier(i, pimpl->prt, pimpl->bank, pimpl->office));
    }

    while (true)
    {
        _Accept( ~WATCardOffice ) { break; }
        or _Accept( transfer , create )
        {
            pimpl->bench.signalBlock(); // If transfer or create was called, signal a waiting courier
        }
        or _Accept( requestWork )
        {
            yield();
        }
        _Else {} 
    }

    // Destructor called, we need to wake all couriers and terminate them
    while (!pimpl->jobs.empty()) pimpl->jobs.pop();
    while (!pimpl->bench.empty()) pimpl->bench.signalBlock();
    for (unsigned int i = 0; i < pimpl->numCouriers; i++)
    {
        delete pimpl->couriers[i];
    }

    pimpl->prt.print( Printer::WATCardOffice , 'F' );
}

void WATCardOffice::PImpl::Courier::main()
{
    prt.print( Printer::WATCardOfficeCourier , id ,'S' );

    while (true)
    {
        Job * currentJob = office.requestWork();
        if (currentJob) 
        {
            // 1 in 6 chance of courier losing watcard
            unsigned int random = mprng(5);
            if (random == 5)
            {
                // When WATCard is lost, except Lost is inserted into the future, current WATCard is deleted 
                if (currentJob->watCard)
                {
                    prt.print( Printer::WATCardOfficeCourier , id , 'L' , currentJob->studentId );
                    delete currentJob->fwatCard;
                    currentJob->fwatCard.exception( new Lost{} );
                }
                else 
                {
                    prt.print( Printer::WATCardOfficeCourier , id , 'L' , currentJob->studentId );
                    delete currentJob->fwatCard;
                    currentJob->fwatCard.exception( new Lost{} );
                }
            }
            // If the courier didn't lose it, then we will complete the operation
            else if( currentJob->watCard )
            {
                // if the current job's watcard is a REAL WATCard, then this is a money TRANSFER
                prt.print( Printer::WATCardOfficeCourier , id , 't' , currentJob->studentId , currentJob->funds );
                bank.withdraw(currentJob->funds,  currentJob->studentId);
                currentJob->watCard->deposit(currentJob->funds); 
                currentJob->fwatCard.delivery( currentJob->watCard );
                prt.print( Printer::WATCardOfficeCourier , id , 'T' , currentJob->studentId );
            }
            else
            {
                // if the current job's result is NOT REAL, then this is a card CREATION 
                prt.print( Printer::WATCardOfficeCourier , id , 't' , currentJob->studentId , currentJob->funds );
                WATCard * newWatCard = new WATCard();
                bank.withdraw(currentJob->funds, currentJob->studentId);
                newWatCard->deposit(currentJob->funds);
                currentJob->fwatCard.delivery( newWatCard );
                prt.print( Printer::WATCardOfficeCourier , id , 'T' , currentJob->studentId , currentJob->funds );
            }

            delete currentJob;
        } // if (currentJob)
        else 
        {
            break; // If there is no current job, so we terminate the courier's main loop
        }
    }

    prt.print( Printer::WATCardOfficeCourier , id , 'F' );
}