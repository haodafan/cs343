#include "student.h"

#include "MPRNG.h"
#include "global.h"

#include <cstdlib>
#include <iostream>

extern MPRNG mprng;

using namespace std;

/*

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
    WATCard::FWATCard giftCard; 
    WATCard::FWATCard watCard; 
    unsigned int lastStop;
    unsigned int maxTripCost;
*/

class Student::PImpl 
{
    public:
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
    WATCard::FWATCard giftCard; 
    WATCard::FWATCard watCard; 
    unsigned int lastStop;
    unsigned int maxTripCost;

    PImpl( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
        unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips )
        : prt(prt), nameServer(nameServer), cardOffice(cardOffice), groupoff(groupoff), id(id), numStops(numStops), stopCost(stopCost),
        maxStudentDelay(maxStudentDelay), maxStudentTrips(maxStudentTrips)
    {
    // Set stored values
    maxTripCost = (numStops / 2) * stopCost;
    watCard = cardOffice.create(id, maxTripCost); 
    giftCard = groupoff.giftCard(); 

    lastStop = numStops + 1;
    }

    ~PImpl()
    {
        try 
        {
            if (watCard.available()) delete watCard; 
        }
        catch (WATCardOffice::Lost &L) {} 
    }

};

// PUBLIC FUNCTIONS =====================================================================

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff, 
    unsigned int id, unsigned int numStops, unsigned int stopCost, unsigned int maxStudentDelay, unsigned int maxStudentTrips )
{
    pimpl = new PImpl( prt, nameServer, cardOffice, groupoff, id, numStops, stopCost, maxStudentDelay, maxStudentTrips );
}
Student::~Student()
{
    delete pimpl;
}

