#ifndef _LBASEOBJECT_H_
#define _LBASEOBJECT_H_

class LBaseObject {
 protected:
  static unsigned long instCount;
 public:
  LBaseObject ();
  ~LBaseObject ();

  static unsigned long getInstCount (); 

};

#endif
