#include "timer.h"

#include "trainstop.h"
#include "nameserver.h"
#include "printer.h"

/*
#pragma once

#include "global.h"

_Task Timer {
	class PImpl;
	PImpl * pimpl;

    void main();
  public:
    Timer( Printer & prt, NameServer & nameServer, unsigned int timerDelay );
    ~Timer();
};
*/

class Timer::PImpl 
{
    public: 
    Printer & prt;
    NameServer & nameServer;
    unsigned int timerDelay;

    //TrainStop** stops;
    //unsigned int stopCount;
    //unsigned int tickNumber = 0;

    PImpl( Printer & prt, NameServer & nameServer, unsigned int timerDelay ) 
            : prt(prt) , nameServer(nameServer) , timerDelay(timerDelay) 
    {
    }
    ~PImpl() {}
};

Timer::Timer( Printer & prt, NameServer & nameServer, unsigned int timerDelay )
{
    pimpl = new PImpl( prt, nameServer, timerDelay);
    //pimpl->stops = pimpl->nameServer.getStopList();
    //pimpl->stopCount = pimpl->nameServer.getNumStops();
}

Timer::~Timer()
{
    delete pimpl;
}

void Timer::main()
{
    pimpl->prt.print( Printer::Timer , 'S' );
    // A Timer’s function is to keep the simulation clock ticking by advancing the simulation in measured increments.
    // It calls tick on each train stop after having delayed timerDelay times by calling yield.

    TrainStop** stops = pimpl->nameServer.getStopList(); 
    unsigned int numStops = pimpl->nameServer.getNumStops();
    unsigned int tickNumber = 0;

    // Main loop:
    while (true)
    {
        _Accept ( ~Timer ) { break; }
        _Else
        {
            // Delay... 
            yield(pimpl->timerDelay);

            // Call tick...    
            pimpl->prt.print( Printer::Timer , 't', tickNumber );
            for (unsigned int i = 0; i < numStops; i++)
            {
                stops[i]->tick();
            }
            tickNumber++;
        }
    }
    pimpl->prt.print( Printer::Timer , 'F' );
}