void Student::main()
{
    // A student's function is to make a random number of trips (in the range [1, maxStudentTrips])
    unsigned int numTrips = mprng(pimpl->maxStudentTrips);

    pimpl->prt.print(Printer::Student, pimpl->id, 'S', numTrips);

    // Go on trips! 
    for (unsigned int i = 0; i < numTrips; i++)
    {
        // Before each trip, student yields [0, maxStudentDelay] times 
        unsigned int numYields = mprng(pimpl->maxStudentDelay);
        yield(numYields);

        unsigned int nextStop;
        do 
        {
            // First trip is between two random stops 
            if (pimpl->lastStop >= pimpl->numStops) pimpl->lastStop = mprng(pimpl->numStops - 1);

            nextStop = mprng(pimpl->numStops - 1);
        } while (pimpl->lastStop == nextStop);

        // Student then obtains the location of the starting stop from name server
        TrainStop * tsLast = pimpl->nameServer.getStop(pimpl->id, pimpl->lastStop);
        TrainStop * tsNext = pimpl->nameServer.getStop(pimpl->id, nextStop);

        // Determine direction... < is clockwise and > is counter-clockwise
        char direction = '>'; // starts off counter-clockwise
        int distance = (pimpl->lastStop - nextStop) % (pimpl->numStops / 2);

        if (((pimpl->lastStop - nextStop) % pimpl->numStops) >= (pimpl->numStops / 2))
        {
            // laststop - nexstop is always large number or negative if clockwise... 
            distance = (nextStop - pimpl->lastStop) % (pimpl->numStops / 2); // since in other dir, 
            direction = '<'; // clockwise
        }

        // rando edge case
        if (distance == 0) distance = 1;

        // Student first decides whether or not he should skip paying ... 
        bool giftCardUsed = false;
        int threshold = 3;
        if ((nextStop - pimpl->lastStop) % pimpl->numStops == 1 || (pimpl->lastStop - nextStop) % pimpl->numStops == 1)
        {
            threshold = 5;
        }
        int dice = mprng(9);
        if (dice > threshold)
        {
            // Student decides to pay       
            AttemptPay: // is this how labels work? 
            _Select ( pimpl->giftCard )
            {
                try 
                {
                    //cout <<"Student" << id << " Buying with giftcard" << endl; //DEBUGGING
                    tsLast->buy(distance, *pimpl->giftCard());
                    pimpl->prt.print(Printer::Student, pimpl->id, 'G', distance*pimpl->stopCost, pimpl->giftCard()->getBalance());
                    giftCardUsed = true;
                }
                catch (TrainStop::Funds &f)
                {
                    //cout <<"Student " << id << " giftcard insufficient funds " << endl; //DEBUGGING
                    // "Reset" the giftcard's future, making it empty again 
                    pimpl->giftCard.reset();
                    goto AttemptPay; // Pay without the gift card
                }
            }
            or _Select ( pimpl->watCard )
            {
                try
                {
                    //cout <<"Student" << id << " Buying with WATCard" << endl; //DEBUGGING
                    tsLast->buy(distance, *pimpl->watCard());
                    pimpl->prt.print(Printer::Student, pimpl->id, 'B', distance*pimpl->stopCost, pimpl->watCard()->getBalance());
                }
                catch (TrainStop::Funds &f)
                {
                    //cout <<"Student " << id << " watcard insufficient funds " << endl; //DEBUGGING
                    // A request is made for the missing amount + initialization 
                    int missing = f.amount + pimpl->maxTripCost;
                    pimpl->cardOffice.transfer( pimpl->id, missing, pimpl->watCard );
                    goto AttemptPay; // Try again with the refilled WATCard
                }
                // Watcard is lost? 
                catch (WATCardOffice::Lost &L)
                {
                    //cout <<"WATCARD LOST!" << endl;
                    pimpl->prt.print(Printer::Student, pimpl->id, 'L');

                    // Request replacement 
                    pimpl->watCard = pimpl->cardOffice.create(pimpl->id, pimpl->maxTripCost);
                    goto AttemptPay;
                }
            }
            //_Else 
            //{
            //    // If we have neither a giftcard nor a watcard available, then we need to wait until one of them becomes available
            //
            //}
            
        } // if
        else 
        {
            // Student decides to skip
            pimpl->prt.print(Printer::Student, pimpl->id, 'f');
        } // else

        // Student then waits for the train and embarks! 
        pimpl->prt.print(Printer::Student, pimpl->id, 'W', pimpl->lastStop);

        Train::Direction trainDir = direction == '<' ? Train::Clockwise : Train::CounterClockwise;

        //cout <<"Student " << id << " waiting at train stop: " << tsLast->getId() << endl; //DEBUGGING

        Train * t = tsLast->wait( pimpl->id, trainDir );

        //cout <<"Student " << id << " Done waiting! " << endl; //DEBUGGING

        attemptEmbark:
        try 
        {
            pimpl->prt.print(Printer::Student, pimpl->id, 'E', t->getId());

            // DEBUGGING
            if (giftCardUsed) 
            {
                bool paid = pimpl->giftCard()->paidForTicket();
                //cout <<"GiftCard exists!" << endl;
            }
            else 
            {
                //cout <<"CHECK IF WATCARD EXISTS" << endl; // DEBUGGING
                bool paid = pimpl->watCard()->paidForTicket();
                //cout <<"WATCard exists!" << endl;
            }
            // /DEBUGGING

            //cout << "student(): Embarking on train at address: " << t << endl;
            tsNext = t->embark( pimpl->id, nextStop, giftCardUsed ? *(pimpl->giftCard()) : *(pimpl->watCard()) ); // Embark with our giftcard if we used it
            //cout << "student(): Successful embarkation" << endl;
        }
        catch (Train::Ejected &e)
        {
            // RIP!!! :( 
            pimpl->prt.print(Printer::Student, pimpl->id, 'e');
            break;
        }
        catch (WATCardOffice::Lost &L)
        {
            pimpl->prt.print(Printer::Student, pimpl->id, 'L');
            //cout <<"WATCARD LOST AT EMBARKATION" << endl;

            // Request replacement 
            pimpl->watCard = pimpl->cardOffice.create(pimpl->id, pimpl->maxTripCost);
            goto attemptEmbark; // start the process again???? 
        }

        // Disembark
        //cout << "student(): Attempting to disembark..." << endl;
        pimpl->prt.print(Printer::Student, pimpl->id, 'D', nextStop);
        tsNext->disembark(pimpl->id);

        // Reset proof of payment
        try 
        {
            if (giftCardUsed) pimpl->giftCard()->resetPOP(); 
            else pimpl->watCard()->resetPOP(); //can u lose the watcard at this stage??? 
        }
        catch (WATCardOffice::Lost &L)
        {
            pimpl->prt.print(Printer::Student, pimpl->id, 'L');
            //cout <<"WATCARD LOST AT EMBARKATION" << endl;

            // Request replacement 
            pimpl->watCard = pimpl->cardOffice.create(pimpl->id, pimpl->maxTripCost);
        }
 
        // Proceed to next stop! 
        pimpl->lastStop = nextStop;
    } // for (trips)

    // We're finished!
    pimpl->prt.print(Printer::Student, pimpl->id, 'F');
}