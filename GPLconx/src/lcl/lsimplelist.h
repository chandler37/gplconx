#ifndef _LSIMPLELIST_H_
#define _LSIMPLELIST_H_

/**
 * Hello. I'm a rather simple doc about this class.
 */
class LSimpleList {
 protected:
  
  class Container {
    friend class LSimpleList;
  protected:
    Container * next, *prev;
    void * ptr;
  };

  Container *head, *tail;
  bool locked;

 public:
  LSimpleList ();
  LSimpleList ( LSimpleList const & p1 );
  virtual ~LSimpleList ();

  void append ( void * );
  void remove ( void * );

  void clear ();
  void cleanUp ();

  void lock ();
  void unlock ();
  
  void forEach ( void (*func)(void*) );
};

#endif
