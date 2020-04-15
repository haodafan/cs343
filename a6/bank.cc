/* _Monitor Bank {
	class PImpl; // *** Replace these two lines with your own implementation.
	PImpl * pimpl;

  public:
	Bank( unsigned int numStudents );
	~Bank();
	
	void deposit( unsigned int id, unsigned int amount ); // deposit "amount" $ for student "id";
	void withdraw( unsigned int id, unsigned int amount ); // withdrawal "amount" $ from student "id"; block until student has enough funds
}; */ // Bank

#include "bank.h"

#include "printer.h"

struct BankAccount 
{
    unsigned int balance = 0; 
    unsigned int credit = 0;
    uCondition blocker;
};

class Bank::PImpl
{
    public:
    unsigned int numStudents;
    BankAccount * accounts;

    PImpl(unsigned int numStudents) : numStudents(numStudents) 
    {
        accounts = new BankAccount[numStudents];
    }
    ~PImpl() 
    {
        delete [] accounts;
    }
};

Bank::Bank( unsigned int numStudents )
{
    pimpl = new PImpl(numStudents);
}
Bank::~Bank()
{
    // Release any waiting couriers 
    for (unsigned int i = 0; i < pimpl->numStudents; i++)
    {
        if (!pimpl->accounts[i].blocker.empty()) pimpl->accounts[i].blocker.signalBlock();
    }
    delete pimpl;
}

// deposit "amount" $ for student "id";
void Bank::deposit( unsigned int id, unsigned int amount )
{
    pimpl->accounts[id].balance += amount;
    if (!pimpl->accounts[id].blocker.empty() && pimpl->accounts[id].credit < pimpl->accounts[id].balance) 
    {
        pimpl->accounts[id].blocker.signalBlock(); // signal blocked couriers
    }
    
}

// withdrawal "amount" $ from student "id"; block until student has enough funds
void Bank::withdraw( unsigned int id, unsigned int amount )
{
    if (pimpl->accounts[id].balance < amount)
    {
        pimpl->accounts[id].credit = amount;
        pimpl->accounts[id].blocker.wait(); // block courier
    }
    pimpl->accounts[id].balance -= amount;
}