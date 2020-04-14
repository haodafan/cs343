#include "conductor.h"

#include "train.h"
#include "printer.h"

/*
_Task Conductor {
	class PImpl;
	PImpl * pimpl;

	void main();
  public:
  	Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay );
  	~Conductor();
};
*/

class Conductor::PImpl
{
    public:
    Printer & prt;
    unsigned int id;
    Train * train;
    unsigned int delay;
    PImpl( Printer & prt, unsigned int id, Train * train, unsigned int delay )
        : prt(prt) , id(id) , train(train) , delay(delay) {}
  	~PImpl() {}
};

Conductor::Conductor( Printer & prt, unsigned int id, Train * train, unsigned int delay )
{
    pimpl = new PImpl(prt, id, train, delay);
}
Conductor::~Conductor()
{
    delete pimpl;
}

void Conductor::main()
{
    pimpl->prt.print( Printer::Conductor , pimpl->id , 'S');
    //main loop consists of yielding conductorDelay times, then calling the train's scanPassengers method
    while (true)
    {
        _Accept( ~Conductor )
        {
            break;
        }
        _Else
        {
            yield(pimpl->delay);
            pimpl->prt.print( Printer::Conductor , pimpl->id , 'c');
            pimpl->train->scanPassengers();
        }
    }
    pimpl->prt.print( Printer::Conductor , pimpl->id , 'F');
}