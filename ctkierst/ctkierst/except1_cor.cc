#include <iostream>
using namespace std;

struct E {};

_Coroutine C {
    bool error;
    void main() {
        try {
            error = false;
            cout << "C::main throw E" << endl;
            throw E();
        } catch( E ) {
            cout << "C::main handler" << endl;
            error = true;
        }
    }
    public:
    void mem() {
        cout << "C::mem before resume" << endl;
        resume();
        cout << "C::mem after resume" << endl;
        if ( error ) { 
            cout << "C::mem throw E" << endl;
            throw E();
        }
    }
};

int main() {
    C c;
    try {
       c.mem();
    } catch( E ) {
       cout << "main caught E" << endl;
    }
}
