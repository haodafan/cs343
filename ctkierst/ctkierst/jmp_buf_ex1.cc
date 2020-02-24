// Example of returning from h() back to f().
//
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
  h( val + 1 );
}

void f() {
  int val = setjmp( buffer );
  std::cout << "1 f::val = " << val << std::endl;
  if ( val == 0 ) g( 10 );
  std::cout << "2 f::val = " << val << std::endl;
}

int main() {
  f();
}
