/*-------------------------------------------------------------------------* 
 * GNU Prolog                                                              * 
 *                                                                         * 
 * Part  : Prolog buit-in predicates                                       * 
 * File  : list_c.c                                                        * 
 * Descr.: list library  - C part                                          *
 * Author: Daniel Diaz                                                     * 
 *                                                                         * 
 * Copyright (C) 1999-2009 Daniel Diaz                                     * 
 *                                                                         * 
 * GNU Prolog is free software; you can redistribute it and/or modify it   * 
 * under the terms of the GNU General Public License as published by the   * 
 * Free Software Foundation; either version 2, or any later version.       * 
 *                                                                         * 
 * GNU Prolog is distributed in the hope that it will be useful, but       * 
 * WITHOUT ANY WARRANTY; without even the implied warranty of              * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        * 
 * General Public License for more details.                                * 
 *                                                                         * 
 * You should have received a copy of the GNU General Public License along * 
 * with this program; if not, write to the Free Software Foundation, Inc.  * 
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.               * 
 *-------------------------------------------------------------------------*/

/* $Id$ */

#include "engine_pl.h"
#include "bips_pl.h"



/*---------------------------------*
 * Constants                       *
 *---------------------------------*/

/*---------------------------------*
 * Type Definitions                *
 *---------------------------------*/

/*---------------------------------*
 * Global Variables                *
 *---------------------------------*/

/*---------------------------------*
 * Function Prototypes             *
 *---------------------------------*/

#define APPEND_ALT  X24617070656E645F616C74
#define LENGTH_ALT  X246C656E6774685F616C74
#define MEMBER_ALT  X246D656D6265725F616C74
#define REVERSE_ALT X24726576657273655F616C74


Prolog_Prototype(APPEND_ALT, 0);
Prolog_Prototype(LENGTH_ALT, 0);
Prolog_Prototype(MEMBER_ALT, 0);
Prolog_Prototype(REVERSE_ALT, 0);


#if 1
/*-------------------------------------------------------------------------*
 * Pl_APPEND_3                                                             *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Append_3(WamWord l1_word, WamWord l2_word, WamWord l3_word)
{
  WamWord word, tag_mask;
  WamWord *adr;
  int len1, len2, len3;
  WamWord result_word;
  WamWord *next_H;


  for(;;) 
    {
      DEREF(l1_word, word, tag_mask);
      if (tag_mask != TAG_LST_MASK)
	break;
      
      adr = UnTag_LST(word);

      DEREF(l3_word, word, tag_mask);
      if (tag_mask == TAG_REF_MASK) /* as soon as L3 is a var, create the result and unify with L3 at the end only */
	{
	  result_word = Tag_LST(H);
	  do {      
	    next_H = H + 2;
	    *H++ = Car(adr);
	    *H++ = Tag_LST(next_H);

	    l1_word = Cdr(adr);

	    DEREF(l1_word, word, tag_mask);
	    adr = UnTag_LST(word);	  
	  } while(tag_mask == TAG_LST_MASK);
	  next_H = H - 1;
	  *next_H = Make_Self_Ref(next_H);

	  Pl_Unify(result_word, l3_word);

	  l3_word = *next_H;
	  break;
	}
      /* here L3 is not a var */

      if (!Pl_Get_List(l3_word) || !Pl_Unify_Value(Car(adr)))
	return FALSE;

      l3_word = Pl_Unify_Variable();
      l1_word = Cdr(adr);
    }

  if (word == NIL_WORD)
    return Pl_Unify(l2_word, l3_word);

  if (tag_mask != TAG_REF_MASK)
    return FALSE;



  /* L1 is a var, let's see L2 and L3 */

  if ((len2 = Pl_List_Length(l2_word)) >= 0 && 
      (len3 = Pl_List_Length(l3_word)) >= 0)
    {		/* deterministic: L1 is the prefix of L3 with len = len3 - len2 */
      if ((len1 = len3 - len2) < 0)
	return FALSE;

      while(len1-- > 0)
	{
	  DEREF(l3_word, word, tag_mask);
	  adr = UnTag_LST(word);
	  Pl_Get_List(l1_word);

	  Pl_Unify_Value(Car(adr));
	  l1_word = Pl_Unify_Variable();
	  l3_word = Cdr(adr);
	}

      return Pl_Get_Nil(l1_word); /* always succeeds */
    }

  /* L1 is a var, L2 / L3 are not 2 proper lists, check L3 */

  DEREF(l3_word, word, tag_mask);
  if (tag_mask == TAG_REF_MASK || tag_mask == TAG_LST_MASK) /* nondet case */
    {
      A(0) = l1_word;
      A(1) = l2_word;
      A(2) = l3_word;
      Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(APPEND_ALT, 0), 3);
    }

  Pl_Get_Nil(l1_word);
  return Pl_Unify(l2_word, l3_word);
}

