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
  Implementation of `cparse.hh'
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>

#include "sth_all.hh"
#include "CString.hh"
#include "clsmgr.hh"
#include "cparse.hh"
#include "sterror.hh"

#define PARSING_PROMPT "$ " << flush

static Boole interactive = TRUE;

/* DLC Force tparser to use its own: */
static CConxClsManager *mgr = NULL;

void conxP_change_manager(CConxClsManager *newMgr)
// Call conxP_final_cleanup() before calling this if you have used the
// default manager.
{
  mgr = newMgr;
}

void conxP_set_interactive(Boole isInteractive)
{
  interactive = isInteractive;
  llextern_set_interactive(interactive);
}

void conxP_initialize(void)
// Call this before your parsing, it sets up the environment so that
// `Point new' will work.
// After a parse, if you wish to return to the startup state, the state
// that exists after this is called and before any parsing is done,
// call conxP_reinitialize
{
  LLL("void conxP_initialize(void)");
  if (mgr == NULL) throw "bad init";
  mgr->firstInit();
  if (interactive) cout << PARSING_PROMPT;
}

void conxP_reinitialize(void)
// Call this to reset the environment to the way it was right after
// conxP_initialize was called.  If conxP_initialize has not yet been
// called, this calls it.
{
  LLL("void conxP_reinitialize(void)");
  // DLC huh? cout << "void conxP_reinitialize(void)""\n";
  if (mgr == NULL) mgr = new CConxClsManager;
  if (mgr == NULL) throw "out of memory k334";
  mgr->reinit();
  if (interactive) cout << PARSING_PROMPT;
}

void conxP_get_variable(SimpleClsRef *r, const char *id)
// If the variable does not already exist, we initialize to a `nil' instance.
{
  assert(r != NULL); assert(id != NULL);
  assert(mgr != NULL);
  CClsBase *pointee = mgr->getModifiableNamedThing(id);
  if (pointee == NULL) {
    conxP_define_new_variable(r, id);
  } else {
    r->ptr = pointee;
    r->ptr_type = pointee->getType();
  }
}

void conxP_send_unary_message(SimpleClsRef *result, SimpleClsRef receiver,
                              const char *unaryMessage)
// Sets result to the result of receiver's application of the unary message
// unaryMessage
{
try {
  LLL("int conxP_send_unary_message(SimpleClsRef *result, SimpleClsRef receiver, const char *unaryMessage)");
  assert(unaryMessage != NULL);
  assert(result != NULL);
  CClsBase *trueResult = NULL;
  CClsBase *trueRecv = NULL;
  REF2OBJ(trueRecv, &receiver);
  assert(trueRecv != NULL);
  (void) mgr->sendUnaryMessage(&trueResult, trueRecv, unaryMessage);
  assert(trueResult != NULL);
  result->ptr = trueResult;
  result->ptr_type = trueResult->getType();
} catch (const char *errs) {
  cerr << "\n008GPLconx tparser had a const char * exception thrown:\n`" << errs << "'\nAborting.\n";
  exit(3);
} catch (int k1) {
  cerr << "\n008GPLconx tparser had an int exception thrown:\n`" << k1 << "'\nAborting.\n";
  exit(3);
} catch (...) {
  cerr << "\n008GPLconx tparser had some exception thrown.\n\nAborting.\n";
  exit(3);
}
}

void conxP_new_Float_instance(SimpleClsRef *result, double f)
{
  // DLC try..catch
  LLL("void conxP_new_Float_instance(SimpleClsRef *result, double f)");
  assert(result != NULL);
  result->ptr = mgr->newInstance(f);
  result->ptr_type = CClsBase::CLS_FLOAT;
}

void conxP_new_SmallInt_instance(SimpleClsRef *result, long l)
{
  // DLC try..catch
  LLL("void conxP_new_SmallInt_instance(SimpleClsRef *result, long l)");
  assert(result != NULL);
  result->ptr = mgr->newInstance(l);
  result->ptr_type = CClsBase::CLS_SMALLINT;
}

void conxP_new_Symbol_instance(SimpleClsRef *result, const char *s)
{
  // DLC try..catch
  LLL("void conxP_new_Symbol_instance(SimpleClsRef *result, const char *s)");
  assert(result != NULL);
  result->ptr = mgr->newInstance(s, CClsBase::CLS_SYMBOL);
  result->ptr_type = CClsBase::CLS_SYMBOL;
}

