/* ****************************************************************************
   |      File: ldict.h
   |      Contains: class definition for LDictionary
   |      Created: 25.02.1998
   |      Last Changed: 25.02.1998
   |      Version: 0.0
   |         
   |      ToDo: + just started
   |
   |
   ************************************************************************* */

/* ====>> See also the ChangeLog in lvallist.cc (n/a) <<==== */
/*
 * -> [v0.0] - 25.02.1998 - Create file and started coding
 * -> [v0.0] - 28.02.1998 - Added several Funcs
 * -> [v0.0] - 01.03.1998 - Added LForEach support
 * -> [v0.0] - 18.03.1998 - Added copy construtor
 * -> [v0.0] - 24.05.1998 - Added LForEach item function (workaround!)
 */

#ifndef _L_DICT_H_
#define _L_DICT_H_

#include <lobject.h>
#include <ldebug.h>
#include <lforeach.h>

//class LException;

template <class Key, class Value>
class LDictContainer {
 public:

  Key key;
  Value value;
  LDictContainer *next, *prev;

  LDictContainer(Key const & k, Value const & v) {
    key = k;
    value = v;
    next = prev = NULL;
  };
  LDictContainer ( const LDictContainer & p1 )
    {
      key = p1.key;
      value = p1.value;
      next = prev = NULL;
    }
};

template <class Key, class Value>
class LDictionary : public LObject {
CLASSNAME("LDictionary");
 protected:
  LDictContainer<Key,Value> * head, *tail;
  unsigned long items;

  LDictContainer<Key,Value> * findContainer(Key) const;

  void add ( Key, Value );
  void addContainer ( LDictContainer<Key,Value> * );

 public:

  LDictionary();
  LDictionary( const LDictionary & );
  ~LDictionary();

  void clear();

  void set ( Key, Value );
  Value get ( Key const &) const; 
  int hasKey ( Key ) const;
  void remove ( Key );

  unsigned long entries() const {return items;};

  void forEach ( LForEach * );
  void forEachItem ( LForEach * );

  void debug();

  const LDictionary & operator=(const LDictionary &);
};

template <class Key, class Value>
inline LDictionary<Key,Value>::LDictionary ()
{
  items = 0;
  head = tail = NULL;
}

template <class Key, class Value>
inline LDictionary<Key,Value>::LDictionary ( const LDictionary<Key,Value> & p1 )
{

  items = 0;
  head = tail = NULL; 

  if ( p1.head != NULL ) {

    LDictContainer<Key,Value> * current = p1.head;
    LDictContainer<Key,Value> * temp;
    while ( current ) {
      temp = new LDictContainer<Key,Value>(*current);
      //      *temp = *current;
      addContainer ( temp );
      current = current->next;
    }

  }
}

template <class Key, class Value>
inline LDictionary<Key,Value>::~LDictionary ()
{
  clear();
}

template <class Key, class Value>
inline void LDictionary<Key,Value>::clear ()
{
  LDictContainer<Key,Value> *temp, * current = head;


  // *** Walk down the tree and remove all items
  while (current!=NULL) {
    temp = current->next;
    delete current;
    current = temp;
  }

  // *** The Dict is now empty
  head = tail = NULL;
  items = 0;
}

template <class Key, class Value>
inline void LDictionary<Key,Value>::addContainer ( LDictContainer<Key,Value> * container )
{
  if ( head == NULL ) {

    // *** This is item #1
    head = tail = container;

  } else {
    
    // *** This is item #x
    container->prev = tail;
    tail->next = container;
    tail = container;

  }

  // *** We added a container
  items++;
}

template <class Key, class Value>
inline void LDictionary<Key,Value>::add (Key k, Value v)
{
  LDictContainer<Key,Value> * container = new LDictContainer<Key,Value>(k, v);

  addContainer ( container );

  /*  if ( head == NULL ) {

    // *** This is item #1
    head = tail = container;

  } else {
    
    // *** This is item #x
    container->prev = tail;
    tail->next = container;
    tail = container;

  }

  // *** We added a container
  items++;*/
}

template <class Key, class Value>
inline LDictContainer<Key,Value> * LDictionary<Key,Value>::findContainer ( Key k ) const
{
  LDictContainer<Key,Value> * current = head;  

  while ( current != NULL ) {
    if ( current->key == k )
      return current;
    current = current->next;
  }
  return NULL;
}

template <class Key, class Value>
Value LDictionary<Key,Value>::get ( Key const & k ) const
{
  LDictContainer<Key,Value> * container = findContainer(k);

  if ( container == NULL ) {
    //    throw LException("Key not found in Dictionary");
    throw 0;
    MSTHROW(Value);
  } else {
    return container->value;
  }
}

template <class Key, class Value>
int LDictionary<Key,Value>::hasKey ( Key k ) const
{
  return findContainer(k) != NULL;
}

template <class Key, class Value>
void LDictionary<Key,Value>::set ( Key k, Value v )
{
  LDictContainer<Key,Value> * container = findContainer(k);

  if ( container != NULL ) {
    container->value = v;
  } else {
    add(k,v);
  }
}

template <class Key, class Value>
void LDictionary<Key,Value>::debug ( )
{
  LDictContainer<Key,Value> * current = head;  

  while ( current != NULL ) {
    cout << "Key: " << current->key << " \t\t Value: " << current->value << "\n";
    current = current->next;
  }
}

template <class Key, class Value>
void LDictionary<Key,Value>::remove ( Key k )
{
  LDictContainer<Key,Value> * container = findContainer ( k );

  if ( container != NULL ) {
    if ( container->prev ) {
      container->prev->next = container->next;
    }
    if ( container->next ) {
      container->next->prev = container->prev;
    }
    
    // *** We removed one container
    items--;

    // *** Check if dictionary is empty now
    if ( items==0 )
      head = tail = NULL;

    // *** and free space
    delete container;
  }
}

template <class Key, class Value>
void LDictionary<Key,Value>::forEach ( LForEach * foreach )
{
  LDictContainer<Key,Value> * current = head;

  while ( current != NULL ) {
    foreach->process ( &current->value );
    current = current->next;
  }

  delete foreach;
}

template <class Key, class Value>
void LDictionary<Key,Value>::forEachItem ( LForEach * foreach )
{
  LDictContainer<Key,Value> * current = head;

  while ( current != NULL ) {
    foreach->process ( current );
    current = current->next;
  }

  delete foreach;
}

template <class Key, class Value>
const LDictionary<Key,Value> & LDictionary<Key,Value>::operator=(const LDictionary<Key,Value> & p1 ) 
{
  clear();

  if ( p1.head == NULL ) {
    items = 0;
    head = tail = NULL; 
  } else {

    LDictContainer<Key,Value> * current = p1.head;
    LDictContainer<Key,Value> * temp;
    while ( current ) {
      temp = new LDictContainer<Key,Value>(*current);
      *temp = *current;
      addContainer ( temp );
      current = current->next;
    }

  }  
  return *this;
}

#endif
