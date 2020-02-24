// Compile with:
// g++ -std=c++17 boost_semi_fib.cc -lboost_coroutine -lboost_context
#include <iostream>
#include <boost/coroutine2/all.hpp>
using namespace boost::coroutines2;

int fibonacci( coroutine< int >::push_type & suspend ) {
  int fn = 0, fn1 = fn, fn2;             // 1st case
  suspend( fn );
  fn = 1; fn2 = fn1; fn1 = fn;           // 2nd case
  suspend( fn );
  for (;;) {
    fn = fn1 + fn2; fn2 = fn1; fn1 = fn; // general case
    suspend( fn );
  }
}

int main() {
  coroutine< int >::pull_type fib( fibonacci );
  // declaration does first resume
  for ( int i = 0; i < 15; i += 1 ) {
    std::cout << fib.get() << " ";      // get output
    fib();                              // resume
  }
  std::cout << std::endl;
}
