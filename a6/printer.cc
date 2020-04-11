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
    // DEBUGGING
    cout << "PRINTER CONSTRUCTION CALLED: " << numStudents << " " << numTrains << " " << numStops << " " << numCouriers << endl;

    // Store parameters 
    nStudents = numStudents; 
    nTrains = numTrains; 
    nStops = numStops; 
    nCouriers = numCouriers;

    // We need a column for: 
    // Parent, Groupoff, Watcard Office, Names, Timer, All trains, All Conductors (1 per train), All stops, All students, All Watcard Couriers 
    nnnnnnColumns = 5 + nTrains + nTrains + nStops + nStudents + nCouriers; 

    // Allocate space for columns on heap
    columns = new Column[nnnnnnColumns];
    setSpecial("");

    // Create the title columns
    columns[0].special = "Parent";
    columns[1].special = "Gropoff";
    columns[2].special = "WAToff";
    columns[3].special = "Names";
    columns[4].special = "Timer";
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + i].special  = "Train" + to_string(i);
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + nTrains + i].special  = "Cond" + to_string(i);
    for (unsigned int i = 0; i < nStops; i++)
        columns[5 + 2*nTrains + i].special  = "Stop" + to_string(i);
    for (unsigned int i = 0; i < nStudents; i++)
        columns[5 + 2*nTrains + nStops + i].special  = "Stud" + to_string(i);
    for (unsigned int i = 0; i < nCouriers; i++)
        columns[5 + 2*nTrains + nStops + nStudents + i].special  = "WCour" + to_string(i);

    flush();

    // Create the barriers
    setSpecial("*******");
    flush();
    resetColumns(); //useless
} // Printer::Printer




Printer::~Printer() {

    flush();
    cout << "***********************" << endl;

    delete[] columns;
} // Printer::~Printer

// Sets all columns 
void Printer::setSpecial(string value)
{
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
    {
        columns[i].special = value; 
        columns[i].state = '$';
    }
}

// Checks if any columns are occupied
bool Printer::flushable()
{
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
    {
        if (columns[i].state != '?')
        {
            cout << "flushable(): FLUSHABLE" << endl;
            return true;

        }
    }
    cout << "flushable(): NOT FLUSHABLE" << endl;
    return false;
}

// Prints all columns to cout and removes occupied flag
void Printer::flush()
{
    // START OF DEBUGGING
    cout << "flush(): Current state of each of the " << nnnnnnColumns << " columns: ";
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
        cout << columns[i].state;

    cout << endl; 
    // END OF DEBUGGING

    if (!flushable()) return;

    for (unsigned int i = 0; i < nnnnnnColumns; i++)
    {
        // Empty case 
        if (columns[i].state == '?')
        {
            cout << "\t";
            continue;
        }//if

        // Special case
        if (columns[i].state == '$')
        {
            // Special state
            cout << columns[i].special << "\t";
            continue;
        }// if

        // Output state 
        cout << columns[i].state;

        // Output values 
        if (columns[i].v1 != UINT_MAX) cout << columns[i].v1;
        if (columns[i].v2 != UINT_MAX) cout << "," << columns[i].v2;
        if (columns[i].oid != UINT_MAX) cout << "," << columns[i].oid;

        // Output character 
        if (columns[i].c != '?') cout << columns[i].c;
        
    } //for
    cout << endl; // Onto next column

    cout << "flush(): flushed!" << endl; //DEBUGGING
    resetColumns();

} // Printer::flush

