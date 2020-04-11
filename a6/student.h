#pragma once
#include "global.h"
#include "train.h"

#include "MPRNG.h"

_Task Student {
    //class PImpl; // *** Replace these two lines with your own implementation.
    //PImpl * pimpl;

    MPRNG mprng;

    // Saved parameters
    Printer & prt;
    NameServer & nameServer;
    WATCardOffice & cardOffice;
    Groupoff & groupoff;

    unsigned int id;
    unsigned int numStops;
    unsigned int stopCost;
    unsigned int maxStudentDelay;
    unsigned int maxStudentTrips;

    // Stored values
    FWATCard giftCard; 
    FWATCard watCard; 
    unsigned int lastStop;


    void main();
  public:
    Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
        unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips , int seed);
    ~Student();
};

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