void conxP_new_String_instance(SimpleClsRef *result, const char *s)
{
  LLL("void conxP_new_String_instance(SimpleClsRef *result, const char *s)");
  assert(result != NULL);
  result->ptr = mgr->newInstance(s, CClsBase::CLS_STRING);
  result->ptr_type = CClsBase::CLS_STRING;
}

void conxP_set_to_empty(SimpleClsRef *r)
// Make *r refer to nothing.
{
  // DLC delete  LLL("void conxP_set_to_empty(SimpleClsRef *r)");
  assert(r != NULL);
  r->ptr = NULL; r->ptr_type = CClsBase::CLS_INVALID;
}

int conxP_is_empty(SimpleClsRef r)
// Returns non-zero if *r does not represent any class or instance.
{
  // DLC delete  LLL("int conxP_is_empty(SimpleClsRef r)");
  if (r.ptr == NULL) {
    assert(r.ptr_type == CClsBase::CLS_INVALID);
    return 1;
  } else {
    assert(r.ptr_type != CClsBase::CLS_INVALID);
  }
  return 0;
}

void conxP_set_to_new_err(SimpleClsRef *resultPtr, const char *errString)
{
  assert(mgr != NULL); assert(resultPtr != NULL); assert(errString != NULL);
  resultPtr->ptr = mgr->newInstance(errString, CClsBase::CLS_ERROR);
  resultPtr->ptr_type = CClsBase::CLS_ERROR;
}

void conxP_define_new_variable(SimpleClsRef *result, const char *varName)
// Defines varName so that it is a variable that points initially to
// something arbitrary.
//
// varName must not name any class or instance.
// varName must be a legal variable identifier.
// result must not be NULL.
{
  LLL("void conxP_define_new_variable(SimpleClsRef *result, const char *varName)");
  // DLC
  assert(mgr != NULL); assert(result != NULL); assert(varName != NULL);
  result->ptr = mgr->newInstance(varName, CClsBase::CLS_VARIABLE);
  result->ptr_type = CClsBase::CLS_VARIABLE;
}

void conxP_print_line_of_output(SimpleClsRef *result)
{
  LLL("void conxP_print_line_of_output(SimpleClsRef *result)");
  if (!interactive) return;
  CClsBase *clsobj;
  if (result->ptr == NULL) {
    //    cout << ">>> GPLconx parse error" << endl << PARSING_PROMPT; // DLC remove "GPLconx " this line is commented because parse errors are objects,
    // only blank lines are not.
  } else {
    REF2OBJ(clsobj, result);
    if (clsobj == NULL) {
      if (result->ptr_type == CClsBase::CLS_VARIABLE) {
        cout << "variable's value is nil" << endl;
      } else {
        cout << "DLC no object to print" << endl; // DLC can this happen?
      }
    } else {
#ifdef DLC
      /*      cout << "About to crash DLC CLS_FLOAT="
           << CClsBase::clsTypeToString(CClsBase::CLS_FLOAT)
           << endl << flush; */
      cout << "DLC printing line -- clsobj is " << clsobj << "\n";
      cout << "clsobj's type is "
           << CClsBase::clsTypeToString(clsobj->getType())
           << endl << flush;
      cout << "clsobj is " << *clsobj << endl << flush;
      const CConxString &ps = clsobj->printString();
      cout << "DLC>>> " << flush;
      cout << ps << endl;
#else
      cout << ">>> " << clsobj->printString() << endl;
#endif
    }
  }
  cout << PARSING_PROMPT;
}

void conxP_new_assignment_list(AssnList *a)
{
  *a = new CConxClsAssignmentList;
  if (a == NULL) throw "out of memory k335";
  //  cout << "DLC new CConxClsAssignmentList is @" << (*a) << endl;
}

void conxP_add_to_assignment_list(AssnList *a, const char *id)
{
  ((CConxClsAssignmentList *)*a)->appendVar(id);
}

long conxP_perform_and_delete_assignments(AssnList *a, SimpleClsRef assignTo)
{
try {
  CConxClsAssignmentList *al = (CConxClsAssignmentList *)*a;
  assert(mgr != NULL);
  assert(al != NULL);
  CClsBase *assnObj;
  REF2OBJ(assnObj, &assignTo);
  long code = mgr->makeAssignments(*al, assnObj);
  delete al;
  al = NULL;
  return code;
} catch (const char *errs) {
  cerr << "\n002GPLconx tparser had a const char * exception thrown:\n`" << errs << "'\nAborting.\n";
  exit(3);
} catch (...) {
  cerr << "\n002GPLconx tparser had some exception thrown.\n\nAborting.\n";
  exit(3);
}
}