void Printer::resetColumns()
{
    // START OF DEBUGGING
    cout << "resetColumns(): Current state of each of the " << nnnnnnColumns << " columns: ";
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
        cout << columns[i].state;

    cout << endl; 
    // END OF DEBUGGING

    cout << "resetColumns(): columns reset!" << endl; //debugging

    for (unsigned int i = 0; i < nnnnnnColumns; i++)
    {
        columns[i].state = '?'; // Note we have two special states: ? for empty, $ for special
		columns[i].v1 = UINT_MAX; // first value (empty = UINT_MAX)
		columns[i].v2 = UINT_MAX; // second value (empty = UINT_MAX)
		columns[i].oid = UINT_MAX; // oid value (empty = UINT_MAX)
		columns[i].c = '?'; // Character value (empty = '?')
		columns[i].special = "n/a"; // Special printing values (like column titles)
    }

    // START OF DEBUGGING
    cout << "resetColumns() 2: Current state of each of the " << nnnnnnColumns << " columns: ";
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
        cout << columns[i].state;

    cout << endl; 
    // END OF DEBUGGING
}

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
    cout << "Set column: " << calculateColumnNum(kind, lid) << " to "; // DEBUGGING
    cout << state << " " << lid << " " << value1 << " " << value2 << " " << c << " " << oid << endl; // DEBUGGING

    // START OF DEBUGGING
    cout << "setColumn(): Current state of each of the " << nnnnnnColumns << " columns: ";
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
        cout << columns[i].state;

    cout << endl; 
    // END OF DEBUGGING

    unsigned int colnum = calculateColumnNum(kind, lid);

    if (columns[colnum].state != '?')
    {
        cout << "EXISTING COLUMN DETECTED: \'" << columns[colnum].state << "\'" << endl; //DEBUGGING
        flush();
    }

    // set column values
    columns[colnum].state = state;
    columns[colnum].v1 = value1;
    columns[colnum].v2 = value2; 
    columns[colnum].oid = oid;
    columns[colnum].c = c;

} // Printer::setColumn

// PUBLIC PRINTING FUNCTIONS -------------------------------------------------------------------------------
void Printer::print( Kind kind, char state )
{   
    // START OF DEBUGGING
    cout << "print(kind, state): Current state of each of the " << nnnnnnColumns << " columns: ";
    for (unsigned int i = 0; i < nnnnnnColumns; i++)
        cout << columns[i].state;

    cout << endl; 
    // END OF DEBUGGING

    cout << "print( kind, " << state << ");" << endl;
    setColumn(kind, state);
} // Printer::print

// Prints with number
void Printer::print( Kind kind, char state, unsigned int value1 )
{
    cout << "print( kind, " << state << ", " << value1 << ");" << endl;
    setColumn(kind, state, 0, value1);
} // Printer::print

// Prints with two numbers
void Printer::print( Kind kind, char state, unsigned int value1 , unsigned int value2 )
{
    cout << "print( kind, " << state << ", " << value1 <<  ", " << value2 << ");" << endl;
    setColumn(kind, state, 0, value1, value2);
} // Printer::print

// Basic, prints only state at a particular id
void Printer::print( Kind kind, unsigned int lid, char state )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid << ");" << endl;
    setColumn(kind, state, lid);
} // Printer::print

// Prints with number at a particular id
void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1 )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid << ", " << value1 <<  ");" <<  endl;
    setColumn(kind, state, lid, value1);
} // Printer::print

// Prints with two number at a particular id
void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 )
{
    
    cout << "print( kind, " << state << ", " << "lid = " << lid << ", " << value1 <<  ", " << value2 << ");" << endl;
    setColumn(kind, state, lid, value1, value2);
} // Printer::print


void Printer::print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid << ", " << "oid = " << oid << ", "<< value1 <<  ", " << value2 << ");" << endl;
    setColumn(kind, state, lid, value1, value2, ' ', oid);
}

void Printer::print( Kind kind, unsigned int lid, char state, char c )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid <<  ", c = " << c << ");" << endl;
    setColumn(kind, state, lid, UINT_MAX, UINT_MAX, c);
} // Printer::print


void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, char c )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid << ", "<< value1 << ", c = " << c << ");" << endl;
    setColumn(kind, state, lid, value1, UINT_MAX, c);
} // Printer::print

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c )
{
    cout << "print( kind, " << state << ", " << "lid = " << lid << ", "<< value1 <<  ", " << value2 << ", c = " << c << ");" << endl;
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