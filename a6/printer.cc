#include "printer.h"

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

    // Value to indicate column is null
    nullColumn = ""

    // We need a column for: 
    // Parent, Groupoff, Watcard Office, Names, Timer, All trains, All Conductors (1 per train), All stops, All students, All Watcard Couriers 
    nColumns = 1 + 1 + 1 + 1 + numTrains + numTrains + numStops + numStudents + numCouriers; 

    // Create the title columns
    columns[0] = "Parent";
    columns[1] = "Gropoff";
    columns[2] = "WAToff";
    columns[3] = "Names";
    columns[4] = "Timer";
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + i] = "Train" + to_string(i);
    for (unsigned int i = 0; i < nTrains; i++)
        columns[5 + nTrains + i] = "Cond" + to_string(i);
    for (unsigned int i = 0; i < nStops; i++)
        columns[5 + 2*nTrains + i] = "Stop" + to_string(i);
    for (unsigned int i = 0; i < nStudents; i++)
        columns[5 + 2*nTrains + nStops + i] = "Stud" + to_string(i);
    for (unsigned int i = 0; i < numCouriers; i++)
        columns[5 + 2*nTrains + nStops + nStudents + i] = "WCour" + to_string(i);

    flush();

    // Create the barriers
    for (unsigned int i = 0; i < nCogflumns; i++)
        columns[i] = "*******";
    
    flush();
} // Printer::Printer

Printer::~Printer() {

    flush();
    cout << "***********************" << endl;

    delete[] columns;
} // Printer::~Printer

// The flush() function prints all columns to cout and replaces all values in columns with nullColumn
void Printer::flush()
{
    for (unsigned int i = 0; i < nColumns; i++)
    {
        // Print to cout
        cout << columns[i] << "\t"; 

        // Replace with null
        columns[i] = nullColumn;
    }
    cout << endl;

} // Printer::flush

// Calculates the index of the column based on its kind and id
unsigned int Printer::calculateColumnNum(Kind kind, int id = 0)
{
    // Can convert directly from enum kind to unsigned int
    if (kind < Kind::Train) return (unsigned int) kind; 

    // id'd ones are based on offset + id
    if (kind == Kind::Train) return 5 + id;
    if (kind == Kind::Conductor) return 5 + nTrains + id; 
    if (kind == Kind::Stop) return 5 + 2*nTrains + id; 
    if (kind == Kind::Student) return 5 + 2*nTrains + nStops + id; 
    if (kind == Kind::Courier) return 5 + 2*nTrains + nStops + nStudents + id; 

    // Error: wrong kind
    return 99999;
}// Printer::calculateColumnNum

// A private function from which all printing happens (to modularize code)
// To omit values from the printer, assign default values to parameters
void Printer::modularPrint( Kind kind, char state, unsigned int lid = 0, unsigned int value1 = -1, unsigned int value2 = -1, char c = ' ')
{
    unsigned int colnum = calculateColumnNum(kind, lid);
    if (column[colnum] != nullColumn)
        flush();

    column[colnum] += state;

    // If there are inputted numbers... 
    if (value1 != -1)
    {
        column[colnum] += std::to_string(value1);

        if (value2 != -1)
            column[colnum] += ',' + std::to_string(value1);
    }

    // If there is an inputted final character
    if (c != ' ')
    {
        column[colnum] += c;
    }
} // Printer::modularPrint

// PUBLIC PRINTING FUNCTIONS -------------------------------------------------------------------------------
void Printer::print( Kind kind, char state )
{
    modularPrint(kind, state);
} // Printer::print

// Prints with number
void Printer::print( Kind kind, char state, unsigned int value1 )
{
    modularPrint(kind, state, 0, value1);
} // Printer::print

// Prints with two numbers
void Printer::print( Kind kind, char state, unsigned int value1 , unsigned int value2 )
{
    modularPrint(kind, state, 0, value1, value2);
} // Printer::print

// Basic, prints only state at a particular id
void Printer::print( Kind kind, unsigned int lid, char state )
{
    modularPrint(kind, state, lid);
} // Printer::print

// Prints with number at a particular id
void print( Kind kind, unsigned int lid, char state, unsigned int value1 )
{
    modularPrint(kind, state, lid, value1);
} // Printer::print

// Prints with two number at a particular id
void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 )
{
    modularPrint(kind, state, lid, value1, value2);
} // Printer::print


void print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 )
{
    // wtf is an oid ????????????????????? 
}

void print( Kind kind, unsigned int lid, char state, char c )
{
    modularPrint(kind, state, lid, -1, -1, c);
} // Printer::print


void print( Kind kind, unsigned int lid, char state, unsigned int value1, char c )
{
    modularPrint(kind, state, lid, value1, -1, c);
} // Printer::print

void print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c )
{
    modularPrint(kind, state, lid, value1, value2, c);
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