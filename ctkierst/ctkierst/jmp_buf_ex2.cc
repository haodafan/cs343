// How does the use of jmp_buf/setjmp/longjmp compare to the use of labels and gotos?
// What happens if call setjmp more than once on the same jmp_buf variable?
// What happens if we call longjmp after the call to f in main?
#include <iostream>
#include <setjmp.h>

jmp_buf buffer;

void h( int val ) {
  std::cout << "h::val = " << val << std::endl;
  longjmp( buffer, val );
  std::cout << "h::never execute\n";
}

void g( int val ) {
  std::cout << "g::val = " << val << std::endl;
  int myval = setjmp( buffer );
  if ( myval == 0 ) { h( val + 1 ); return; }
  std::cout << "g::myval = " << myval << std::endl;
}

void f() {
  int val = setjmp( buffer );
  if ( val == 0 ) g( 10 );
  std::cout << "f::val = " << val << std::endl;
}

int main() {
  f();
  //longjmp(buffer,13);
}
