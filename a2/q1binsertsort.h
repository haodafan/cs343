
template<typename T> _Coroutine Binsertsort {
    T value;                        // communication: value being passed down/up the tree
    void main();                    // YOU WRITE THIS ROUTINE
  public:
    _Event Sentinel {}; // Sentinel: 
    void sort( T value ) {          // value to be sorted
        Binsertsort::value = value;
        resume();
    }
    T retrieve() {                  // retrieve sorted value
        resume();
        return value;
    }
};

// called on resume()
template<typename T> void Binsertsort<T>::main()
{
	T pivot = value;
	try {
		_Enable {
		suspend();                      // return for next value
		}
	} catch (Sentinel &) {              // end of value set
		// implies leaf node at retrieval mode
		_Enable {
		suspend();
		_Resume Sentinel() _At resumer();
		return;
		} // _Enable
	}

	// implies vertex node
	Binsertsort<T> less, greater;       // children of this node

	// Enter infinite loop, terminate on Sentinel 
	try 
	{
		_Enable {
		while (1 == 1) {
			if (value < pivot){
				less.sort(value); 
			}
			else {
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
		if (less.getState() == State::Inactive) {
			_Resume Sentinel() _At less;
		}
		if (greater.getState() == State::Inactive) {
			_Resume Sentinel() _At greater;
		}

		} // _Enable
	}
	// Retrieve works on similar logic to sort
	if (less.getState() == State::Inactive) {
		try  {
			_Enable {
			while (1==1) {
				value = less.retrieve();
				suspend(); 
			}
			} // _Enable
		} 
		catch (Sentinel &) {} // Sentinel means all nodes less than this one has been retrieved
	}
	value = pivot; // Current value
	if (greater.getState()  == State::Inactive) {
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
	}
	suspend(); // Return final value
	_Resume Sentinel() _At resumer();
}
