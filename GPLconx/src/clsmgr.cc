/*
    GPLconx -- visualize 2-D hyperbolic geometry.
    Copyright (C) 1996-2001  David L. Chandler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
  Implementation of C++ classes in `clsmgr.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "clsmgr.hh"
#include "stcommon.hh"
#include "stconx.hh"
#include "sth_var.hh"
#include "sth_assn.hh"

NF_INLINE
long CConxClsManager::makeAssignments(const CConxClsAssignmentList &al,
                                      CClsBase *assignTo)
  // Returns -1 if assignTo is not a valid instance.
  // Returns the number of failures due to attempting to assign to classes,
  // or zero on success.
{
  MMM("long makeAssignments(const CConxClsAssignmentList &al, CClsBase *assignTo)");
  if (assignTo == NULL) return -1;
  long retval = 0;
  //  cout << "DLC alist is " << flush << al << endl;
  for (size_t i = 0; i < al.getNumberVars(); i++) {
    if (makeAssignment(al.getNthVar(i), assignTo))
      ++retval;
  }
  return retval;
}

NF_INLINE
Boole CConxClsManager::makeAssignment(const CConxString &id,
                                      CClsBase *v) throw(const char *)
{
  MMM("Boole makeAssignment(const CConxString &id, CClsBase *v) throw(const char *)");

  LLL("assigning " << id << " to " << v << " which prints as:" << *v);

  if (v == NULL) throw ("never");

  if (isReservedWord(id.getString())) {
    return TRUE; // Do not allow `Float := 3.3', e.g.
  }

  // If we are reassigning, then do garbage collection.

  // Be careful not to delete what we are about to point to.
  LLL("makeAssignment is about to void incrementUsers()");
  v->incrementUsers();

  CClsBase *ref;
  CConxClsVariable *varPtr;
  if (isVariableName(id)) {
    ref = getModifiableNamedThing(id);
    removeUser(ref); // GC
    varPtr = getVariable(id);
  } else {
    varPtr = newVariable(FALSE, id);
  }
  assert(varPtr != NULL);
  varPtr->setPointee(v);
  return FALSE;
}

CF_INLINE
CConxClsManager::~CConxClsManager()
{
  MMM("~CConxClsManager()");
  // EEEk DLC DLC DLC This is not trivial.

  // Do garbage collection.

  // Undo the effects of all the addInstance() calls and the firstInit()
  // call.

  unsigned long asz = array.size();
  for (unsigned long i = 0; i < asz; i++) {
    CClsBase *bptr = array.get(i);
    if (bptr != NULL) {
      // Relies on the virtual destructor provided by CClsBase:
      LLL("~CConxClsManager 650 " << i << " about to delete "
          << bptr << " which is " << "of type "
          << CClsBase::clsTypeToString(bptr->getType())
          << " which prints as " << *bptr << endl);
      delete bptr;
    }
  }

  // DLC kill dict so that the variables's pointees have their GC counts
  // decremented.
}

CF_INLINE
CConxClsManager::CConxClsManager()
{
  MMM("CConxClsManager()");
  init();
}

NF_INLINE
void CConxClsManager::reportParsingError(const char *s)
{
  ++numParseErrors;
}

NF_INLINE
void CConxClsManager::zeroParsingErrors()
{
  numParseErrors = 0;
}

NF_INLINE
void CConxClsManager::clear()
{
  abort(); // DLC the GC needs to be used, and then all the first-class
  // entities need to be removed.

  dict.clear(); // DLC use class with ~ that calls
  // `delete (ptr_type) x->ptr' to stop this memory leak
  array.clear();
  reservedWords.clear();
  isInitialized = FALSE;
}

NF_INLINE
void CConxClsManager::firstInit(CConxCanvas *pdCanvas = NULL,
                                CConxCanvas *kdCanvas = NULL,
                                CConxCanvas *uhpCanvas = NULL)
{
  MMM("firstInit()");
  if (!isInitialized) {
    // Load a Float class, a Circle class, a true object instance, a false object instance, a ...
    // Load a System instance, a Klein instance, a ...

    loadStartupClassInstances();
    loadStartupObjectInstances(pdCanvas, kdCanvas, uhpCanvas);
    loadReservedWords();

    isInitialized = TRUE;
  }
}

NF_INLINE
void CConxClsManager::reinit(CConxCanvas *pdCanvas = NULL,
                             CConxCanvas *kdCanvas = NULL,
                             CConxCanvas *uhpCanvas = NULL)
{
  MMM("reinit()");
  if (isInitialized) {
    clear();
  }
  firstInit(pdCanvas, kdCanvas, uhpCanvas);
}

NF_INLINE
int CConxClsManager::sendUnaryMessage(CClsBase **result, CClsBase *receiver,
                                      const CConxString &msg)
{
  MMM("int sendUnaryMessage(CClsBase **result, CClsBase *receiver, const CConxString &msg)");
  if (receiver == NULL) return -3;

  CConxClsMessage tmpM(msg);
  CClsBase *resPtr = NULL;
  CClsBase::ErrType code = receiver->sendMessage(&resPtr, tmpM);
  assert(resPtr != NULL);
  *result = resPtr;
  if (code == CClsBase::OK_NEW_THING) {
    // We need to take charge of this fellow and put him under GC.
    addNewInstance(resPtr);
  }
  return ((code == CClsBase::OK || code == CClsBase::OK_NEW_THING)
          ? 0 : 1);
}

NF_INLINE
Boole CConxClsManager::isVariableName(const CConxString &s) const
{
  DEBUG_DICT();
  return dict.hasKey(s);
}

NF_INLINE
const CClsBase *CConxClsManager::getNamedThing(const CConxString &id) const
{
  return constGetNamedThing(id);
}

NF_INLINE
CClsBase *CConxClsManager::getModifiableNamedThing(const CConxString &id)
{
  return constGetNamedThing(id);
}

NF_INLINE
Boole CConxClsManager::pointsToClassInstance(const CConxString &id) const
{
  CClsBase *p = constGetNamedThing(id);
  return (p != NULL && p->isClassInstance());
}

NF_INLINE
CClsFloat *CConxClsManager::newInstance(double f) throw(int, const char *)
{
  MMM("CClsFloat *newInstance(double f) throw(int)");
  CClsFloat *nf = new CClsFloat(f); // 0 users.
  if (nf == NULL) OOM();
  addNewInstance(nf);
  return nf;
}

NF_INLINE
CClsSmallInt *CConxClsManager::newInstance(long l) throw(int, const char *)
{
  MMM("CClsFloat *newInstance(long l) throw(int)");
  CClsSmallInt *nf = new CClsSmallInt(l); // 0 users.
  if (nf == NULL) OOM();
  addNewInstance(nf);
  return nf;
}

NF_INLINE
CConxObject *CConxClsManager::newInstance(const char *s, CClsBase::ClsType t)
  throw(int, const char *)
{
  MMM("CConxObject *newInstance(const char *s, CClsBase::ClsType t)");
  if (t == CClsBase::CLS_VARIABLE) {
    // New variables always point to a new object instance of
    // class UndefinedObject, i.e. to a newly allocated `nil'.
    // makeAssignment will not call this since it is wasteful.
    return newVariable(TRUE, s);
  }
  CClsBase *newObject;
  if (t == CClsBase::CLS_SYMBOL) {
    newObject = new CClsSymbol(s); // 0 users.
  } else if (t == CClsBase::CLS_STRING) {
    newObject = new CClsStringLiteral(s); // 0 users.
  } else if (t == CClsBase::CLS_ERROR) {
    newObject = new CClsError(s); // 0 users.
  } else
    throw 1;
  if (newObject == NULL) OOM();
  addNewInstance(newObject);
  return newObject;
}

NF_INLINE
const CClsBase *CConxClsManager::getLastResult() const
{
  return constGetNamedThing("ans");
}

NF_INLINE
CConxClsVariable *CConxClsManager::newVariable(Boole set,
                                               const char *s) throw(int)
{
  MMM("CConxClsVariable *newVariable(Boole set, const char *s) throw(int)");
  if (s == NULL) throw 0;
  CConxClsVariable *nv = new CConxClsVariable(s);
  if (nv == NULL) OOM();
  if (set) {
    CClsBase *nnil = new CClsUndefinedObject();
    if (nnil == NULL) OOM();
    // no need to addNewInstance(nnil) -- we have a first-class object
    // pointing to him, so he doesn't need to be first-class himself.
    nnil->incrementUsers(); // zero -> one user
    nv->setPointee(nnil); // two users, right?
  }
  setVariable(s, nv);
  // DLC For GC, anything in the variable dictionary is a first-class
  // object.  We thus avoid `addNewInstance(nv)'
  return nv;
}

NF_INLINE
void CConxClsManager::loadStartupClassInstances()
{
  Boole r;
  CClsBase *bp;

#define LOAD_CLASS_INSTANCE(type) \
    bp = new type(); \
    if (bp == NULL) OOM(); \
    bp->setClassInstance(TRUE); \
    r = makeAssignment(bp->getClsName(), bp); \
    assert(!r); \
    addReservedWord(bp->getClsName())

#define LOAD_ALIAS_FOR_THAT_CLASS_INST(alias) \
  r = makeAssignment(alias, bp); \
  assert(!r); \
  addReservedWord(alias)

  LOAD_CLASS_INSTANCE(CClsModelIdentifier);
  LOAD_CLASS_INSTANCE(CClsColor);
  LOAD_CLASS_INSTANCE(CClsCharacterArray);
  LOAD_CLASS_INSTANCE(CClsArray);
  LOAD_CLASS_INSTANCE(CClsSmallInt);
  LOAD_CLASS_INSTANCE(CClsDrawable);
  LOAD_CLASS_INSTANCE(CClsNumber);
  LOAD_CLASS_INSTANCE(CClsSystem);
  ((CClsSystem *)bp)->setClsMgr(this);
  LOAD_CLASS_INSTANCE(CClsCanvas);
  LOAD_CLASS_INSTANCE(CClsLine);
  LOAD_CLASS_INSTANCE(CClsCircle);
  LOAD_CLASS_INSTANCE(CClsParabola);
  LOAD_CLASS_INSTANCE(CClsHypEllipse);
  // HypEllipse is hard to remember, so we'll set up two other names for this
  // class:
  LOAD_ALIAS_FOR_THAT_CLASS_INST("Hyperbola");
  LOAD_ALIAS_FOR_THAT_CLASS_INST("Ellipse");

  // DLC NEWSTCLASS
  LOAD_CLASS_INSTANCE(CClsBase);
  LOAD_CLASS_INSTANCE(CClsFloat);
  LOAD_CLASS_INSTANCE(CClsSymbol);
  LOAD_CLASS_INSTANCE(CClsStringLiteral);
  LOAD_CLASS_INSTANCE(CClsPoint);
  LOAD_CLASS_INSTANCE(CClsBoolean);
  LOAD_CLASS_INSTANCE(CClsError);
  LOAD_CLASS_INSTANCE(CClsUndefinedObject);

}

NF_INLINE
void CConxClsManager::loadStartupObjectInstances(CConxCanvas *pdCanvas,
                                                 CConxCanvas *kdCanvas,
                                                 CConxCanvas *uhpCanvas)
{
  Boole r;
  CClsBase *bp;

#define LOAD_OBJECT_INSTANCE(type, name) \
    bp = new type(); \
    bp->setClassInstance(FALSE); \
    bp->setReadOnly(TRUE); \
    r = makeAssignment(name, bp); \
    assert(bp->getNumUsers() == 1); \
    addReservedWord(name); \
    assert(!r)

  // NULL CConxCanvas pointers are fine -- it just means some 
  // things won't work.
  LOAD_OBJECT_INSTANCE(CClsCanvas, "kdc");
  ((CClsCanvas *)bp)->setValue(kdCanvas);
  ((CClsCanvas *)bp)->setReadOnly(FALSE);
  LOAD_OBJECT_INSTANCE(CClsCanvas, "pdc");
  ((CClsCanvas *)bp)->setValue(pdCanvas);
  ((CClsCanvas *)bp)->setReadOnly(FALSE);
  LOAD_OBJECT_INSTANCE(CClsCanvas, "uhpc");
  ((CClsCanvas *)bp)->setValue(uhpCanvas);
  ((CClsCanvas *)bp)->setReadOnly(FALSE);

  LOAD_OBJECT_INSTANCE(CClsModelIdentifier, "kd");
  ((CClsModelIdentifier *)bp)->setValue("kd");
  assert(((CClsModelIdentifier *)bp)->model() == CClsModelIdentifier::KD);

  LOAD_OBJECT_INSTANCE(CClsModelIdentifier, "pd");
  ((CClsModelIdentifier *)bp)->setValue("pd");
  assert(((CClsModelIdentifier *)bp)->model() == CClsModelIdentifier::PD);

  LOAD_OBJECT_INSTANCE(CClsModelIdentifier, "uhp");
  ((CClsModelIdentifier *)bp)->setValue("uhp");
  assert(((CClsModelIdentifier *)bp)->model() == CClsModelIdentifier::UHP);

  LOAD_OBJECT_INSTANCE(CClsBoolean, "true");
  ((CClsBoolean *)bp)->setValue(TRUE);
  LOAD_OBJECT_INSTANCE(CClsBoolean, "false");
  ((CClsBoolean *)bp)->setValue(FALSE);

  LOAD_OBJECT_INSTANCE(CClsUndefinedObject, "nil");
}

NF_INLINE
void CConxClsManager::loadReservedWords()
{
  // We do everything inline thus far.
}

NF_INLINE
Boole CConxClsManager::isReservedWord(const char *w) const
{
  size_t sz = reservedWords.size();
  for (size_t i = 0; i < sz; i++) {
    if (reservedWords.get(i) == w) return TRUE;
  }
  return FALSE;
}

NF_INLINE
void CConxClsManager::addReservedWord(const char *w)
{
  reservedWords.append(w);
}

NF_INLINE
void CConxClsManager::removeUser(CClsBase *pp) throw(const char *)
{
  MMM("void removeUser(CClsBase *pp) throw(const char *)");
  if (pp == NULL) throw "why a NULL arg?";
  if (pp->decrementUsers() == 0) {

    LLL("@DLC Parser GC at work on an object of type "
        << CClsBase::clsTypeToString(pp->getType())
        << "\nthat prints as `" << pp->printString() << "'@" << pp << "\n");
    // DLC test L := 3.3. L := 4.4 to make sure this is called.   DLC L. L := 3.3. 3.3 ^-D

    // Find this item in the array and delete it.  We don't have to look
    // for it in the dictionary because if a variable referred to it then
    // we wouldn't be deleting it!
    if (array.size() > 0) {
      size_t k;
      CClsBase *r = NULL;
      for (k = 0; k < array.size(); k++) {
        r = array.get(k);
        if (r == pp) {
          array.deleteEntry(k);
          break;
        }
      }
      if (r != pp) {
        // pp is not a first class object, so we'll wait for the GC
        // to deal with it.  (We've already decremented its user count).
#ifdef DLC
        // Yeah, I know garbage collection has failed!
        throw PACKAGE " Garbage Collection: Cannot remove a user from an unknown object.";
#else
        LLL("DLC GC needs to be implemented!");
#endif
      } else {
        // DLC how could we get here?  We can only get the user
        // count of a first-class object down to 1, so we can't be here!
        abort();

#ifndef NDEBUG
        // k1 = k, not k+1, because deleteEntry(4) makes a[5] become a[4]
        for (size_t k1 = k; k1 < array.size(); k1++) {
          if (array.get(k1) == pp) {
            cerr << "\n Bad bad bad: " << array.get(k1) << endl;
          }
          assert(array.get(k1) != pp);
        }
#endif
        delete pp;
      }
    }
  }

#ifdef DLC
  // Set the dangling pointer to NULL.
  if (r->ptr_type == CClsBase::CLS_VARIABLE) {
    CConxClsVariable *vv = (CConxClsVariable *)r.ptr;
    vv->setPointee(NULL);
  } else {
    r.ptr = NULL;
  }
#endif
}

NF_INLINE
void CConxClsManager::setVariable(const CConxString &s, CConxClsVariable *r)
{
  MMM("void setVariable(const CConxString &s, CConxClsVariable *r)");
  DEBUG_DICT();
  dict.set(CConxString(s), r);
  // DLC GC this points to a first-class object!
  DEBUG_DICT();

  assert(isVariableName(CConxString(s)));
  // Equal but not the very same string
  // to make sure that dictionaries work.
  assert(dict.get(CConxString(s)) == r);
}

NF_INLINE
CConxClsVariable *CConxClsManager::getVariable(const CConxString &id)
{
  if (isVariableName(id)) {
    return dict.get(id);
  }
  return NULL;
}

NF_INLINE
CClsBase *CConxClsManager::constGetNamedThing(const CConxString &id) const
{
  MMM("CClsBase *constGetNamedThing(const CConxString &id) const");
  if (isVariableName(id)) {
    assert((dict.get(id))->getPointee() != NULL);
    return (dict.get(id))->getPointee();
  } else {
    return NULL;
  }
}

NF_INLINE
void CConxClsManager::addNewInstance(CClsBase *p)
{
  MMM("void addNewInstance(CClsBase *p)");

  if (p == NULL) throw "Hey, that's impossible!";
  // DLC please work! MARK1
  p->incrementUsers();
  LLL("appending " << p << " to the clsmgr array");
  array.append(p); // DLC does this copy???
  // DLC GC increment here???
}

NF_INLINE
void CConxClsManager::init()
{
  numParseErrors = 0;
  isInitialized = FALSE;
}


OOLTLTI_INLINE P_STREAM_OUTPUT_SHORTCUT(CConxClsManager)

