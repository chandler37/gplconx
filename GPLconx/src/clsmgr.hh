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
  C++ classes to manage the ``Smalltalk-like object hierarchy'' that we use
  when parsing.

  CCls... models a Smalltalk class; its instances are Smalltalk class
  intances and Smalltalk object instances.  CClsBase is the root
  of this hierarchy.  If you want to add a new one of these, search for
  the string NEWSTCLASS to find places you need to modify.  Then copy a class
  like CClsFloat and Replace Float with Foobar to get started.

  CConxCls... is a helper class.




  DLC begin
    Garbage colleciton is an issue.  If you want to delete instantly all of
    the time, you'll have so much bookkeeping to do and so many objects
    will have to know who their garbage collector is...
    
    We could allow instant deletion when it's handy, like an
       ClsPoint::~ClsPoint
       {
         if (xcoordinate != NULL) {
           if (xcoordinate->decrementUsers() == 0)
             delete xcoordinate;
         }
       }
    whenever it's easy, but also a tagging tracer that asks all first class
    objects, known to CConxClsManager, that is, which objects are you
    using?  They'll answer their dependents and then all of their dependents
    will answer, recursively.  That would make copying semantics for
    CConxClsManager easier also.

    It is not done yet, and 
  DLC end

*/
  // DLC make a better parse error message for `x1:=4.4'; make it a test case
  /* DLC
     We want both early and late bindings available so that we can do neat
     tricks like the following:

      $ x1 := 3.3. y1 := 3.4
      $ m1 := uhp
      $ pLate := Point x: x1 y: y1 m: m1
      $ pEarly := Point x: x1 clone y: y1 clone m: m1 clone
      $ m1 set: 'klein'. x1 set: 3.2
      $ pLate
      >>> ParseError: The point is out of range, ...
      $ pEarly
      >>> [... a valid point...]
  */

#ifndef GPLCONX_CLSMGR_CXX_H
#define GPLCONX_CLSMGR_CXX_H 1

#include <assert.h>

#include <ldict.h>

#include "CString.hh"
#include "geomobj.hh"
#include "Starray.hh"
#include "CArray.hh"
#include "stcommon.hh"
#include "stconx.hh"
#include "decls.hh"

// Low-priority efficiency issue: DLC do not make a Float instance for temporaries, i.e. add a double to the YYSTYPE union.


typedef CConxSimpleArray<CClsBase *> BPArray;


//////////////////////////////////////////////////////////////////////////////
// CConxClsManager holds all classes and objects for the
// ``Smalltalk-like object hierarchy'', so the parser interacts with it and
// not with the CCls... directly.
class CConxClsManager : VIRT public CConxObject, public CConxPrintable {
  CCONX_CLASSNAME("CConxClsManager")
  DEFAULT_PRINTON()
public: // operations
  CConxClsManager()
  {
    MMM("CConxClsManager()");
    isInitialized = FALSE;
  }
  ~CConxClsManager();

  int operator==(const CConxClsManager &o) const { return 0; } // DLC
  int operator!=(const CConxClsManager &o) const { return !operator==(o); }

  void clear()
  {
    abort(); // DLC the GC needs to be used, and then all the first-class
    // entities need to be removed.

    dict.clear(); // DLC use class with ~ that calls
    // `delete (ptr_type) x->ptr' to stop this memory leak
    array.clear();
    reservedWords.clear();
    isInitialized = FALSE;
  }
  void firstInit(CConxCanvas *pdCanvas = NULL, CConxCanvas *kdCanvas = NULL,
                 CConxCanvas *uhpCanvas = NULL)
  // Call this before your parsing, it sets up the environment so that
  // `Point new' will work.
  // After a parse, if you wish to return to the startup state, the state
  // that exists after this is called and before any parsing is done,
  // call reinit()
  {
    MMM("firstInit()");
    if (!isInitialized) {
      // DLC
      loadStartupClassInstances();
      loadStartupObjectInstances(pdCanvas, kdCanvas, uhpCanvas);
      loadReservedWords();

      // Load a Float class, a Circle class, a true object instance, a false object instance, a ...
      // Load a System instance, a Klein instance, a ...
      isInitialized = TRUE;
    }
  }
  void reinit(CConxCanvas *pdCanvas = NULL, CConxCanvas *kdCanvas = NULL,
              CConxCanvas *uhpCanvas = NULL)
  // Remove all instances that have been created since firstInit()
  // or reinit() was last called
  {
    MMM("reinit()");
    if (isInitialized) {
      clear();
    }
    firstInit(pdCanvas, kdCanvas, uhpCanvas);
  }
  int sendUnaryMessage(CClsBase **result, CClsBase *receiver,
                       const CConxString &msg)
  // Sends a unary message msg to the object instance or class instance
  // that r represents.
  // Returns -3 if r does not point to an instance or class.
  // Returns 1 if the message does not exist.
  //   (unassigned variable's pointee, e.g.)
  // Returns 0 on success.
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
  //#define DEBUG_DICT() cout << "\nDLC debug\n"; dict.debug(); cout << "end DLC\n"
#define DEBUG_DICT()

