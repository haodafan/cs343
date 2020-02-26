#include <iostream>
using namespace std;

_Coroutine Cons;					// Pre-declaration.

// Basic structure of the producer full coroutine.
_Coroutine Prod {
    Cons *c;						// Address of partner coroutine.
    int   N;						// Number of items to produce.
    int   money;					// Payment received.
    int   receipt;					// Number of returned receipt.

    void  main();					// Main body of the coroutine.
  public:
    int  payment(int money);				// Transfers payment in and returns receipt.
    void start(int N, Cons *c);				// Initializes the coroutine.
}; // Prod

// Basic structure of the consumer full coroutine.
_Coroutine Cons {
    Prod &p;						// Address of partner coroutine.
    int   p1, p2;					// Items to be consumed.
    int   status;					// Marks a successful delivery.
    int   done;						// Termination flag.

    void main();					// Main body of the coroutine.
  public:
    Cons(Prod &p) : p(p), done(0) {}			// Constructor.
    int delivery(int p1, int p2);			// Delivers the two items to be consumed.
    void stop();					// Used to terminate the coroutine.
}; // Cons

void Prod::main() {
    int i;						// Loop counter.
    int p1, p2;						// Items produced by random # generator.
    int status;						// Status of delivery.

    // 1st resume starts here.
    for (i = 1; i <= N; i += 1) {
	p1 = random() % 100;				// Generate p1 and p2.
	p2 = random() % 100;
	cout << "delivers:" << p1 << ", " << p2 << endl;
	status = c->delivery(p1, p2);
	cout << " gets status:" << status << endl;
	receipt += 1;
    } // for
    cout << "Prod stops" << endl;
    c->stop();
} // Prod::main

int Prod::payment(int money) {
    Prod::money = money;
    cout << " gets payment $" << money << endl;
    resume();						// Restart prod in Cons::delivery
    return receipt;
} // Prod::payment

void Prod::start(int N, Cons *c) {
    Prod::N = N;
    Prod::c = c;
    receipt = 0;
    resume();						// Restart Prod::main().
} // Prod::start

void Cons::main() {
    int money = 1;					// Amount to pay for the delivery.
    int receipt;					// Receipt received for payment.
    status = 0;

    // 1st resume starts here.
    for (;;) {
      if (done) break;
	cout << "receives:" << p1 << ", " << p2 << " and pays $" << money << endl;
	status += 1;
	receipt = p.payment(money);
	cout << "gets receipt #" << receipt << endl;
	money += 1;
    } // for
    cout << "Cons stops" << endl;
} // Cons::main

int Cons::delivery(int p1, int p2) {
    Cons::p1 = p1;
    Cons::p2 = p2;
    resume();						// Restart cons in Cons::main first time
    return status;					// and cons in Prod::payment afterwards.
} // Cons::delivery

void Cons::stop() {
    done = 1;
    resume();						// Restart main.
} // Cons::stop

// Main body of the program starts up the coroutines and waits for them to terminate.
int main() {
    Prod prod;
    Cons cons(prod);
    prod.start(5, &cons);
} // main
