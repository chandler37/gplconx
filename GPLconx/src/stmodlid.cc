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
  Implementation of C++ classes in `stmodlid.hh'.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "stmodlid.hh"
#include "sterror.hh"

Answerers *CClsModelIdentifier::ansMachs = NULL;

NF_INLINE
CClsModelIdentifier::Model CClsModelIdentifier::model() const
// Will not return NIL, of course.
{
  // DLC use gperf for greater speed.
  if (mdl == NIL) {
    const CConxString &s = getValue();
    // We include dashes and spaces here but the scanner may or may not allow them.
    // 'poincare-disk' is ok but #poincare-disk is illegal.
    if (s.isPrefixOf("kleindisk", 1, FALSE)
        || s.isPrefixOf("kd", 2, FALSE)
        || s.isPrefixOf("klein-disk", 1, FALSE)
        || s.isPrefixOf("beltrami-klein", 1, FALSE)
        || s.isPrefixOf("klein_disk", 1, FALSE)
        || s.isPrefixOf("beltrami_klein", 1, FALSE)
        || s.isPrefixOf("klein disk", 1, FALSE)
        || s.isPrefixOf("beltrami klein", 1, FALSE)
        || s.isPrefixOf("beltramiklein", 1, FALSE)) {
      mdl = KD;
    } else if (s.isPrefixOf("uhp", 1, FALSE)
               || s.isPrefixOf("puhp", CConxString("pu").getLength(), FALSE)
               || s.isPrefixOf("upperhalfplane", 1, FALSE)
               || s.isPrefixOf("poincareuhp",
                               CConxString("poincareu").getLength(), FALSE)
               || s.isPrefixOf("poincare-upper-half-plane",
                               CConxString("poincare-u").getLength(), FALSE)
               || s.isPrefixOf("upper-half-plane", 1, FALSE) // DLC add 'poincare uhp
               || s.isPrefixOf("poincare_upper_half_plane",
                               CConxString("poincare_u").getLength(), FALSE)
               || s.isPrefixOf("upper_half_plane", 1, FALSE)
               || s.isPrefixOf("poincare upper half plane",
                               CConxString("poincare u").getLength(), FALSE)
               || s.isPrefixOf("upper half plane", 1, FALSE)) {
      mdl = UHP;
    } else if (s.isPrefixOf("poincaredisk",
                            CConxString("poincared").getLength(), FALSE)
               || s.isPrefixOf("poincare-disk",
                               CConxString("poincare-d").getLength(), FALSE)
               || s.isPrefixOf("poincare_disk",
                               CConxString("poincare_d").getLength(), FALSE)
               || s.isPrefixOf("poincare disk",
                               CConxString("poincare d").getLength(), FALSE)
               || s.isPrefixOf("pd", 100, FALSE)) {
      mdl = PD;
    } else {
      //      if (s.prefixEquals("klein" || s == 
      mdl = NONE; // DLC finish this.
    }
  }
  assert(mdl == NONE || mdl == PD || mdl == KD || mdl == UHP);
  return mdl;
}

NF_INLINE
CConxString CClsModelIdentifier::printString() const
{
  if (isClassInstance())
    return getClsName();
  else {
    const char *ms;
    if (mdl == NIL)
      (void) model(); // calculate it.
    assert(mdl != NIL);
    switch(mdl) {
    case NONE: ms = "no valid model"; break;
    case PD: ms = "the Poincare Disk"; break;
    case UHP: ms = "the Poincare Upper Half Plane"; break;
    default: assert(mdl == KD); ms = "the Beltrami-Klein Disk"; break;
    }
    return CConxString("Model Identifier '") + getValue()
      + "' which refers to " + ms;
  }
}

CF_INLINE
CClsModelIdentifier::CClsModelIdentifier(ConxModlType m)
{
  if (m == CONX_POINCARE_UHP) {
    mdl = UHP;
    setValue("uhp");
  } else if (m == CONX_POINCARE_DISK) {
    mdl = PD;
    setValue("pd");
  } else {
    assert(m == CONX_KLEIN_DISK);
    mdl = KD;
    setValue("kd");
  }
}

NF_INLINE ConxModlType CClsModelIdentifier::cModel() const throw(int)
// Throws if model() == NONE
{
  if (mdl == NIL)
    (void) model(); // calculate it
  if (mdl == NONE) throw(1);
  else if (mdl == PD) return CONX_POINCARE_DISK;
  else if (mdl == KD) return CONX_KLEIN_DISK;
  else return CONX_POINCARE_UHP;
}

NF_INLINE CClsModelIdentifier &
CClsModelIdentifier::operator=(const CClsModelIdentifier &o)
{
  (void) CClsCharacterArray::operator=(o);
  mdl = o.mdl;
  return *this;
}

NF_INLINE
void CClsModelIdentifier::initializeAnsweringMachines()
{
  if (ansMachs == NULL) {
    ansMachs = new Answerers();
    if (ansMachs == NULL) OOM();

    // DLC for efficiency, don't allow `Boolean true' and `Boolean false',
    // force copying `true' and `false'.
    ST_CMETHOD(ansMachs, "new:", "instance creation", CLASS, ciAnswererNew,
               "Returns a new object instance that represents the Beltrami-Klein disk, Poincare Disk, Poincare Upper Halp Plane, or an unknown model of hyperbolic geometry, depending on the String or Symbol object instance argument.  Note that `kd', `pd', and `uhp' are reserved variables that are already provided for you, and it is more efficient to use them");
    ST_CMETHOD(ansMachs, "set:", "superclass method implementation",
               OBJECT, ciAnswererSet,
               "Sets the value of the receiver to the Symbol or String argument");
  }
}

NF_INLINE
CConxString CClsModelIdentifier::printShortString() const
{
  Model m = model();
  switch (m) {
  case KD: return "kd";
  case PD: return "pd";
  case UHP: return "uhp";
  default: return "unknown";
  }
}

NF_INLINE
void CClsModelIdentifier::setValue(const CConxString &str)
{
  CClsCharacterArray::setValue(str);
  mdl = NIL;
}

NF_INLINE
CClsBase::ErrType 
CClsModelIdentifier::ciActionNew(CClsBase **result, CConxClsMessage &o) const
{
  /*
    DLC test
    $ M1 := ModelIdentifier new: #p
    >>> Model Identifier #p which refers to no valid model
    $ M2 := ModelIdentifier new: #poincare-disk
    Parser Error: <parse error, expecting `TOK_STMT_END' or `'.''>

    GPLconx: parser got error

    GPLconx: parser got error
    >>> nil
    $ M2 := ModelIdentifier new: 'poincare-disk'
    >>> Model Identifier #poincare-disk which refers to the Poincare Disk
    $ M3 := ModelIdentifier new: 'poincare upper half'
    >>> Model Identifier #poincare upper half which refers to the Poincare Upper Half Plane
  */
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
  RETURN_NEW_RESULT(result, new CClsModelIdentifier(argv[0]->getValue()));
}

NF_INLINE
CClsBase::ErrType 
CClsModelIdentifier::ciActionSet(CClsBase **result, CConxClsMessage &o)
{
  ENSURE_SINGLE_ARG_IS_OF_TYPE(CClsCharacterArray, argv, o);
  CHECK_READ_ONLYNESS(result);
  setValue(argv[0]->getValue());
  RETURN_THIS(result);
}