  Boole isVariableName(const CConxString &s) const { DEBUG_DICT(); return dict.hasKey(s); }
  const CClsBase *getNamedThing(const CConxString &id) const
  // This is a convenience that returns a class or instance that exists
  // and is labeled id.  If it does not exist, zero is thrown.  You
  // must not modify the returned object or its ptr field.
  // The returned object may become invalid any time you call a non-const
  // method.
  {
    return constGetNamedThing(id);
  }
  CClsBase *getModifiableNamedThing(const CConxString &id)
  // This is not a const method because this is a way to modify this object
  // indirectly, through one of its Smalltalk object/class instance minions.
  {
    return constGetNamedThing(id);
  }
  Boole pointsToClassInstance(const CConxString &id) const
  // Returns TRUE if id is a variable that points to a class instance
  // rather than an object instance.   After `c := Circle',
  // pointsToClassInstance("c") == pointsToClassInstance("Circle") == TRUE.
  // You may wish to call this before allowing `Circle := 0.0' since we
  // don't have the equivalent of GNU Smalltalk's class Metaclass.
  {
    CClsBase *p = constGetNamedThing(id);
    return (p != NULL && p->isClassInstance());
  }
  long makeAssignments(const CConxClsAssignmentList &al,
                       CClsBase *assignTo);
  Boole makeAssignment(const CConxString &id, CClsBase *v)
    throw(const char *);
  CClsFloat *newInstance(double f) throw(int, const char *)
  // Returns a new Float instance whose value is f.  You do not own the
  // returned object, so don't delete it.
  //
  // Throws an int if result is NULL.
  // Never returns NULL -- throws in that case (out of memory)
  {
    MMM("CClsFloat *newInstance(double f) throw(int)");
    CClsFloat *nf = new CClsFloat(f); // 0 users.
    if (nf == NULL) OOM();
    addNewInstance(nf);
    return nf;
  }
  CClsSmallInt *newInstance(long l) throw(int, const char *)
  {
    MMM("CClsFloat *newInstance(long l) throw(int)");
    CClsSmallInt *nf = new CClsSmallInt(l); // 0 users.
    if (nf == NULL) OOM();
    addNewInstance(nf);
    return nf;
  }
  CConxObject *newInstance(const char *s, CClsBase::ClsType t)
    throw(int, const char *)
  // Returns a new object whose initial value is s and whose type
  // is t.  Throws if OOM, throws 1 if t is an illegal type.
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
  const CClsBase *getLastResult() const
  // Returns what the variable `ans' points to, or NULL if `ans' does not
  // point to anything.
  {
    return constGetNamedThing("ans");
  }


protected:
  CConxClsVariable *newVariable(Boole set, const char *s) throw(int)
  // If set is TRUE, then the new variable will be set to `nil'.  Otherwise,
  // the variable will not point to anything.
  // Returns a new variable that you must delete.
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
  void loadStartupClassInstances()
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

