_Coroutine FloatConstant {
  public:
    enum Status { CONT, MATCH };			// possible status
  private:
    void main();					// coroutine main
  public:
    _Event Error {};					// last character is invalid

    Status next( char c ) {
	ch = c;						// communication in
	resume();					// activate
	return status;					// communication out
    } // FloatConstant::next
}; // FloatConstant
