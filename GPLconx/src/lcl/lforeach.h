/* ****************************************************************************
   |      File: lforeach.h
   |      Contains: base class definition for LForEach
   |      Created: 01.03.1998
   |      Last Changed: 19.03.1998
   |      Version: 0.0
   |         
   |      ToDo: + just started
   |
   ************************************************************************* */

/* ====>> See also the ChangeLog in lforeach.cc <<==== */

#ifndef _L_FOREACH_H_
#define _L_FOREACH_H_

#include "lcl.h"
#include "lobject.h"

class LForEach : public LObject {

  CLASSNAME("LForEach");
  
 public:
  virtual void process ( void * value );
};

#endif
