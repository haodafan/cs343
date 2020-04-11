#pragma once

_Monitor Printer {
  public:
	enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Timer, Train, Conductor, TrainStop, Student, WATCardOfficeCourier };

  private:
	struct Column 
	{
		char state; // Note we have two special states: ? for empty, $ for special
		unsigned int v1; // first value (empty = UINT_MAX)
		unsigned int v2; // second value (empty = UINT_MAX)
		unsigned int oid; // oid value (empty = UINT_MAX)
		char c; // Character value (empty = '?')
		std::string special; // Special printing values (like column titles)
	};

  	//class PImpl;
  	//PImpl * pimpl;  

	unsigned int nStudents; 
	unsigned int nTrains; 
	unsigned int nStops; 
	unsigned int nCouriers; 

	unsigned int nColumns;
	Column* columns;

	// private functions
	void setSpecial(std::string value);
	void resetColumns();
	bool flushable();
	void flush();
	unsigned int calculateColumnNum(Kind kind, int id = 0);
	void setColumn( Kind kind, char state, 
			unsigned int lid = 0, 
			unsigned int value1 = UINT_MAX, 
			unsigned int value2 = UINT_MAX, 
			char c = '?', 
			unsigned int oid = UINT_MAX);


	// public functions
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

// Local Variables: //
// mode: c++ //
// tab-width: 4 //
// compile-command: "make" //
// End: //
