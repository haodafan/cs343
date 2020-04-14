#pragma once
#include "global.h"
#include "train.h"

#include "printer.h"
#include "cardoffice.h"
#include "train.h"
#include "trainstop.h"
#include "groupoff.h"
#include "nameserver.h"

_Task Student {
    class PImpl; // *** Replace these two lines with your own implementation.
    PImpl * pimpl;

    void main();
  public:
    Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
        unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips );
    ~Student();
};

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
