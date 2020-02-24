#include "q2polymult.h"
#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif

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

// main function 
int main( int argc , char * arvg[] ) 
{
    
}
