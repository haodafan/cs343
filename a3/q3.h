#include "MPRNG.h"
#include <iostream>

template<typename T> class BoundedBuffer {
    const unsigned int size; 
    unsigned int num; // representation of USED SIZE (not index)

    unsigned int prod_i; 
    unsigned int cons_i;

    T* buffer;

    // required locks
    uOwnerLock * owner; // used for changing `buffer` and `num`
    uCondLock * prodlock; // used for syncing producers 
    uCondLock * conslock; // used for syncing consumers
  
  void inc_i(unsigned int & i)
  {
    if (i < size - 1) i++;
    else i = 0; 
  }

  #ifdef NOBUSY
    //int locks = 0; 
    //void smartSignal(uCondLock& lk);
    //uCondLock * bargeProdlock; 
    //uCondLock * bargeConslock; 
    bool signal_is_occuring = false;
  #endif 

  public:
    BoundedBuffer( const unsigned int size = 10 ) : size(size), num(0) 
    {
      buffer = new T[size];
      owner = new uOwnerLock();
      prodlock = new uCondLock();
      conslock = new uCondLock(); 

      prod_i = 0;
      cons_i = 0;
    }
    void insert( T elem );
    T remove();
    ~BoundedBuffer()
    {
      delete owner;
      delete prodlock;
      delete conslock;
      delete [] buffer;
    }
};

#ifdef BUSY                            // busy waiting implementation
// implementation

template <typename T>
void BoundedBuffer<T>::insert(T elem)
{
  std::cout << "INSERTING: " << elem << std::endl; // DEBUGGING
  // Perform assert 
  owner->acquire(); 
  while (num >= size)
  {
    // Busy wait
    prodlock->wait(*owner);
  }
  owner->release(); 

  // actual insertion 
  owner->acquire();
  buffer[prod_i] = elem; 
  num++; 
  inc_i(prod_i);

  // signal
  if (num == 1)
    conslock->broadcast(); 

  owner->release(); 
}

template <typename T>
T BoundedBuffer<T>::remove()
{
  std::cout << "ATTEMPT REMOVE INDEX " << num << std::endl; // DEBUGGING

  // perform assert
  owner->acquire(); 
  while (num == 0)
  {
    //busy wait
    conslock->wait(*owner);
  }
  
  std::cout << "SUCCESSFUL REMOVE INDEX " << num << std::endl; // DEBUGGING

  // get remove value
  int val = buffer[cons_i];

  // actual removal 
  num--; 
  inc_i(cons_i);

  // signal
  if (num == size - 1)
    prodlock->broadcast(); 

  owner->release();
  return val;
}
#endif // BUSY

#ifdef NOBUSY                          // no busy waiting implementation
// implementation
template <typename T>
void BoundedBuffer<T>::insert(T elem)
{

  // Perform assert 
  owner->acquire(); 

  // check for barging 
  if (signal_is_occuring)
  {
    prodlock->wait(*owner); 
    signal_is_occuring = false;
  }

  if (num >= size)
  {
    // Non busy wait
    prodlock->wait(*owner);
    signal_is_occuring = false;
  }
  owner->release(); 

  // actual insertion 
  owner->acquire();
  buffer[prod_i] = elem; 
  num++; 
  inc_i(prod_i);

  // signal
  if (!conslock->empty())
  {
    signal_is_occuring = true;
    conslock->signal(); 
  }


  owner->release(); 
}

template <typename T>
T BoundedBuffer<T>::remove()
{
  // perform assert
  owner->acquire(); 

  // check for barging 
  if (signal_is_occuring)
  {
    conslock->wait(*owner); 
    signal_is_occuring = false;
  }

  if (num == 0)
  {
    //nonbusy wait
    conslock->wait(*owner);
    signal_is_occuring = false;
  }

  // get remove value
  int val = buffer[cons_i];

  // actual removal 
  num--; 
  inc_i(cons_i);

  if (prodlock->empty() == false)
  {
    signal_is_occuring = true;
    // signal
    prodlock->signal();
  }


  owner->release();
  return val;
}
#endif // NOBUSY

_Task Producer {
    BoundedBuffer<int> &buffer; 
    const int Produce; 
    const int Delay; 

    bool SentinelInserter; 

    void main()
    {
      for (int i = 1; i <= Produce; i++)
      {
        // first yield 
        MPRNG rng = MPRNG();
        int numYields = rng(Delay);
        yield(numYields);
        buffer.insert(i);
      }
    }
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay ) : 
      buffer(buffer), Produce(Produce), Delay(Delay) { SentinelInserter = false; } 

    //Producer( BoundedBuffer<int> &buffer, )
};

_Task Consumer {
    BoundedBuffer<int> &buffer; 
    const int Delay, Sentinel;
    int &sum;

    void main()
    {
      while (true)
      {
        MPRNG rng = MPRNG();
        int numYields = rng(Delay);
        yield(numYields); 

        const int result = buffer.remove(); 
        //std::cout << "Result: " << result << " Sentinel: " << Sentinel << std::endl; // debugging
        if (result == Sentinel)
          return;

        sum += result; 
      }
    }
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum ): 
      buffer(buffer), Delay(Delay), Sentinel(Sentinel), sum(sum) {} 
};
