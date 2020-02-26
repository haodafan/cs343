#include "q3.h"
#include <iostream> 

using namespace std;

int main( int argc, char * argv[] )
{
    //buffer [ Cons [ Prods [ Produce [ BufferSize [ Delays ] ] ] ] ]
    int num_cons = 5;
    int num_prods = 3;
    int produce = 10;
    int size = 10;
    int delay = -1; 
    int sentinel = SENTINEL;

    try {                                               // process command-line arguments
        switch ( argc ) {
          case 6:
            delay = stoi( argv[5] );
          case 5:
            size = stoi( argv[4] ); 
          case 4:
            produce = stoi( argv[3] ); 
          case 3:
            num_prods = stoi( argv[2] );
          case 2:
            num_cons = stoi( argv[1] );
          case 1:
            break;
          default: 
            throw 1;
        } // switch
    } catch( ... ) {
        cout << "error wrong args" << endl;
        exit( 1 );
    } // try
    if (delay == -1)
        delay = num_cons + num_prods; 

    // Create things
    BoundedBuffer<int> buffy = BoundedBuffer<int>(size);
    Consumer* cons[num_cons];
    Producer* prods[num_prods];
    int sums[num_cons];

    // Initialize sums
    for (int i = 0; i < num_cons; i++)
    {
        sums[i] = 0;
    }

    for (int i = 0; i < num_cons; i++)
    {
        cons[i] = new Consumer(buffy, delay, sentinel, sums[i]);
    }

    for (int i = 0; i < num_prods; i++)
    {
        prods[i] = new Producer(buffy, produce, delay);
    }

    for (int i = 0; i < num_prods; i++)
    {
        delete prods[i];
    }

    // Producers are done -- insert Sentinels
    for (int i = 0; i < num_cons; i++)
    {
        buffy.insert(sentinel);
    }

    for (int i = 0; i < num_cons; i++)
    {
        delete cons[i];
    }


    int total = 0; 
    for (int i = 0; i < num_cons; i++)
        total += sums[i];

    cout << "total: " << total << endl;
}