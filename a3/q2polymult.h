#pragma once

struct polynomial
{
	int* coeffs; 
	int size;
};

typedef polynomial poly_t; // is dat right? 

void polymultiply( const poly_t & a, const poly_t & b, poly_t & c, const size_t delta );
