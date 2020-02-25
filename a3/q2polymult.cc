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
		
		while (curr_it <= ((a.size*2) - 1))
		{
            c.coeffs[curr_it] = 0; 
            for (int index = 0; index <= curr_it; index++) // Coefficient calculation
            {
				if (index < a.size && curr_it - index < a.size)
                	c.coeffs[curr_it] += (a.coeffs[index] * b.coeffs[curr_it - index]);
            }
			curr_it += delta; 
		} 
	);
    
#elif defined( ACTOR )

    struct WorkMsg : public uActor::Message {
		poly_t a, b;
		int co_i; 
		int& coeff; 

	WorkMsg(poly_t a, poly_t b, int co_i, int& coeff) : Message( uActor::Delete ), a(a), b(b), co_i(co_i), coeff(coeff) {}
    };

    _Actor Multiply {
		int id; //DEBUGGING
		Allocation receive( Message & w ) {
			Case( WorkMsg, w ) {			// discriminate derived message
				w_d->coeff = 0; 
				for (int index = 0; index <= w_d->co_i; index++)
				{
					if (index < w_d->a.size && w_d->co_i - index < w_d->a.size)
						w_d->coeff += w_d->a.coeffs[index] * w_d->b.coeffs[w_d->co_i - index];
				}
				//cout << "returning Nodelete from coefficient index " << w_d->co_i << " and Actor id " << id << endl; //debugging
			} else Case ( StopMsg, w ) {
				//cout << "return Delete from id " << id << endl; //debugging
				return Delete;
			} else assert( false );			// bad message
			return Nodelete;				// one-shot
		} // Multiply::receive
		public:
		Multiply(){}
		Multiply(int id) : id(id) {}
    }; // Multiply

    uActorStart();					// start actor system
	Multiply* a_mult[delta];
	for (int i = 0; i < delta; i++) a_mult[i] = new Multiply(i);
	//try 
	//{
		int offset = 0;
		// a non-flag way to break from both loops
		while (true)
		{
			for (int i = 0; i < delta; i++)
			{
				if (i + offset >= c.size)
					break; // exit calculations 

				//WorkMsg work(a, b, i + offset, c.coeffs[i+offset]);
				//cout << "working on: " << i+offset <<  endl;
				*a_mult[i] | *new WorkMsg(a, b, i+offset, c.coeffs[i+offset]); 
			}
			offset += delta; 
			if (offset > c.size)
				break; // exit calculations
		}
	//}
	//catch (...) 
	//{
		for (int i = 0; i < delta; i++)
			*a_mult[i] | uActor::stopMsg;
	//} 
    uActorStop();
    
#elif defined( TASK )

    _Task Multiply {
		poly_t a, b, c; 
		size_t startIndex, endIndex, delta;

		void main()
		{
			int i = startIndex; 
			while (true)
			{
				if (i >= endIndex)
					break; // we reached the end of this task's loop
				
				// calculation
				c.coeffs[i] = 0;
				for (int index = 0; index <= i; index++)
				{
					if (index < a.size && i - index < a.size)
						c.coeffs[i] += a.coeffs[index] * b.coeffs[i - index];
				}

				i += delta; 
			}
		}

      public:
	Multiply( const poly_t & a, const poly_t & b, poly_t & c,  
		  size_t startIndex, size_t endIndex, const size_t delta ) : 
		  a(a), b(b), c(c), startIndex(startIndex), endIndex(endIndex), delta(delta) {} 

    };

    // fill in
	
	Multiply * t_mults[delta]; 
	for (int i = 0; i < delta; i++)
	{
		t_mults[i] = new Multiply(a,b,c,i,c.size,delta);
	}
	for (int i = 0; i < delta; i++) // wait for tasks to finish
	{
		delete t_mults[i];
	}

#else
    #error invalid kind of concurrency system
#endif
    
} // polymultiply

// polyprinter
void polyprint(poly_t& p)
{
	for (int i = p.size - 1; i > 0; i--)
	{
		if (p.coeffs[i] != 0)
			cout << p.coeffs[i] << "x^" << i << " + ";
	}
	cout << p.coeffs[0] << "x^0" << endl;
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

	bool printout = true; 

	try 
	{
		// `-p` processors flag
		if (argv[argIndex][0] == '-' && argv && argv[argIndex][1] == 'p')
		{
			argIndex += 1; // next argument
			processors = atoi( argv[argIndex] ); 

			if (processors > 1)
				uProcessor p[processors - 1];

			argIndex += 1; // next argument
		}

		poly_t a; 
		poly_t b;

		// `-t` flag
		if (argIndex < argc && argv[argIndex][0] == '-' && argv[argIndex][1] == 't')
		{
			argIndex++; 
			printout = false; 
			int numCoeffs = atoi( argv[argIndex] );

			a.size = numCoeffs; 
			b.size = numCoeffs; 
			a.coeffs = new int[a.size];
			b.coeffs = new int[b.size];

			for (int i = 0; i < numCoeffs; i++)
			{
				a.coeffs[i] = i;
				b.coeffs[i] = i;
			}
		}
		else 
		{
			// The two files 
			try { file_A = new ifstream(argv[argIndex]); } 
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }
			argIndex++; // next argument
			try { file_B = new ifstream(argv[argIndex]); }
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }

			// this could probably be done concurrently lol 
			int curr_num;
			if (!(*file_A >> curr_num)) return 1; 
			a.size = curr_num; 
			a.coeffs = new int[a.size]; // does this count as dynamic allocation? It's heap allocation 
			//for (int i = a.size - 1; i >= 0; i--) // is this backwards?
			for (int i = 0; i < a.size; i++)
			{
				*file_A >> curr_num; 
				a.coeffs[i] = curr_num;
			}

			if (!(*file_B >> curr_num)) return 1; 
			b.size = curr_num; 
			b.coeffs = new int[b.size]; // does this count as dynamic allocation? It's heap allocation 
			//for (int i = b.size - 1; i >= 0; i--) // is this backwards?
			for (int i = 0; i < b.size; i++) 
			{
				*file_B >> curr_num; 
				b.coeffs[i] = curr_num;
			}
		}

		// at this point we should have a and b completely filled out 
		poly_t c; 
		c.size = (a.size * 2) - 1;
		c.coeffs = new int[c.size];
		int delta = sqrt(abs(c.size)); // needs to be sqrt(abs(c.size))

		polymultiply(a, b, c, delta);

		if (printout)
		{
			polyprint(a);
			polyprint(b);
			polyprint(c);
		}

		// free the memory lol 
		delete [] a.coeffs; 
		delete [] b.coeffs; 
		delete [] c.coeffs;
	}
	catch (...)
	{
		cerr << "ERROR?!" << endl;
	}

}
