#include "printer.h"

#include <iostream>
#include <limits.h>

using namespace std;
/*
_Monitor Printer {
  public:
	enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Timer, Train, Conductor, TrainStop, Student, WATCardOfficeCourier };

  private:
  	class PImpl;
  	PImpl * pimpl;

  public:
	Printer( unsigned int numStudents, unsigned int numTrains, unsigned int numStops, unsigned int numCouriers );
	~Printer();
	void print( Kind kind, char state );
	void print( Kind kind, char state, unsigned int value1 );
	void print( Kind kind, char state, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1 );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 );
	void print( Kind kind, unsigned int lid, char state, char c );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, char c );
	void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c );
}; // Printer

*/

Printer::Printer( unsigned int numStudents, unsigned int numTrains, unsigned int numStops, unsigned int numCouriers )
{
    // Store parameters 
    nStudents = numStudents; 
    nTrains = numTrains; 
    nStops = numStops; 
    nCouriers = numCouriers;

    // We need a column for: 
    // Parent, Groupoff, Watcard Office, Names, Timer, All trains, All Conductors (1 per train), All stops, All students, All Watcard Couriers 
    nColumns = 5 + nTrains + nTrains + nStops + nStudents + nCouriers; 

    // Allocate space for columns on heap
    columns = new Column[nColumns];
    initializeColumns("QQQ", true);

    cout << "space allocated!" << endl; //DEBUGGING

    // Create the title columns
    columns[0].output = "Parent";
    columns[1].output = "Gropoff";
    columns[2].output = "WAToff";
    columns[3].output = "Names";
    columns[4].output = "Timer";
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + i].output  = "Train" + to_string(i);
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + nTrains + i].output  = "Cond" + to_string(i);
    for (unsigned int i = 0; i < nStops; i++)
        columns[5 + 2*nTrains + i].output  = "Stop" + to_string(i);
    for (unsigned int i = 0; i < nStudents; i++)
        columns[5 + 2*nTrains + nStops + i].output  = "Stud" + to_string(i);
    for (unsigned int i = 0; i < nCouriers; i++)
        columns[5 + 2*nTrains + nStops + nStudents + i].output  = "WCour" + to_string(i);

    flush();

    // Create the barriers
    initializeColumns("*******", true);
    flush();
    
} // Printer::Printer




Printer::~Printer() {

    flush();
    cout << "***********************" << endl;

    delete[] columns;
} // Printer::~Printer

// Sets all columns 
void Printer::initializeColumns(string value, bool occupied)
{
    for (unsigned int i = 0; i < nColumns; i++)
    {
        columns[i].output = value; 
        columns[i].occupied = occupied;
    }
}

// Checks if any columns are occupied
bool Printer::flushable()
{
    for (unsigned int i = 0; i < nColumns; i++)
    {
        if (columns[i].occupied == true)
            return true;
    }
    return false;
}

// Prints all columns to cout and removes occupied flag
void Printer::flush()
{
    cout << "Flush called!" << nColumns << endl; // DEBUGGING

    if (!flushable()) return;

    for (unsigned int i = 0; i < nColumns; i++)
    {
        if (!columns[i].occupied)
        {
            cout << "\t";
            continue;
        } 
        // Print to cout
        cout << columns[i].output << "\t"; 
    }
    cout << endl;

    // Reset columns, remove flag
    initializeColumns("", false);

    cout << "flushed!" << endl; //DEBUGGING

} // Printer::flush

// Calculates the index of the column based on its kind and id
unsigned int Printer::calculateColumnNum(Kind kind, int id)
{
    // Can convert directly from enum kind to unsigned int
    if (kind < Kind::Train) return (unsigned int) kind; 

    // id'd ones are based on offset + id
    if (kind == Kind::Train) return 5 + id;
    if (kind == Kind::Conductor) return 5 + nTrains + id; 
    if (kind == Kind::TrainStop) return 5 + 2*nTrains + id; 
    if (kind == Kind::Student) return 5 + 2*nTrains + nStops + id; 
    if (kind == Kind::WATCardOfficeCourier) return 5 + 2*nTrains + nStops + nStudents + id; 

    // Error: wrong kind
    cerr << "WRONG KIND OF KIND ??? " << endl;
    return 99999;
}// Printer::calculateColumnNum

