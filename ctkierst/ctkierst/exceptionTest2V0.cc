#include <iostream>
using namespace std;

_Event E {};

_Coroutine C {
    void main() {
        try {
            for ( int i = 0; i < 5; i += 1 ) {
                _Enable { // allow non-local exceptions
                    cout << &uThisCoroutine() << ": " << i << endl;
                    suspend();
                } // _Enable
            } // for
        } catch (E & e ) {
            cout << &uThisCoroutine() << ": caught E" << endl;
        } // catch
    } // main
public:
    C() { resume(); }
    void mem() { resume(); }
};

int main() {
    C c;
    for ( int i = 0; i < 5; i += 1 ) {
        _Resume E() _At c; // exception pending
        c.mem(); // trigger exception
    } // for
} // main
