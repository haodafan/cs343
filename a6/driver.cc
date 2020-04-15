// PROGRAM MAIN
#include "MPRNG.h"

//MPRNG mprng;

// Include everything?
#include "bank.h"
#include "cardoffice.h"
#include "conductor.h"
#include "config.h"
#include "global.h"
#include "groupoff.h"
#include "nameserver.h"
#include "parent.h"
#include "printer.h"
#include "student.h"
#include "timer.h"
#include "train.h"
#include "trainstop.h"
#include "watcard.h"


// Other libraries
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    //lrt [ config-file [ Seed ] ]
    const char* filename = "lrt.config";
    int seed = getpid();

    unsigned int numTrains = 2; // 0 => clockwise, 1 => counterclockwise

    try 
    {
        switch (argc)
        {
            case 3: seed = stoi(argv[2]);
            case 2: filename = argv[1];
            case 1: break;
        }
    }
    catch (...)
    {
        cerr << "Invalid input! Format required: lrt [ config-file [ Seed ] ]" << endl; 
        return 1;
    }

    // The program main starts by calling processConfigFile to read and parse the simulation configurations
    ConfigParms cparms;
    processConfigFile( filename, cparms );

    // Important values
    unsigned int maxTripCost = cparms.stopCost * (cparms.numStops / 2);
    mprng.set_seed(seed);

    // It then creates in order the:
    // printer, bank, WATCard office, groupoff, parent, name server, timer, train stops, trains, and students.
    Printer prt = Printer( cparms.numStudents, numTrains, cparms.numStops, cparms.numCouriers ); 
    Bank bank = Bank( cparms.numStudents ); 
    WATCardOffice cardOffice = WATCardOffice( prt, bank, cparms.numCouriers );
    Groupoff groupoff = Groupoff( prt, cparms.numStudents, maxTripCost, cparms.groupoffDelay );
    Parent parent = Parent( prt, bank, cparms.numStudents, cparms.parentalDelay, maxTripCost );
    NameServer nameServer = NameServer( prt, cparms.numStops, cparms.numStudents ); 
    Timer * timer = new Timer( prt, nameServer, cparms.timerDelay ); 

    TrainStop * trainStops[cparms.numStops]; 
    for (unsigned int i = 0; i < cparms.numStops; i++)
        trainStops[i] = new TrainStop( prt, nameServer, i, cparms.stopCost );

    Train * trains[numTrains]; 
    for (unsigned int i = 0; i < numTrains; i++)
        trains[i] = new Train( prt, nameServer, i, cparms.numStudents, cparms.numStops );

    Conductor * conductors[numTrains];
    for (unsigned int i = 0; i < numTrains; i++)
        conductors[i] = new Conductor( prt, i, trains[i], cparms.conductorDelay );


    Student * students[cparms.numStudents];
    for (unsigned int i = 0; i < cparms.numStudents; i++)
        students[i] = new Student( prt, nameServer, cardOffice, groupoff, i, cparms.numStops, cparms.stopCost,
                                cparms.maxStudentDelay, cparms.maxStudentTrips );

    // The entire simulation must shut down in an orderly fashion once all of the students have completed their trips

    // DELETE EVERYTHING IN OPPOSITE ORDER OF CREATION

    for (unsigned int i = 0; i < cparms.numStudents; i++)
        delete students[i]; // Wait for students to finish trips
    
    for (unsigned int i = 0; i < numTrains; i++)
        delete conductors[i]; // Wait for conductors to finish

    for (unsigned int i = 0; i < numTrains; i++)
        delete trains[i]; // Wait for trains to finish
        
    delete timer; 
    
    for (unsigned int i = 0; i < cparms.numStops; i++)
        delete trainStops[i]; // Wait for trainstops to finish

    return 0;
}