#include <lbaseobject.h>

unsigned long LBaseObject::instCount = 0;

LBaseObject::LBaseObject ()
{
  instCount++;
}

LBaseObject::~LBaseObject ()
{
  instCount--;
}


unsigned long LBaseObject::getInstCount ()
{
  return instCount;
}
