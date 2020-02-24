template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size = 10 );
    void insert( T elem );
    T remove();
};

#ifdef BUSY                            // busy waiting implementation
// implementation
#endif // BUSY

#ifdef NOBUSY                          // no busy waiting implementation
// implementation
#endif // NOBUSY

_Task Producer {
    void main();
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay );
};

_Task Consumer {
    void main();
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum );
};
