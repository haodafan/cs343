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

class WATCardOffice::PImpl
{
    struct Job { // marshalled arguments and return future
        Args args; // call arguments (YOU DEFINE “Args”)
        WATCard::FWATCard result; // return future
        Job( Args args ) : args( args ) {}
    };
    _Task Courier { 
        
    }; // communicates with bank

    // Saved parameters
    Printer & prt;
    Bank & bank;
    unsigned int numCouriers;

    PImpl( Printer & prt, Bank & bank, unsigned int numCouriers )
    {

    }
    ~PImpl()
    {
        
    }
};

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers )
{

}
WATCardOffice::~WATCardOffice();