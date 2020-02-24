#include <iostream>
#include <string>
using namespace std;

_Coroutine Fc {
    void main() {
        string name = "fc::main";
        uThisCoroutine().setName( name.c_str() ); 
        cout << uThisCoroutine().getName() << " this = " << this
	     << ", starter = " << starter().getName()
	     << ", and resumer = " << resumer().getName() << endl;  
        mem();
        cout << uThisCoroutine().getName() << " before resume and resumer = "
	     << resumer().getName() << endl;
        resume();
        cout << uThisCoroutine().getName() << " before suspend and resumer = "
	     << resumer().getName() << endl;
        suspend();
        cout << uThisCoroutine().getName() << " terminating" << endl;
    }
  public:
    void mem() {
        cout << "fc::mem before resume this = " << this << "; uThisCoroutine = " 
            << uThisCoroutine().getName() << endl;
        resume();
        cout << "fc::mem after resume this = " << this << "; uThisCoroutine = " 
            << uThisCoroutine().getName() << endl;
    }
};

int main() {
    Fc fc;
    string name = "::main";
    uThisCoroutine().setName( name.c_str() );
    cout << "&fc = " << &fc << endl << uThisCoroutine().getName() << " before fc::mem" << endl;
    fc.mem();
    cout << uThisCoroutine().getName() << " after fc::mem 1" << endl;
    fc.mem();
    cout << uThisCoroutine().getName() << " after fc::mem 2" << endl;
}
