#include <iostream>
using namespace std;

struct E {};

_Coroutine C {
    void main() { 
        cout << "C::main before throw" << endl; 
        throw E(); 
        cout << "C::main after throw" << endl; 
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
        cout << "::main before call" << endl;
        c.mem();
        cout << "::main after call" << endl;
#ifndef RES
    } catch( uBaseCoroutine::UnhandledException ) {
        cout << "::main catch" << endl;
#else
    } _CatchResume( uBaseCoroutine::UnhandledException ) {
        cout << "::main catchresume" << endl;
#endif
    }
}
