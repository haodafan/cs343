#include <iostream>
using namespace std;

_Event E { 
  public:
    int v;
    E(int i) : v(i) {}
};

_Coroutine C {
    void main() {
        _Enable {
            cout << "C::main before resume E at resumer" << endl;
            _Resume E(13) _At resumer(); // _Throw deprecated for asynchronous raise
            cout << "C::main after resume E at resumer" << endl;
            suspend();
            cout << "C::main after suspend and terminating" << endl;
        } // enable
    }
    public:
    void mem() {
        cout << "C::mem before resume" << endl;
        resume();
        cout << "C::mem after resume" << endl;
    }
};

int main() {
    C c;
    try {
        _Enable {   
            c.mem();
        } // enable
    } _CatchResume( E e ) {
        cout << "caught E.v = " << e.v << endl;
    } // catch
    c.mem();
} // main
