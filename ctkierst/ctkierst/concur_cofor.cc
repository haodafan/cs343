#include <uCobegin.h>
#include <iostream>
#include <iomanip>
#include "MPRNG2.h"

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

	COFOR( r, 0, rows,
		// for ( int r = 0; r < rows; r += 1 )
		subtotals[r] = 0; // r is loop number
		for ( int c = 0; c < cols; c += 1 )
			subtotals[r] += matrix[r][c];
	); // wait for threads

	for ( int r = 0; r < rows; r += 1 ) {
		total += subtotals[r];
	} // for
	std::cout << total << std::endl;
}