#else  /* less efficient version */

/*-------------------------------------------------------------------------*
 * Pl_APPEND_3                                                             *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Append_3(WamWord l1_word, WamWord l2_word, WamWord l3_word)
{
  WamWord word, tag_mask;
  WamWord *adr;
  int len1, len2, len3;

  for(;;) 
    {
      DEREF(l1_word, word, tag_mask);
      if (tag_mask != TAG_LST_MASK)
	break;
      
      adr = UnTag_LST(word);
      if (!Pl_Get_List(l3_word) || !Pl_Unify_Value(Car(adr)))
	return FALSE;

      l3_word = Pl_Unify_Variable();
      l1_word = Cdr(adr);
    }

  if (word == NIL_WORD)
    return Pl_Unify(l2_word, l3_word);

  if (tag_mask != TAG_REF_MASK)
    return FALSE;


  /* L1 is a var, let's see L2 and L3 */

  if ((len2 = Pl_List_Length(l2_word)) >= 0 && 
      (len3 = Pl_List_Length(l3_word)) >= 0)
    {		/* deterministic: L1 is the prefix of L3 with len = len3 - len2 */
      if ((len1 = len3 - len2) < 0)
	return FALSE;

      while(len1-- > 0)
	{
	  DEREF(l3_word, word, tag_mask);
	  adr = UnTag_LST(word);
	  Pl_Get_List(l1_word);

	  Pl_Unify_Value(Car(adr));
	  l1_word = Pl_Unify_Variable();
	  l3_word = Cdr(adr);
	}

      return Pl_Get_Nil(l1_word); /* always succeeds */
    }

  /* L1 is a var, L2 / L3 are not 2 proper lists, check L3 */

  DEREF(l3_word, word, tag_mask);
  if (tag_mask == TAG_REF_MASK || tag_mask == TAG_LST_MASK) /* nondet case */
    {
      A(0) = l1_word;
      A(1) = l2_word;
      A(2) = l3_word;
      Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(APPEND_ALT, 0), 3);
    }

  Pl_Get_Nil(l1_word);
  return Pl_Unify(l2_word, l3_word);
}
#endif



/*-------------------------------------------------------------------------*
 * PL_APPEND_ALT_0                                                         *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Append_Alt_0(void)
{
  WamWord x_word;
  WamWord word, tag_mask;

  /* here L1 is a var - L3 is a var or [_|_] */

  Pl_Delete_Choice_Point(3);

  Pl_Get_List(A(2));
  x_word = Pl_Unify_Variable();
  A(2) = Pl_Unify_Variable();

  Pl_Get_List(A(0));
  Pl_Unify_Value(x_word);	/* cannot fail since L1 is a var */
  A(0) = Pl_Unify_Variable();


  DEREF(A(2), word, tag_mask);
  if (tag_mask == TAG_REF_MASK || tag_mask == TAG_LST_MASK) /* nondet case */
    Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(APPEND_ALT, 0), 3);

  Pl_Get_Nil(A(0));
  return Pl_Unify(A(1), A(2));
}




/*-------------------------------------------------------------------------*
 * Pl_MEMBER_2                                                             *
 *                                                                         *
 * To optimize, we don't pass arguments, we know they are in A(0) and A(1) *
 *-------------------------------------------------------------------------*/
