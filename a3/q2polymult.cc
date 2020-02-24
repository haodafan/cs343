#include "q2polymult.h"
#include <iostream> 
#include <fstream> 
#include <cmath>

#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

using namespace std;

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta ) {

#if defined( CFOR )
    COFOR( // fill in
		i, 0, delta,
		// this calculates the value for i, i + delta, i + delta*2, i + delta*3, ...
		int curr_it = i; // how do we know how large the polynomials are?
		
		while (curr_it < ((a.size*2) - 1))
		{
            c.coeffs[curr_it] = 0; 
            for (int index = 0; index < curr_it; index++)
            {
                c.coeffs[curr_it] += (a.coeffs[index] * b.coeffs[b.size - 1 - index]);
            }
			curr_it = curr_it + delta; 
		} 
	);
    
#elif defined( ACTOR )

    struct WorkMsg : public uActor::Message {
	// fill in
	WorkMsg() : Message( uActor::Delete ) {}
    };

    _Actor Multiply {
	Allocation receive( Message & w ) {
	    Case( WorkMsg, w ) {			// discriminate derived message
		// ...
	    } else assert( false );			// bad message
	    return Finished;				// one-shot
	} // Multiply::receive
    }; // Multiply

    uActorStart();					// start actor system
    // fill in
    uActorStop();
    
#elif defined( TASK )

    _Task Multiply {
	// fill in
      public:
	Multiply( const poly_t & a, const poly_t & b, poly_t & c,  
		  size_t startIndex, size_t endIndex, const size_t delta );
    };

    // fill in
#else
    #error invalid kind of concurrency system
#endif
    
} // polymultiply

// polyprinter
void polyprint(poly_t& p)
{
	for (int i = p.size - 2; i > 0; i++)
	{
		cout << p.coeffs[i] << "x^" << p.size - i << " + ";
	}
	cout << p.coeffs[p.size - 1] << "x^0" << endl;
}

// main function 
int main( int argc , char * argv[] ) 
{
    // SHELL INTERFACE: 
	// polymult [ -p processors (> 0) ] [ polyfileA polyfileB | -t numCoeff (> 0) ]

	int processors = 1; // default value of 1 
	int argIndex = 1;	
	istream * file_A = &cin;
	istream * file_B = &cin; 
	try 
	{
		// `-p` processors flag
		if (argv[argIndex][0] == '-' && argv && argv[argIndex][1] == 'p')
		{
			argIndex += 1; // next argument
			processors = atoi( argv[argIndex] ); 
			argIndex += 1; // next argument
		}

		poly_t a; 
		poly_t b;

		// `-t` flag
		if (argIndex < argc && argv[argIndex][0] == '-' && argv[argIndex][1] == 't')
		{
			argIndex++; // next argument
			// oh fuck
			
		}
		else 
		{
			// The two files 
			try { file_A = new ifstream(argv[argIndex]); } 
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }
			argIndex++; // next argument
			try { file_B = new ifstream(argv[1]); }
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }

			// this could probably be done concurrently lol 
			int curr_num;
			if (*file_A >> curr_num); 
			a.size = curr_num; 
			a.coeffs = new int[a.size]; // does this count as dynamic allocation? It's heap allocation 
			for (int i = 0; i < a.size; i++)
			{
				*file_A >> curr_num; 
				a.coeffs[i] = curr_num;
			}

			if (*file_B >> curr_num); 
			b.size = curr_num; 
			b.coeffs = new int[a.size]; // does this count as dynamic allocation? It's heap allocation 
			for (int i = 0; i < a.size; i++)
			{
				*file_A >> curr_num; 
				b.coeffs[i] = curr_num;
			}
		}

		// at this point we should have a and b completely filled out 
		poly_t c; 
		c.size = (a.size * 2) - 1;
		c.coeffs = new int[c.size];
		int delta = sqrt(abs(c.size)); // needs to be sqrt(abs(c.size))

		polymultiply(a, b, c, delta);

		polyprint(c);
	}
	catch (...)
	{
		cerr << "ERROR?!" << endl;
	}


}