void conxP_send_and_delete_keyword_message(SimpleClsRef *result,
                                           SimpleClsRef receiver,
                                           KeywordMessage *km)
// Returns zero on success.
{
try {
  assert(km != NULL); assert(result != NULL);
  CClsBase *bp, *resultPtr = NULL;
  assert(receiver.ptr != NULL); // blank variables are set to nil.
  REF2OBJ(bp, &receiver);
  assert(bp != NULL);
  CConxClsMessage tmpM(*(*(CConxClsKeywordMessage **)km)); // DLC why a copy?
  (void) bp->sendMessage(&resultPtr, tmpM);
  assert(resultPtr != NULL);
  result->ptr = resultPtr;
  result->ptr_type = resultPtr->getType();

  // DLC GC should delete this because we could have had a parse error!
  delete (*(CConxClsKeywordMessage **)km);
  *km = NULL;
} catch (const char *errs) {
  cerr << "\n001GPLconx tparser had a const char * exception thrown:\n`" << errs << "'\nAborting.\n";
  exit(3);
} catch (...) {
  cerr << "\n001GPLconx tparser had some exception thrown.\n\nAborting.\n";
  exit(3);
}
}

void conxP_new_keyword_message(KeywordMessage *km)
{
  assert(km != NULL);
  *km = new CConxClsKeywordMessage();
  if (*km == NULL) throw "out of memory k336";
  // DLC
}

void conxP_add_to_keyword_message(KeywordMessage *km, const char *keyword,
                                  SimpleClsRef m)
{
  // DLC
  CClsBase *bp;
  REF2OBJ(bp, &m); // DLC check memory alloc
  assert(bp != NULL);
  assert(mgr != NULL);
  (*(CConxClsKeywordMessage **)km)->appendKeyedArg( \
                                           new CConxClsKeyedArg(keyword, bp));
}

static CConxStringArray *pyyerror = NULL;
static CConxString cat;
void conxP_yyerror(const char *s)
// Uses pyyerror.
{
  assert(mgr != NULL);
  mgr->reportParsingError(s);
  // kill the last parse error.
  if (pyyerror != NULL) {
    delete pyyerror;
    pyyerror = NULL;
  }

  // begin a new parse error.
  pyyerror = new CConxStringArray();

  // add this string to the parse error.
  pyyerror->append(s);

// DLC delete.  (void) printf("Parser Error: <%s>\n", s);
}

const char *conxP_get_accumulated_pyyerror(const char *breaker)
// Uses pyyerror.
// Returns the concatenation of all the calls to yyerror since you
// last called this function.  The string breaker is inserted between each
// error.  If breaker is NULL, then it is as if breaker is "".
{
  if (pyyerror == NULL)
    return NULL;
  cat.setString(NULL);
  size_t sz = pyyerror->size();
  for (size_t i = 0; i + 1 < sz; i++) {
    cat += pyyerror->get(i);
    if (breaker != NULL) cat.append(breaker);
  }
  assert(sz > 0);
  cat += pyyerror->get(sz - 1);
  return cat.getString();
}

void conxP_handle_yyerror(SimpleClsRef *result, const char *first,
                          const char *breaker)
{
  assert(first != NULL);
  assert(result != NULL);
  CConxString es = first;
  es += conxP_get_accumulated_pyyerror(breaker);
  conxP_set_to_new_err(result, es.getString());
}

const char *conxP_get_specific_parse_error(SimpleClsRef r)
/* This will not be valid if r is changed or destroyed. */
{
  assert(conxP_is_parse_error(r));
  CClsBase *t = NULL;
  REF2OBJ(t, &r);
  assert(t != NULL);
  return ((CClsError *)t)->getValue().getString();
}

int conxP_is_parse_error(SimpleClsRef r)
/* Returns non-zero if r points to a CClsParseError object instance.  If you
   don't take note of such a result and pass it on to the user, then
   `System unrecogNizedUNARYmessage . 2.2' will just print as `>>> 2.2' with
   the user unaware that the first statement went bomb-o.
*/
{
  if (conxP_is_empty(r)) return 0;

  CClsBase *trueRecv = NULL;
  REF2OBJ(trueRecv, &r);
  return (!trueRecv->isClassInstance()
          && trueRecv->isType(CClsBase::CLS_ERROR));
}