Bool
Pl_Member_2(void)
{
  WamWord word, tag_mask;
  WamWord head_word;
  Bool ok;

  if (!Pl_Get_List(A(1)))
    return FALSE;

  head_word = Pl_Unify_Variable();
  A(1) = Pl_Unify_Variable();
  
  Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(MEMBER_ALT, 0), 2);

  for(;;)
    {
      ok = Pl_Unify(A(0), head_word);

      DEREF(A(1), word, tag_mask);

      if (tag_mask != TAG_REF_MASK && tag_mask != TAG_LST_MASK)
	{
	  Assign_B(BB(B));  /* cut (if failure faster then Pl_Delete_Choice_Point() */  
	  return ok;
	}

      if (ok)
	{
	  AB(B, 1) = word;
	  return ok;
	}

      Pl_Update_Choice_Point((CodePtr) Prolog_Predicate(MEMBER_ALT, 0), 0);

      Pl_Get_List(A(1));	/* always succeeds */
      head_word = Pl_Unify_Variable();
      A(1) = Pl_Unify_Variable();
    }
}




/*-------------------------------------------------------------------------*
 * PL_MEMBER_ALT_0                                                         *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Member_Alt_0(void)
{
  Pl_Delete_Choice_Point(2);
  return Pl_Member_2();
}




/*-------------------------------------------------------------------------*
 * Pl_MEMBERCHK_2                                                          *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Memberchk_2(WamWord elem_word, WamWord list_word)
{
  Bool ret;

  Pl_Defeasible_Open();

  for(;;)
    {
      if (!Pl_Get_List(list_word))
	{
	  ret = FALSE;
	  break;
	}

      if (Pl_Unify_Value(elem_word)) /* unification will trail */
	{
	  Pl_Unify_Void(1);  /* important if in write mode (list_word is a var) */
	  ret = TRUE;
	  break;
	}

      Pl_Defeasible_Undo();	/* undo bindings after unif failure (e.g.  f(X,b,c) = f(a,Y,d)) */
      list_word = Pl_Unify_Variable();
    }

  Pl_Defeasible_Close(ret);
  return ret;
}




/*-------------------------------------------------------------------------*
 * Pl_LENGTH_2                                                             *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Length_2(WamWord list_word, WamWord n_word)
{
  WamWord word, tag_mask;
  WamWord *adr;
  long n, len = 0;

  DEREF(n_word, word, tag_mask);
  n_word = word;
  if (tag_mask == TAG_INT_MASK)
    {
      n = UnTag_INT(word);
      if (n < 0)
	return FALSE;
    }
  else
    {
      if (tag_mask != TAG_REF_MASK)
	Pl_Err_Type(pl_type_integer, word);
      n = -1;
    }

  for(;;)
    {
      DEREF(list_word, word, tag_mask);
      if (word == NIL_WORD)
	return (n == len) || Pl_Get_Integer(len, n_word);


      if (tag_mask == TAG_REF_MASK)
	{
	  if (n < 0)		/* non-deterministic case */
	    break;

	  if (n == len)
	      return Pl_Get_Nil(word); /* return is TRUE */

	  Pl_Get_List(word);
	  Pl_Unify_Void(1);
	  list_word = Pl_Unify_Variable();
	  len++;
	  continue;
	}

      if (tag_mask != TAG_LST_MASK)
	return FALSE;

      len++;
      if ((unsigned long) n < (unsigned long) len)
	return FALSE;

      adr = UnTag_LST(word);
      list_word = Cdr(adr);
    }

		/* non-deterministic case */
  A(0) = list_word;
  A(1) = n_word;
  A(2) = Tag_INT(len);
  A(3) = Tag_INT(0);
  Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(LENGTH_ALT, 0), 4);

  Pl_Get_Nil(list_word);
  return Pl_Get_Integer(len, n_word); /* always TRUE */
}




/*-------------------------------------------------------------------------*
 * PL_LENGTH_ALT_0                                                         *
 *                                                                         *
 *-------------------------------------------------------------------------*/
