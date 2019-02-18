//  Copyright (c) 2007 by Princeton University
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0.
//
//  file : AtomicPtr.h
//  author : Christian Bienia - cbienia@cs.princeton.edu
//  description : An atomic datatype

// IMPORTANT INFORMATION REGARDING USAGE OF ATOMIC POINTERS:
//
// While an atomic pointer itself can be manipulated through
// the provided methods and operators without having to worry
// about race conditions, this does not imply that the data
// pointed to can be worked with in a similar manner without
// taking further precautions.
//
// For example, the following code is incorrect:
//
// AtomicPtr<int> p(&my_int);
// ...
// (*my_int)++;
//
// The pointer to the integer stored inside p is protected
// against concurrent accesses, but the data stored in my_int
// isn't.
//
// Furthermore, users of the atomic pointer class should not
// assume the pointer remains unchanged between accesses.
//
// For example, the following code is also wrong:
//
// AtomicPtr<my_struct_t> p(&my_struct);
// ...
// int x = (*p).x;
// int y = (*p).y;
//
// Unless further steps have been taken, another thread might
// update p between the loads of x and y, and thus values
// belonging to two different structures are read.
//
// If the referenced data is never updated (i.e. no
// synchronization is required), the code can be rewritten
// to be safe by accessing the atomic pointer only once
// as follows:
//
// AtomicPtr<my_struct_t> p(&my_struct);
// ...
// my_struct_t *temp = p.Get();
// int x = temp->x;
// int y = temp->y;
//
// This has been the rationale behind the design decision to
// implement the dereference operator '*', but not the member
// select operator '->'. Member accesses via an atomic pointer
// are inherently non-atomic as a whole.
//
// The atomic pointer class can be used to synchronize accesses
// to the referenced data if two conditions hold:
//
// 1. Only one thread can have the atomic pointer at any time.
//    The atomic pointer will thus have the semantics of a unique
//    token in the system.
// 2. The correct memory barriers are added to prevent the system
//    from reordering the memory accesses and to achieve full
//    acquire / release semantics. This functionality is only
//    indirectly provided by the AtomicPtr class (see below).
//
// The AtomicPtr class provides two functions - Checkin() and
// Checkout() - which fulfill both conditions and allow the atomic
// pointer to function like a mutex. For each Checkout(), exactly
// one Checkin() must be executed.
//
// For example, a critical section updating the referenced data
// can be protected as follows:
//
// temp = p.Checkout();
// update(temp);
// p.Checkin(temp);
//
// While a pointer is checked out from its AtomicPtr container
// object, all accesses to the AtomicPtr object other than Checkin()
// and the Try*() functions will cause the calling thread to spin
// until a pointer has been checked in again. The Try*() functions
// will return false instead to indicate that the operation could not
// be completed and will return immediately without spinning.
//
// The Checkin() function allows the checkin of any pointer, i.e.
// the new pointer may be different from the one previously checked
// out.

#ifndef ATOMICPTR_H
#define ATOMICPTR_H

//uncomment to compile with additional error checks
#define NDEBUG

#include <cassert>

#ifdef ENABLE_THREADS
#include "atomic/atomic.h"
#endif



namespace threads {

template <typename T>
class AtomicPtr {
  private:
    //the pointer to access atomically and types to use for access (32-bit or 64-bit width)
#if defined(_LP64)
    typedef long unsigned int ATOMIC_TYPE;
    T *p __attribute__ ((aligned (8)));
#else
    typedef unsigned int ATOMIC_TYPE;
    T *p __attribute__ ((aligned (4)));
#endif

    //value to use internally to indicate a temporarily unaccessible pointer
    static const T *ATOMIC_NULL;

    //helper function to set the pointer to a value (without any checks)
    inline T *PrivateSet(T *x) {
      T *val;

#ifdef ENABLE_THREADS
      do {
        val = Get();
      } while(!atomic_cmpset_ptr((ATOMIC_TYPE *)&p, (ATOMIC_TYPE)val, (ATOMIC_TYPE)x));
#else
        val = Get();
        p = x;
#endif //ENABLE_THREADS

      return val;
    }

    //helper function to try to set the pointer to a value (without any checks)
    inline bool TryPrivateSet(T *x, T **y) {
      T *val;
      bool rv;

#ifdef ENABLE_THREADS
      if(!TryGet(&val)) {
        return false;
      }
      rv = atomic_cmpset_ptr((ATOMIC_TYPE *)&p, (ATOMIC_TYPE)val, (ATOMIC_TYPE)x);
      if(rv && (y != NULL)) {
        *y = val;
      }
      return rv;
#else
      assert(p != ATOMIC_NULL);
      *y = p;
      p = x;
      return true;
#endif //ENABLE_THREADS
    }

