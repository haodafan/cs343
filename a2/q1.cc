#include <iostream> 
#include <fstream>

#include "q1binsertsort.h"

using namespace std;

int main(int argc, char* argv[])
{

	istream * file_in = &cin;
	ostream * file_out = &cout; 

	switch (argc)
	{
		case 3: 
			try { file_out = new ofstream(argv[2]); } 
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }
		case 2: 
			try { file_in = new ifstream(argv[1]); }
			catch(...) { cerr << "Cannot open file \"" << argv[2] << "\""; return 1; }
			break;
		default: 
			cerr << "error: not enough args" << endl;
			return 1; 
	}
	
	int num;
	while (*file_in >> num)
	{
		Binsertsort<int> B;
		int numTotal = num; 
		for (int i = 0; i < numTotal; i++)
		{
			try {
				*file_in >> num; 
				B.sort(num);
			}
			catch (...)
			{
				cout << "error: not enough input" << endl;
			}
		} //for

		// End of input, throw sentinel
		_Resume Binsertsort<int>::Sentinel() _At B;
		
		// Begin Retrieve Values 
		try {
			_Enable {
			for (int i = 0; i < numTotal; i++) 
			{
				*file_out << B.retrieve() << " "; 
			}
			*file_out << endl;
			} // enable
		} // try
		catch (Binsertsort<int>::Sentinel &) {}
	} // while
	delete file_in;
	if (argc == 3) // case 3: file_out = new ofstream(...)
		delete file_out;
	return 0;
}
