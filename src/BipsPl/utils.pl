/*-------------------------------------------------------------------------*
 * GNU Prolog                                                              *
 *                                                                         *
 * Part  : Prolog buit-in predicates                                       *
 * File  : utils.pl                                                        *
 * Descr.: utilities                                                       *
 * Author: Daniel Diaz                                                     *
 *                                                                         *
 * Copyright (C) 1999-2012 Daniel Diaz                                     *
 *                                                                         *
 * This file is part of GNU Prolog                                         *
 *                                                                         *
 * GNU Prolog is free software: you can redistribute it and/or             *
 * modify it under the terms of either:                                    *
 *                                                                         *
 *   - the GNU Lesser General Public License as published by the Free      *
 *     Software Foundation; either version 3 of the License, or (at your   *
 *     option) any later version.                                          *
 *                                                                         *
 * or                                                                      *
 *                                                                         *
 *   - the GNU General Public License as published by the Free             *
 *     Software Foundation; either version 2 of the License, or (at your   *
 *     option) any later version.                                          *
 *                                                                         *
 * or both in parallel, as here.                                           *
 *                                                                         *
 * GNU Prolog is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received copies of the GNU General Public License and   *
 * the GNU Lesser General Public License along with this program.  If      *
 * not, see http://www.gnu.org/licenses/.                                  *
 *-------------------------------------------------------------------------*/

/* $Id$ */

:-	built_in.


'$term_to_goal'(P, CallInfo, CallerModule, QualifModule, P1) :-
	g_assign('$term_to_goal_info', CallInfo),
	g_assign('$term_to_goal_caller_module', CallerModule),
	'$term_to_goal1'(P, QualifModule, P1).

'$term_to_goal1'(P, _, _, _, _) :-
	'$pl_err_type'(callable, P).


'$term_to_goal1'(P, _QualifModule, P1) :-
	var(P), !,
	g_read('$term_to_goal_call_info', CallInfo),
	g_read('$term_to_goal_caller_module', CallerModule),
	(   CallInfo = none ->
	    P1 = call(P)   % TODO: pb with caller module ??? check it
	;
				% call is not transparent to module qualif (pass CallerModule)
	    P1 = '$call_internal'(P, CallerModule, CallerModule, CallInfo)
	).

				% TODO: what about '$meta_term'/1 ???
'$term_to_goal1'(QualifModule:P, _QualifModule, P1) :-
	!,
	'$term_to_goal1'(P, QualifModule, P1).

'$term_to_goal1'((P -> Q), QualifModule, (P1 -> Q1)) :-
	!,
	'$term_to_goal1'(P, QualifModule, P1),
	'$term_to_goal1'(Q, QualifModule, Q1).

'$term_to_goal1'((P, Q), QualifModule, (P1, Q1)) :-
	!,
	'$term_to_goal1'(P, QualifModule, P1),
	'$term_to_goal1'(Q, QualifModule, Q1).

'$term_to_goal1'((P ; Q), QualifModule, (P1 ; Q1)) :-
	!,
	'$term_to_goal1'(P, QualifModule, P1),
	'$term_to_goal1'(Q, QualifModule, Q1).
	
'$term_to_goal1'(P, _QualifModule, P) :-
	P = call(_), !.

'$term_to_goal1'(P, _QualifModule, P) :-
	P = catch(_, _, _), !.

'$term_to_goal1'(P, QualifModule, P1) :-
	callable(P),
	(   g_read('$term_to_goal_caller_module', QualifModule) -> %TODO check QualifModule vs CallerModule, g_read vs received argument
	    P1 = P  	% same module as caller, qualification is useless
	;
	    QualifModule = none -> % check none (see assert.pl)
	    P1 = P
	;
	    P1 = QualifModule:P
	).


'$check_list'(List) :-
	list(List), !.

'$check_list'(List) :-
	list_or_partial_list(List), !,
	'$pl_err_instantiation'.

'$check_list'(List) :-
	'$pl_err_type'(list, List).




'$check_list_or_partial_list'(List) :-
	list_or_partial_list(List), !.

'$check_list_or_partial_list'(List) :-
	'$pl_err_type'(list, List).




'$check_atom_or_atom_list'(List) :-
	atom(List), !.

'$check_atom_or_atom_list'(List) :-
	'$check_atom_or_atom_list1'(List), !.


'$check_atom_or_atom_list1'(List) :-
	var(List),
	'$pl_err_instantiation'.

'$check_atom_or_atom_list1'([]).

'$check_atom_or_atom_list1'([X|List]) :-
	'$check_atom_or_atom_list2'(X),
	'$check_atom_or_atom_list1'(List).

'$check_atom_or_atom_list1'(List) :-
	'$pl_err_type'(list, List).


'$check_atom_or_atom_list2'(X) :-
	atom(X), !.

'$check_atom_or_atom_list2'(X) :-
	var(X), !,
	'$pl_err_instantiation'.

'$check_atom_or_atom_list2'(X) :-
	'$pl_err_type'(atom, X).




'$get_head_and_body'((H :- B), H, B) :-
	!,
	'$check_head'(H).

'$get_head_and_body'(H, H, true) :-
	'$check_head'(H).




'$check_head'(H) :-
	var(H), !,
	'$pl_err_instantiation'.

'$check_head'(H) :-
	(   callable(H) ->
	    true
	;   '$pl_err_type'(callable, H)
	).




'$get_pred_indic'(PI, N, A) :-
	'$call_c_test'('Pl_Get_Pred_Indic_3'(PI, N, A)).
