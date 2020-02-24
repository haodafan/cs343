#include <uCobegin.h>
#include <iostream>
#include "MPRNG2.h"
int i;
MPRNG prng;

void p1(int j) { uBaseTask::yield( prng(10) ); i += j; }
void p2(int j) { uBaseTask::yield( prng(10) ); i += j; }
void p3(int j) { uBaseTask::yield( prng(10) ); i -= j; }

int main() {
	uProcessor p[5];
	i = 0;
	COBEGIN
		BEGIN uBaseTask::yield( prng(10) ); i = 1; END
		BEGIN p1( 5 ); END
		BEGIN p2( 7 ); END
		BEGIN p3( 9 ); END
	COEND
	std::cout << i << std::endl;
}
