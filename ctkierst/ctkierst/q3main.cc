// Purpose: float number format check
//
// Command syntax:
//   $ ./a.out [filename]
// filename parameter is optional. Default: use cin 

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;					// direct access to std
#include <cstdlib>					// exit

#include "q3floatconstant.h"        // main functionality

int main( int argc, char * argv[] ) {
	
    istream * infile = &cin;				// default value
    ostream * outfile = &cout;				// default value

    try {
	
	if (argc == 1) // can use switch instead 
	{
		try {
		//user inputted file 
		infile = new ifstream(argv[1]);  // is this the right arg? lol // open input file
		}
		catch ( uFile::Failure & ) {  // open failed?
			cerr << "Error! Could not open file \"" << "\"" endl;
			throw 1; 
		}//try
	}
	
	} catch( ... ) {
	cerr << "Usage: " << argv[0]
	     << " [ size (>= 0) | 'd' (default " << DefaultSize
	     << ") [ code (>= 0) | 'd' (default " << DefaultCode
	     << ") [ input-file [ output-file ] ] ] ]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
	
	// q3float logic 
	

    if ( infile != &cin ) delete infile;		// close file, do not delete cin!
    if ( outfile != &cout ) delete outfile;		// close file, do not delete cout!
} // main

// Local Variables: //
// compile-command: "u++ uIO.cc" //
// End: //