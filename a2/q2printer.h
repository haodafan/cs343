#pragma once

class Printer {
    struct PImpl;
    PImpl *pimpl;
    
  public:
    Printer( const unsigned int NoOfPlayers, const unsigned int NoOfCards );
    ~Printer();
    void prt( unsigned int id, int took, int RemainingPlayers );
}; // Printer

// Local Variables: //
// mode: c++ //
// compile-command: "make cardgame" //
// End: //
