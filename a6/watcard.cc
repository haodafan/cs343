#include "watcard.h"

#include <cassert>
/*
class WATCard {
	class PImpl; // *** Replace these two lines with your own implementation.
	PImpl * pimpl;

	WATCard( const WATCard & ) = delete;				// prevent copying
	WATCard & operator=( const WATCard & ) = delete;

  public:
	typedef Future_ISM<WATCard *> FWATCard;				// future watcard pointer
	WATCard();
	~WATCard();
	void deposit( unsigned int amount );
	void withdraw( unsigned int amount );
	unsigned int getBalance();
	bool paidForTicket();
  	void resetPOP();
}; // WATCard
*/

class WATCard::PImpl 
{
    public:
    bool paid = false; 
    unsigned int balance = 0;
};

WATCard::WATCard()
{
    pimpl = new PImpl();
}

WATCard::~WATCard()
{
    delete pimpl;
}

void WATCard::deposit( unsigned int amount )
{
    pimpl->balance += amount;
}

void WATCard::withdraw( unsigned int amount )
{
    assert ( amount <= pimpl->balance );
    pimpl->balance -= amount;
    pimpl->paid = true;
}

unsigned int WATCard::getBalance()
{
    return pimpl->balance;
}

bool WATCard::paidForTicket()
{
    return pimpl->paid;
}

void WATCard::resetPOP()
{
    pimpl->paid = false;
}