void
Pl_Length_Alt_0(void)
{
  WamWord list_word, n_word;
  int len, i;

  Pl_Update_Choice_Point((CodePtr) Prolog_Predicate(LENGTH_ALT, 0), 0);
  list_word = AB(B, 0);
  n_word = AB(B, 1);
  len = UnTag_INT(AB(B, 2));
  i =  UnTag_INT(AB(B, 3)) + 1;
  
  AB(B, 3) = Tag_INT(i);
  
  Pl_Get_Integer(i + len, n_word);

  do
    {
      Pl_Get_List(list_word);
      Pl_Unify_Void(1);
      list_word = Pl_Unify_Variable();
      
    }
  while(--i);
  Pl_Get_Nil(list_word);
}




/*-------------------------------------------------------------------------*
 * PL_NTH0_3                                                               *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Nth0_3(WamWord n_word, WamWord list_word, WamWord res_word, int base)
{
  WamWord word, tag_mask;
  WamWord *adr;
  long n = Pl_Rd_Integer(n_word) - base;

  if (n < 0)
    return FALSE;

  for(;;)
    {
      DEREF(list_word, word, tag_mask);
      if (tag_mask != TAG_LST_MASK)
	return FALSE;

      adr = UnTag_LST(word);

      if (n == 0)
	return Pl_Unify(Car(adr), res_word);

      n--;
      list_word = Cdr(adr);
    }
}



/*-------------------------------------------------------------------------*
 * PL_REVERSE_2                                                            *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Reverse_2(WamWord l1_word, WamWord l2_word)
{
  WamWord word, tag_mask;
  WamWord *adr;
  int len1 = 0, len2;
  WamWord x_word;
  WamWord result_word = NIL_WORD;

  for(;;) 
    {
      DEREF(l1_word, word, tag_mask);
      if (tag_mask != TAG_LST_MASK)
	break;
      
      adr = UnTag_LST(word);
      word = Tag_LST(H);
      *H++ = Car(adr);
      *H++ = result_word;
      result_word = word;
      len1++;
      l1_word = Cdr(adr);
    }

  if (word == NIL_WORD)
    return Pl_Unify(result_word, l2_word);

  if (tag_mask != TAG_REF_MASK)
    return FALSE;

  /* L1 ends with a variable */

  len2 = Pl_List_Length(l2_word);
  if (len2 >= 0)
    {
      if (len2 < len1)
	return FALSE;
      while(len1 < len2)	/* L1 and Result must have len2 elements */
	{
	  Pl_Get_List(l1_word);
	  x_word = Pl_Unify_Variable();
	  l1_word = Pl_Unify_Variable();

	  word = Tag_LST(H);
	  *H++ = x_word;
	  *H++ = result_word;
	  result_word = word;
	  
	  len1++;
	}
    }
  else
    {
      A(0) = l1_word;   /* non-deterministic case: both end with a variable - infinite nb of sols */
      A(1) = l2_word;
      A(2) = result_word;
      Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(REVERSE_ALT, 0), 3);
    }

  Pl_Get_Nil(l1_word);
  return Pl_Unify(result_word, l2_word);
}



/*-------------------------------------------------------------------------*
 * PL_REVERSE_ALT_0                                                        *
 *                                                                         *
 *-------------------------------------------------------------------------*/
Bool
Pl_Reverse_Alt_0(void)
{
  WamWord x_word, result_word;

  Pl_Delete_Choice_Point(3);

  /* create a new sol: add X at the of L1 and X at the beginning of Result */
  /* (which is unified with L2) */

  Pl_Get_List(A(0));
  x_word = Pl_Unify_Variable();
  A(0) = Pl_Unify_Variable();

  result_word = Tag_LST(H);
  *H++ = x_word;
  *H++ = A(2);
  A(2) = result_word;

  Pl_Create_Choice_Point((CodePtr) Prolog_Predicate(REVERSE_ALT, 0), 3);

  Pl_Get_Nil(A(0));
  return Pl_Unify(result_word, A(1));
}