/* ****************************************************************************
   |      File: lobject.cc
   |      Contains: base class LObject
   |      Created: 16.03.1998
   |      Last Changed: 16.03.1998
   |      Version: 0.0
   |         
   |      ToDo: + (global) add some real exceptions !!
   |
   ************************************************************************* */

/* ************************************************************************* *
 * Change log:                                                               *
 * -------------                                                             *
 *                                                                           *
 *   -> [v0.00] - <unkown>   - Created file and started coding               *
 *   -> [v0.00] - 16.04.1998 - Added auto remove                             *
 *   -> [v0.00] - 23.08.1998 - Added Parent functions                        *
 *                                                                           *
 * ************************************************************************* */

#define _L_OBJECT_CC_

#include <iostream.h>
#include "lobject.h"

int LObject::num = 0;

void destroyChildren ( void * ptr )
{
  LObject * obj = (LObject *) ptr;
  delete obj;
}

LObject::LObject () :
  parent ( NULL )
{
  num++;
}

LObject::LObject ( LObject * parent ) :
  parent ( NULL )
{
  setParent ( parent );
  num++;
}

LObject::LObject ( const LObject & p1 ) :
  parent ( NULL )
{
  setParent ( p1.parent );
  num++;
}

LObject * LObject::setParent ( LObject * newparent )
{  
  LObject * oldParent = parent;
  
  if ( parent ) {
    //  cout << "Parent (" << (void*)parent << ") reset!\n";
    parent->childList.remove ( this );
    parent = NULL;
  }

  if ( newparent ) {
    //    cout << "Parent set!\n";
    parent = newparent;
    parent->childList.append ( this );
  }

  return oldParent;
}

LObject * LObject::getParent ( ) const
{
  return parent;
}

int LObject::getNum ()
{
  return num;
}

LObject::~LObject ()
{

  // First Remove from out parent list!!
  setParent ( );

  // Destroy all our children
  childList.lock ();
  childList.forEach ( destroyChildren );
  childList.unlock ();
  childList.clear ();

  //  if ( !appMain.isDestructing )
  //    appMain.removeDestObject ( this );
  num--;
}
