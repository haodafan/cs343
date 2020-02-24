#include <uActor.h>
#include <iostream>
#include <iomanip>
#include "MPRNG2.h"

_Actor Adder {
	int * row, cols, & subtotal; // communication

	Allocation receive( Message & ) { // only startMsg
		subtotal = 0;
		for ( int c = 0; c < cols; c += 1 ) {
			subtotal += row[c];
		}
		return Delete; // delete actor (match new)
	}
public:
	Adder( int row[], int cols, int & subtotal ) :
		row{row}, cols{cols}, subtotal{subtotal} {}
};

int main() { 
	MPRNG prng;
	const int rows = 10, cols = 10;
	int matrix[rows][cols], subtotals[rows], total = 0;
	for ( int r = 0; r < rows; r += 1 ) {
		for ( int c = 0; c < cols; c += 1 ) {
			matrix[r][c] = prng( 100 );
			std::cout << std::setw(5) << matrix[r][c];
		} // for
		std::cout << std::endl;
	} // for

	uActorStart(); // start actor system
		for ( int r = 0; r < rows; r += 1 ) {
			*new Adder( matrix[r], cols, subtotals[r] ) | uActor::startMsg;
		} // for
	uActorStop();

	for ( int r = 0; r < rows; r += 1 ) {
		total += subtotals[r];
	} // for
	std::cout << total << std::endl;
}