    LOAD_CLASS_INSTANCE(CClsModelIdentifier);
    LOAD_CLASS_INSTANCE(CClsColor);
    LOAD_CLASS_INSTANCE(CClsCharacterArray);
    LOAD_CLASS_INSTANCE(CClsArray);
    LOAD_CLASS_INSTANCE(CClsSmallInt);
    LOAD_CLASS_INSTANCE(CClsDrawable);
    LOAD_CLASS_INSTANCE(CClsNumber);
    LOAD_CLASS_INSTANCE(CClsSystem);
    LOAD_CLASS_INSTANCE(CClsCanvas);
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
  void loadStartupObjectInstances(CConxCanvas *pdCanvas, CConxCanvas *kdCanvas,
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
  void loadReservedWords()
  {
    // we do everything inline thus far.
  }
  Boole isReservedWord(const char *w) const
  // Returns true if you must not reassign or delete the variable w.
  {
    size_t sz = reservedWords.size();
    for (size_t i = 0; i < sz; i++) {
      if (reservedWords.get(i) == w) return TRUE;
    }
    return FALSE;
  }
  void addReservedWord(const char *w)
  // Adds a reserved word, i.e. a variable name that may not be reassigned
  // or deleted.
  {
    reservedWords.append(w);
  }
  void removeUser(CClsBase *pp) throw(const char *)
  // Call this when you were using pp but are no longer.  pp itself should
  // not call this.  Throws a const char * if pp is not known.
  //
  // This is public only because making CConxClsKeywordMessage a friend seems
  // like too far to go for this functionality.
  //
  // Call this when the class instance or object instance
  // pp loses a user.
  // E.g., in `P := Point new. P := 3.3' you would call this with
  // the `Point new' object before assigning P to 3.3.
  //
  // Aids in garbage collection.
  //
    // DLC implement:
  // GC will work as follows: when a variable points to you, you're tagged.
  // When a tagged item points to you, you're tagged.  At any time, deleting
  // an item that is not tagged is fair game.  (I.e., there exist unnamed
  // variables, such as Point's x: and y: fields.)
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

  void setVariable(const CConxString &s, CConxClsVariable *r)
  // Adds or sets a variable named s.
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
  CConxClsVariable *getVariable(const CConxString &id)
  {
    if (isVariableName(id)) {
      return dict.get(id);
    }
    return NULL;
  }
  CClsBase *constGetNamedThing(const CConxString &id) const
  // Gets the class instance or object instance that the variable named id
  // points to, or NULL if id is not the name of an existing variable,
  // and puts it in result.
  {
    MMM("CClsBase *constGetNamedThing(const CConxString &id) const");
    if (isVariableName(id)) {
      assert((dict.get(id))->getPointee() != NULL);
      return (dict.get(id))->getPointee();
    } else {
      return NULL;
    }
  }
  void addNewInstance(CClsBase *p)
  {
    MMM("void addNewInstance(CClsBase *p)");

    if (p == NULL) throw "Hey, that's impossible!";
    // DLC please work! MARK1
    p->incrementUsers();
    LLL("appending " << p << " to the clsmgr array");
    array.append(p); // DLC does this copy???
    // DLC GC increment here???
  }

  /* DLC GC note that `2.2 between: 1.1 and: 3.3<Enter>^-D' causes a
     memory leak, i.e.
<<<<< CConxObject::~CConxObject()
There are now, after this destruction, 0 objects in existence.
>>>>>
does not appear

BEFORE `ans' SUPPORT, that is.
  */

private: // forbidden operations
#if 0
public:
  CConxClsManager(const CConxClsManager &o)
    : CConxObject(o)
  {
    MMM("CConxClsManager(const CConxClsManager &o)");
    uninitializedCopy(o);
  }
  CConxClsManager &operator=(const CConxClsManager &o)
  {
    MMM("CConxClsManager &operator=(const CConxClsManager &o)");
    (void) CConxObject::operator=(o);
    uninitializedCopy(o);
    return *this;
  }

  void collectGarbage()
  {
    MMM("void collectGarbage()");
    // DLC implement
  }

  void uninitializedCopy(const CConxClsManager &o)
  // Copying semantics: We don't copy the array, we create a new array 
  // with its entries' .ptr fields as copies of o's array's entries'
  // .ptr fields.
  //
  // How does the dict work?  Must we test for pointer equality, work out
  // a map, and use the pointers to the new copies???  If so, why
  // don't we use a map to the indices to begin with?
  {
    isInitialized = o.isInitialized;

    // DLC track keyword messages from this object.

    array = o.array;
    reservedWords = o.reservedWords;
    // Now change keyword messages to point to this manager object, not o. DLC

    // DLC bad!!
    abort();
    dict = o.dict;
  }
#else
  CConxClsManager(const CConxClsManager &o)
  {
    CXXFATAL("CConxClsManager(const CConxClsManager &o) can never be called because the copying semantics are not obvious and the need does not yet exist.");
  }
  CConxClsManager &operator=(const CConxClsManager &o)
  {
    CXXFATAL("CConxClsManager &operator=(const CConxClsManager &o) can never be called because the copying semantics are not obvious and the need does not yet exist.");
  }
#endif

private: // attributes
  LDictionary<CConxString, CConxClsVariable *> dict;
  // variables and class names.

  BPArray array;                   // Instances and classes.
  Boole isInitialized;            // Do we have the initial hierarchy loaded?
  CConxStringArray reservedWords; // DLC inefficient.
}; // class CConxClsManager


OOLTLT_INLINE P_STREAM_OUTPUT_SHORTCUT_DECL(CConxClsManager);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Implementation

#endif // GPLCONX_CLSMGR_CXX_H

