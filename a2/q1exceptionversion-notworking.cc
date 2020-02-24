
#include "q1binsertsort.h"

using namespace std;

// called on resume()
template<typename T> void Binsertsort<T>::main()
{
	// Base case: This node doesn't actually exist
	_Enable {
	_Resume _Event() _At resumer();
	cout << "Initialized" << endl;
	suspend(); // Thus, we need to "initialize" all values first
	}

	cout << "begin " << value << endl;
	
	T pivot = value;
	try {
		_Enable {
		suspend();                      // return for next value
		}
	} catch (Sentinel &) {              // end of value set
		// implies leaf node at retrieval mode
		cout << "Retrieve leaf " << value;
		_Enable {
		suspend();
		_Resume Sentinel() _At resumer();
		return;
		} // _Enable
	}

	cout << "begin sort " << value << endl;

	// implies vertex node
	Binsertsort<T> less, greater;       // children of this node
	
	cout << "initialize pivots for " << value << endl;

	// Initialize them 
	if (value < pivot)
	{
		try { _Enable{ less.sort(value); }} catch (_Event &) {cout << "values initialized for less than " << value << endl;}
	}
	else
	{
		try { _Enable{ greater.sort(value); }} catch (_Event &) {cout << "values initialized for greater than " << value << endl;}
	}
	
	

	//bool b_less = false;
	//bool b_greater = false;

	// Enter infinite loop, terminate on Sentinel 
	try 
	{
		_Enable {
		while (1 == 1) {
			cout << " looping on " << value << endl;
			if (value < pivot){
				//b_less = true;
				less.sort(value); 
			}
			else {
				//b_greater = true;
				greater.sort(value);
			}
			value = pivot;
			suspend();
		}
		} // _Enable
	}
	catch (Sentinel &)
	{
		_Enable {
		// Retrieval begins here 

		// Notify children of impending retrieval
		//if (b_less) {
			_Resume Sentinel() _At less;
		//}
		//if (b_greater) {
			_Resume Sentinel() _At greater;
		//}

		} // _Enable
	}
	// Retrieve works on similar logic to sort
	//if (b_less) {
		try  {
			_Enable {
			while (1==1) {
				value = less.retrieve();
				suspend(); 
			}
			}
		} 
		catch (Sentinel &) {} // Sentinel means all nodes less than this one has been retrieved
	//}
	value = pivot; // Current value
	//if (b_greater) {
		try {
			_Enable {
			suspend();
			while (1==1) {
				value = greater.retrieve();
				suspend(); 
			}
			}
		} 
		catch (Sentinel &) {
			_Resume Sentinel() _At resumer(); // Don't know why but we need this here
		} // Sentinel means all nodes more than this one has been retrieved
	//}
	suspend(); // Return final value
	_Resume Sentinel() _At resumer();
}

int main(int argc, char* argv[])
{

	istream * file_in = &cin;
	ostream * file_out = &cout; 

	switch (argc)
	{
		case 3: 
			file_out = new ofstream(argv[2]);
		case 2: 
			file_in = new ifstream(argv[1]);
			break;
		default: 
			cerr << "error: not enough args" << endl;
			return 1; 
	}
	
	int num;
	while (*file_in >> num)
	{
		Binsertsort<int> B;
		try {B.sort(0);} catch (Binsertsort<int>::Sentinel &) {}; // Initialization 
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
