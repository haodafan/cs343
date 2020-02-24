#include <uCobegin.h>
#include <iostream>
#include "MPRNG2.h"
int i;
MPRNG prng;

void p(int j) { uBaseTask::yield( prng(10) );i += j; }
int f(int j) { uBaseTask::yield( prng(10) );return j * 2;}

int main() {
	uProcessor pr[3];
	i = 0;
	auto tp = START( p, 5 );
	std::cout << i << std::endl;
	auto tf = START( f, 8 );
	std::cout << i << std::endl;
	WAIT( tp );
	std::cout << i << std::endl;
	i = WAIT( tf );
	std::cout << i << std::endl;
}