  public:
    // *** Constructors and Destructors ***

    //regular constructor
    AtomicPtr(T *x) {
      assert(x != ATOMIC_NULL);
      p = x;
    }

    //copy constructor
    AtomicPtr(const AtomicPtr<T> &X) {
      p = X.Get();
    }

    // *** Functions to modify and obtain encapsulated data ***

    //set the pointer to x, return the replaced value
    inline T *Set(T *x) {
      assert(x != ATOMIC_NULL);
      return PrivateSet(x);
    }

    //set the pointer to x, store the previous value in *y and return true
    //if the pointer isn't currently checked out, otherwise return false
    //if y is NULL, the previous is not stored in *y
    inline bool TrySet(T *x, T **y = NULL) {
      assert(x != ATOMIC_NULL);
      return TryPrivateSet(x, y);
    }

    //return the current value of the pointer
    inline T *Get() const {
      T *val;

#ifdef ENABLE_THREADS
      do {
        val = (T *)atomic_load_acq_ptr((ATOMIC_TYPE *)&p);
      } while(val == ATOMIC_NULL);
#else
      val = p;
      assert(val != ATOMIC_NULL);
#endif //ENABLE_THREADS

      return val;
    }

    //writes the current value of the pointer to *x and returns true
    //if the pointer isn't currently checked out, otherwise returns
    //false
    inline bool TryGet(T **x) const {
      T *val;

#ifdef ENABLE_THREADS
      val = (T *)atomic_load_acq_ptr((ATOMIC_TYPE *)&p);
#else
      val = p;
#endif //ENABLE_THREADS
      if(val != ATOMIC_NULL) {
        *x = val;
        return true;
      } else {
        return false;
      }
    }

    //swap pointers atomically and deadlock-free
    inline void Swap(AtomicPtr<T> &X) {
      //define partial order in which to acquire elements to prevent deadlocks
      AtomicPtr<T> *first;
      AtomicPtr<T> *last;
      //always process elements from lower to higher memory addresses
      if(this < &X) {
        first = this;
        last = &X;
      } else {
        first = &X;
        last = this;
      }

      //acquire and update elements in correct order
      T *valFirst = first->Checkout();
      T *valLast = last->PrivateSet(valFirst);
      first->Checkin(valLast);
    }

    //try to swap pointers atomically and deadlock-free, return true if successful,
    //false otherwise
    inline bool TrySwap(AtomicPtr<T> &X) {
      //define partial order in which to acquire elements to prevent deadlocks
      AtomicPtr<T> *first;
      AtomicPtr<T> *last;
      //always process elements from lower to higher memory addresses
      if(this < &X) {
        first = this;
        last = &X;
      } else {
        first = &X;
        last = this;
      }

      //acquire and update elements in correct order
      T *valFirst;
      T *valLast;
      if(!first->TryCheckout(&valFirst)) {
        return false;
      } else {
        if(!last->TryPrivateSet(valFirst, &valLast)) {
          first->Checkin(valFirst);
          return false;
        } else {
          first->Checkin(valLast);
          return true;
        }
      }
    }

    // *** Functions with mutex semantics ***

    //acquire a pointer for exclusive access (mutex lock semantics)
    inline T *Checkout() {
      //Note: PrivateSet() calls Get(), which has acquire semantics
      return PrivateSet((T *)ATOMIC_NULL);
    }

    //try to acquire a pointer for exclusive access, return true if
    //successful, false otherwise (mutex trylock semantics)
    inline bool TryCheckout(T **x) {
      //Note: TryPrivateSet() calls TryGet(), which has acquire semantics
      return TryPrivateSet((T *)ATOMIC_NULL, x);
    }

    //release an exclusive pointer (mutex unlock semantics)
    inline void Checkin(T *x) {
#ifdef ENABLE_TRHEADS
      atomic_store_rel_ptr((ATOMIC_TYPE *)(&p), (ATOMIC_TYPE)x);
#else
      p = x;
#endif //ENABLE_THREADS
    }


    // *** Operators ***


    T operator*() {
      return *Get();
    }

    T *operator=(T *x) {
      return Set(x);
    }

    T *operator=(AtomicPtr<T> X) {
#ifdef ENABLE_THREADS
      T *val = (T *)atomic_load_acq_ptr((ATOMIC_TYPE *)&X.p);
#else
      T * val = X.p;
#endif //ENABLE_THREADS
      Set(val);
      return val;
    }

};

//set ATOMIC_NULL to a value the user hopefully will never use
template<typename T>
const T *AtomicPtr<T>::ATOMIC_NULL((T *)((int)NULL + 1));

} //namespace threads

#endif //ATOMICPTR_H
