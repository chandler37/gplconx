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

#include "stobject.hh"
#include "CString.hh"
#include "Starray.hh"
#include "CArray.hh"

#include "stcanvas.hh"

class CClsFloat;
class CClsSymbol;
class CClsSmallInt;

class CConxClsVariable;
class CConxClsAssignmentList;

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
  CConxClsManager();
  ~CConxClsManager();

  int operator==(const CConxClsManager &o) const { return 0; } // DLC
  int operator!=(const CConxClsManager &o) const { return !operator==(o); }

  void reportParsingError(const char *s);
  size_t getNumParsingErrors() const { return numParseErrors; }
  void zeroParsingErrors();

  void clear();

  // Call this before your parsing, it sets up the environment so that
  // `Point new' will work.
  // After a parse, if you wish to return to the startup state, the state
  // that exists after this is called and before any parsing is done,
  // call reinit()
  void firstInit(CConxCanvas *pdCanvas = NULL, CConxCanvas *kdCanvas = NULL,
                 CConxCanvas *uhpCanvas = NULL);

  // Remove all instances that have been created since firstInit()
  // or reinit() was last called
  void reinit(CConxCanvas *pdCanvas = NULL, CConxCanvas *kdCanvas = NULL,
              CConxCanvas *uhpCanvas = NULL);

  // Sends a unary message msg to the object instance or class instance
  // that r represents.
  // Returns -3 if r does not point to an instance or class.
  // Returns 1 if the message does not exist.
  //   (unassigned variable's pointee, e.g.)
  // Returns 0 on success.
  int sendUnaryMessage(CClsBase **result, CClsBase *receiver,
                       const CConxString &msg);

  //#define DEBUG_DICT() cout << "\nDLC debug\n"; dict.debug(); cout << "end DLC\n"
#define DEBUG_DICT()

  typedef LDictionary<CConxString, CConxClsVariable *> VarDict;
  typedef LDictContainer<CConxString, CConxClsVariable *> VarDictElem;
  const VarDict &getVarDict() const { return dict; }
  VarDict &getVarDict() { return dict; } // DLC do not modify this.
  // Unfortunately, there is no `forEach() const'

  Boole isVariableName(const CConxString &s) const;

  // This is a convenience that returns a class or instance that exists
  // and is labeled id.  If it does not exist, zero is thrown.  You
  // must not modify the returned object or its ptr field.
  // The returned object may become invalid any time you call a non-const
  // method.
  const CClsBase *getNamedThing(const CConxString &id) const;

  // This is not a const method because this is a way to modify this object
  // indirectly, through one of its Smalltalk object/class instance minions.
  CClsBase *getModifiableNamedThing(const CConxString &id);

  // Returns TRUE if id is a variable that points to a class instance
  // rather than an object instance.   After `c := Circle',
  // pointsToClassInstance("c") == pointsToClassInstance("Circle") == TRUE.
  // You may wish to call this before allowing `Circle := 0.0' since we
  // don't have the equivalent of GNU Smalltalk's class Metaclass.
  Boole pointsToClassInstance(const CConxString &id) const;

  long makeAssignments(const CConxClsAssignmentList &al, CClsBase *assignTo);

  // Call to implement `F := G' with id == "F" and
  // v == the thing G points to, e.g.
  // If v is NULL, a const char * is thrown.
  // Returns TRUE on failure, if you try, e.g.,
  // to assign `Circle := 3.3', a reserved word.
  // Returns FALSE on success, e.g. `C := Circle' or `C := 3.3'
  // DLC we should allow reassignment like `C := Circle. C := 3.3'
  Boole makeAssignment(const CConxString &id, CClsBase *v) throw(const char *);

  // Returns a new Float instance whose value is f.  You do not own the
  // returned object, so don't delete it.
  //
  // Throws an int if result is NULL.
  // Never returns NULL -- throws in that case (out of memory)
  CClsFloat *newInstance(double f) throw(int, const char *);

  CClsSmallInt *newInstance(long l) throw(int, const char *);

  // Returns a new object whose initial value is s and whose type
  // is t.  Throws if OOM, throws 1 if t is an illegal type.
  CConxObject *newInstance(const char *s,
                           CClsBase::ClsType t) throw(int, const char *);

  // Returns what the variable `ans' points to, or NULL if `ans' does not
  // point to anything.
  const CClsBase *getLastResult() const;


protected:
  // If set is TRUE, then the new variable will be set to `nil'.  Otherwise,
  // the variable will not point to anything.
  // Returns a new variable that you must delete.
  CConxClsVariable *newVariable(Boole set, const char *s) throw(int);

  void loadStartupClassInstances();
  void loadStartupObjectInstances(CConxCanvas *pdCanvas,
                                  CConxCanvas *kdCanvas,
                                  CConxCanvas *uhpCanvas);
  void loadReservedWords();

  // Returns true if you must not reassign or delete the variable w.
  Boole isReservedWord(const char *w) const;

  // Adds a reserved word, i.e. a variable name that may not be reassigned
  // or deleted.
  void addReservedWord(const char *w);

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
  void removeUser(CClsBase *pp) throw(const char *);

  // Adds or sets a variable named s.
  void setVariable(const CConxString &s, CConxClsVariable *r);

  CConxClsVariable *getVariable(const CConxString &id);

  // Gets the class instance or object instance that the variable named id
  // points to, or NULL if id is not the name of an existing variable,
  // and puts it in result.
  CClsBase *constGetNamedThing(const CConxString &id) const;

  void addNewInstance(CClsBase *p);

  /* DLC GC note that `2.2 between: 1.1 and: 3.3<Enter>^-D' causes a
     memory leak, i.e.
<<<<< CConxObject::~CConxObject()
There are now, after this destruction, 0 objects in existence.
>>>>>
does not appear

BEFORE `ans' SUPPORT, that is.
  */

private: // forbidden operations
#ifdef DLC
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
private:
  void uninitializedCopy(const CConxClsManager &o)
  // Copying semantics: We don't copy the array, we create a new array 
  // with its entries' .ptr fields as copies of o's array's entries'
  // .ptr fields.
  //
  // How does the dict work?  Must we test for pointer equality, work out
  // a map, and use the pointers to the new copies???  If so, why
  // don't we use a map to the indices to begin with?
  {
    numParseErrors = o.numParseErrors;
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

private:
  void init();

private: // attributes
  size_t numParseErrors;
  VarDict dict;
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

// DLC
// l2 := Line A: Point new B: Point new isSegment: true. l2 isSegment set: false
// should not cause an error.  Be smart and check in ciActionABIsSegment()
// to see if the args are read-only.  If they are, then optimize.  Make
// makeReadOnly an Object method that is unreversible.
// Alternatively, if the args are read-only, clone them.  Then the whole
// line could be read-only, but a non-read-only line will have editable
// members.
