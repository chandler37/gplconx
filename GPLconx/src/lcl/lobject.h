#ifndef _L_OBJECT_H_
#define _L_OBJECT_H_

#include <lcl.h>
#include <lsimplelist.h>
#include <lbaseobject.h>

#include <sys/types.h>
#include <malloc.h>
#include <string.h>

/**
 * \class LObject
 * \brief Base class for all LCL objects
 * \author Jens Reimann <ctron@dentrassi.de>
 * \version 0.1
 * \date 1999
 * \bug Deriving all classes from one is silly :) (rikkus)
 * \warning This class has not made the CE standard. It will explode your
 * monitor into your face, showering you with broken class, if used
 * incorrectly. Instructions to follow.
 *
 * A link to LSimpleList
 */
class LObject : public LBaseObject {
 protected:

  static int num;

  LSimpleList childList;
  LObject * parent;

 public:
  CLASSNAME("LObject");

  LObject ();
  LObject ( LObject * parent );
  LObject ( const LObject & );

  virtual ~LObject();

  LObject * setParent ( LObject * = NULL );
  LObject * getParent ( ) const ;

  static int getNum ();
};

#endif
