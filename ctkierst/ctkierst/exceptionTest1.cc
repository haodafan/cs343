#include <iostream>
#include <string>
using namespace std;

_Event ev {};

_Coroutine X {
    X * partner;
    string name;
    static int id;
    int myid;
    void main();
   public:
    X(X *partner);
    void next() { 
        cout << uThisCoroutine().getName() << " before resume" << endl;
        resume();
        cout << uThisCoroutine().getName() << " after resume" << endl;
    }
};

int X::id = 0;

X::X(X *partner) : partner( partner ), myid( id ) { id += 1; resume();}
void X::main() {
    name = "X" + std::to_string( myid );
    setName( name.c_str() );
    suspend();
    try {
        cout << getName() << " starting" << endl;
        _Enable {
            if ( partner != nullptr ) partner->next();
            else suspend();
            cout << getName() << " continuing" << endl;
            if ( myid == 1 ) {
                cout << getName() << " throwing exception "<< endl;
#ifdef AT
                _Resume ev() _At *partner;
                partner->next();
#else
                _Resume ev();
                cout << getName() << " after throwing exception "<< endl;
#endif
            } // if
      } // _Enable
    } _CatchResume( ev ) {
        cout << getName() << " receiving exception" << endl;
    } // catch
} // X::main
    
int main() {
    string name = "::main";
    uThisCoroutine().setName( name.c_str() );
    X x0( nullptr );
    X x1( &x0 );
    X x2( &x1 );
    x2.next();
} // main
