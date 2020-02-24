// Purpose: copy file
//
// Command syntax:
//   $ ./a.out [ size (>= 0) | 'd' (default 20) [ code (>= 0) | 'd' (default 5) [ input-file [ output-file ] ] ] ]
// all parameters are optional. Defaults: size=>20, code=>5, input=>cin, output=>cout
//
// Examples:
//   $ ./a.out 34
//   $ ./a.out 34 0
//   $ ./a.out 34 0 in
//   $ ./a.out 34 0 in out

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;					// direct access to std
#include <cstdlib>					// exit

bool convert( int & val, char * buffer ) {		// convert C string to integer
    std::stringstream ss( buffer );			// connect stream and buffer
    string temp;
    ss >> dec >> val;					// convert integer from buffer
    return ! ss.fail() &&				// conversion successful ?
	! ( ss >> temp );				// characters after conversion all blank ?
} // convert

int main( int argc, char * argv[] ) {
    enum { DefaultSize = 20, DefaultCode = 5 };		// global defaults
    // MUST BE INT (NOT UNSIGNED) TO CORRECTLY TEST FOR NEGATIVE VALUES
    int size = DefaultSize, code = DefaultCode;		// default value
    istream * infile = &cin;				// default value
    ostream * outfile = &cout;				// default value

    try {
	switch ( argc ) {
	  case 4: case 5:
	    try {					// open input file first as output creates file
		infile = new ifstream( argv[3] );	// open input file
	    } catch( uFile::Failure & ) {
		cerr << "Error! Could not open input file \"" << argv[3] << "\"" << endl;
		throw 1;
	    } // try
	    if ( argc == 5 ) {
		try {
		    outfile = new ofstream( argv[4] );	// open output file
		} catch( uFile::Failure & ) {		// open failed ?
		    cerr << "Error! Could not open output file \"" << argv[4] << "\"" << endl;
		    throw 1;
		} // try
	    } // if
	    // FALL THROUGH
	  case 3:
	    if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
		if ( ! convert( code, argv[2] ) || code < 0 ) throw 1; // invalid ?
	    } // if
	    // FALL THROUGH
	  case 2:
	    if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
		if ( ! convert( size, argv[1] ) || size < 0 ) throw 1; // invalid ?
	    } // if
	    // FALL THROUGH
	  case 1:					// defaults
	    break;
	  default:					// wrong number of options
	    throw 1;
	} // switch
    } catch( ... ) {
	cerr << "Usage: " << argv[0]
	     << " [ size (>= 0) | 'd' (default " << DefaultSize
	     << ") [ code (>= 0) | 'd' (default " << DefaultCode
	     << ") [ input-file [ output-file ] ] ] ]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
    } // try
    //cout << "size " << size << " code " << code << endl;

    *infile >> noskipws;				// turn off white space skipping during input

    char ch;
    for ( ;; ) {					// copy input file to output file
	*infile >> ch;					// read a character
      if ( infile->fail() ) break;			// failed ?
	*outfile << ch;					// write a character
    } // for

    if ( infile != &cin ) delete infile;		// close file, do not delete cin!
    if ( outfile != &cout ) delete outfile;		// close file, do not delete cout!
} // main

// Local Variables: //
// compile-command: "u++ uIO.cc" //
// End: //
