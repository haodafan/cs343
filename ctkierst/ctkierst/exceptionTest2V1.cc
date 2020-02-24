#include <iostream>
using namespace std;

_Event E {};

_Coroutine C {
    void main() {
        try {
            _Enable { // allow non-local exceptions
                for ( int i = 0; i < 5; i += 1 ) {
                    cout << &uThisCoroutine() << ": " << i << endl;
                    suspend();
                } // for
            } // _Enable
        } catch (E & e ) {
            cout << &uThisCoroutine() << ": caught E" << endl;
        } // catch
    } // main
public:
    C() {}
    void mem() { resume(); }
};

int main() {
    C c;
    for ( int i = 0; i < 5; i += 1 ) {
        _Resume E() _At c; // exception pending
        c.mem(); // trigger exception
    } // for
} // main
