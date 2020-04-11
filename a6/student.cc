#include "student.h"

#include <cstdlib>

using namespace std;

// PUBLIC FUNCTIONS =====================================================================

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
    unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips, int seed )
    : prt(prt), nameServer(nameServer), cardOffice(cardOffice), groupoff(groupoff), id(id), numStops(numStops), stopCost(stopCost),
    maxStudentDelay(maxStudentDelay), maxStudentTrips(maxStudentTrips), seed(seed)
{

    mprng.set_seed(seed);

    // Set stored values
    watCard = cardOffice.create(id, (numStops / 2) * stopCost); 
    giftCard = groupoff.giftcard(); 

    lastStop = numStops + 1;
}
Student::~Student()
{

}

void Student::main()
{
    // A student's function is to make a random number of trips (in the range [1, maxStudentTrips])
    unsigned int numTrips = mprng(maxStudentTrips);

    // Go on trips! 
    for (unsigned int i = 0; i < numTrips; i++)
    {
        // Before each trip, student yields [0, maxStudentDelay] times 
        unsigned numYields = mprng(maxStudentDelay);
        yield(numYields);

        int nextStop;
        do 
        {
            // First trip is between two random stops 
            if (lastStop > numStops) lastStop = mprng(numStops);

            nextStop = mprng(numStops);
        } while (lastStop == nextStop)

        // Student then obtains the location of the starting stop from name server
        TrainStop tsLast = nameServer.getStop(id, lastStop);
        TrainStop tsNext = nameServer.getStop(id, nextStop);

        char direction = '<'; // counterclockwise
        int distance = (nextStop - lastStop) % numStops;
        if (((lastStop - nextStop) % numStops) >= ((nextStop - lastStop) % numStops))
        {
            direction = '>'
            distance = (lastStop - nextStop) % numStops;
        }
        

        // Student first decides whether or not he should skip paying ... 
        int threshold = 3;
        if (nextStop - lastStop == 1 || lastStop - nextStop == 1)
        {
            threshold = 5;
        }
        int dice = mprng(9);
        if (dice > threshold)
        {
            // Student decides to pay       

            // If the giftcard has enough to pay
            _Select ( giftCard || watCard )
            {
                
            }
            
        }
        else 
        {
            // Student decides to skip

        }
    }
}