// A private function from which all printing happens (to modularize code)
// To omit values from the printer, assign default values to parameters
void Printer::setColumn( Kind kind, char state, unsigned int lid, unsigned int value1, unsigned int value2, char c, unsigned int oid)
{
    cout << calculateColumnNum(kind, lid); // DEBUGGING
    cout << state << " " << lid << " " << value1 << " " << value2 << " " << c << " " << oid << endl; // DEBUGGING

    unsigned int colnum = calculateColumnNum(kind, lid);

    if (columns[colnum].occupied)
    {
        cout << "EXISTING COLUMN DETECTED: \"" << columns[colnum].output << "\"" << endl; //DEBUGGING
        flush();
    }

    columns[colnum].output.append(state, 1);

    cout << "ADDED STATE" << endl;// DEBUGGING

    // If there are inputted numbers... 
    if (value1 != UINT_MAX)
    {
        columns[colnum].output += std::to_string(value1);

        if (value2 != UINT_MAX)
            columns[colnum].output += ',' + std::to_string(value2);
        
        if (oid != UINT_MAX)
            columns[colnum].output += ',' + std::to_string(oid);
    }

    cout << "ADDED NUMBERS IF ANY" << endl;// DEBUGGING

    // If there is an inputted final character
    if (c != ' ')
    {
        columns[colnum].output += c;
    }

    cout << "ADDED CHARS IF ANY" << endl;// DEBUGGING

    // Set flag
    columns[colnum].occupied = true;
} // Printer::setColumn

// PUBLIC PRINTING FUNCTIONS -------------------------------------------------------------------------------
void Printer::print( Kind kind, char state )
{
    setColumn(kind, state);
} // Printer::print

// Prints with number
void Printer::print( Kind kind, char state, unsigned int value1 )
{
    setColumn(kind, state, 0, value1);
} // Printer::print

// Prints with two numbers
void Printer::print( Kind kind, char state, unsigned int value1 , unsigned int value2 )
{
    setColumn(kind, state, 0, value1, value2);
} // Printer::print

// Basic, prints only state at a particular id
void Printer::print( Kind kind, unsigned int lid, char state )
{
    setColumn(kind, state, lid);
} // Printer::print

// Prints with number at a particular id
void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1 )
{
    setColumn(kind, state, lid, value1);
} // Printer::print

// Prints with two number at a particular id
void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 )
{
    setColumn(kind, state, lid, value1, value2);
} // Printer::print


void Printer::print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 )
{
    setColumn(kind, state, lid, value1, value2, ' ', oid);
}

void Printer::print( Kind kind, unsigned int lid, char state, char c )
{
    setColumn(kind, state, lid, UINT_MAX, UINT_MAX, c);
} // Printer::print


void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, char c )
{
    setColumn(kind, state, lid, value1, UINT_MAX, c);
} // Printer::print

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c )
{
    setColumn(kind, state, lid, value1, value2, c);
} // Printer::print


/* OLD CODE 
// Basic, prints only state
void print( Kind kind, char state )
{
    int colnum = calculateColumnNum(kind);
    if (column[colnum] != nullColumn)
        flush();

    column[colnum] += state;
}

// Prints with number
void print( Kind kind, char state, unsigned int value1 )
{
    print(kind, state);

    int colnum = calculateColumnNum(kind);
    column[colnum] += value1;
}

// Prints with two numbers
void print( Kind kind, char state, unsigned int value1 , unsigned int value2 )
{
    print(kind, state, value1);

    int colnum = calculateColumnNum(kind);
    column[colnum] += ",";
    column[colnum] += value2;
}

// Basic, prints only state at a particular id
void print( Kind kind, unsigned int lid, char state );

*/