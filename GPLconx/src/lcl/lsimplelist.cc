#include <lsimplelist.h>
#include <iostream.h>
#include <malloc.h>

#define DEBUG
#include <ldebug.h>

LSimpleList::LSimpleList ()
{
  head = tail = NULL;
  locked = false;
}

LSimpleList::LSimpleList ( LSimpleList const & p1 )
{
  cout << "SIMPLELIST copy ctor\n";

  head = tail = NULL;
  locked = p1.locked;

  Container * node = p1.head;
  Container * temp;

  while ( node ) {
    temp = new Container ( *node );
    append ( temp );
    node = node->next;
  }
}

LSimpleList::~LSimpleList ()
{  
  clear ();
}

void LSimpleList::append ( void * ptr )
{
  Container * node = new Container;
  node->ptr = ptr;

  if ( tail ) {
    node->prev = tail;
    node->next = NULL;
    tail->next = node;
    tail = node;
  }  else {
    node->prev = NULL;
    node->next = NULL;
    tail = node;
    head = node;
  }

}

void LSimpleList::remove ( void * ptr )
{
  if ( head == NULL )
    return;

  Container * node = head;
  Container * temp;

  while ( node ) {

    temp = node->next;

    // Check if this is a node to process
    if (node->ptr == ptr) {

      if ( locked ) {

	node->ptr = NULL;
	//	cout << "Node zeroed (" << (void*)node << "\n";
	cout.flush();

      } else {

	// Remove this node
	
	if ( node->prev ) {
	  node->prev->next = node->next;	
	} else {
	  if (node->next)
	    node->next->prev = NULL;
	  head = node->next;
	}
	
	if ( node->next ) {
	  node->next->prev = node->prev;
	} else {
	  if ( node->prev )
	    node->prev->next = NULL;
	  tail = node->prev;	
	}
	
	// Delete Container
	//	cout << "Delete node " << (void*)node << "\n";
	//	cout.flush();
	delete node;

      } // Node deleted

    }
      
    //    node = node->next;
    node = temp;
  }
}

void LSimpleList::clear ()
{
  Container * node, *temp;

  node = head;

  while ( node ) {    
    temp = node->next;
    delete node;
    node = temp;
  }

  head = tail = NULL;  

}

void LSimpleList::forEach ( void (*func)(void*) )
{
  Container * node;
  
  node = head;

  while (node) {
    if (node->ptr)
      func ( node->ptr );
    node = node->next;
  }

  cleanUp ();
}

void LSimpleList::cleanUp ( )
{
  if ( !locked )
    remove(NULL);
}

void LSimpleList::lock ()
{
  locked = true;
}

void LSimpleList::unlock ()
{
  locked = false;
}
