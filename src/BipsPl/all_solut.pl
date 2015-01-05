/*-------------------------------------------------------------------------*
 * GNU Prolog                                                              *
 *                                                                         *
 * Part  : Prolog buit-in predicates                                       *
 * File  : all_solut.pl                                                    *
 * Descr.: all solution collector management                               *
 * Author: Daniel Diaz                                                     *
 *                                                                         *
 * Copyright (C) 1999-2015 Daniel Diaz                                     *
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


:-	built_in.

'$use_all_solut'.

:- meta_predicate(findall(?, 0, -)).
:- meta_predicate(findall(?, 0, -, ?)).
:- meta_predicate(bagof(?, 0, -)).
:- meta_predicate(setof(?, 0, -)).



findall(Template, MGenerator, Instances) :-
	'$check_list_arg'(Instances, findall, 3),
	'$strip_module_nonvar'(MGenerator, Module, Generator),
	'$findall'(Template, Generator, Module, Instances, [], findall, 3).

findall(Template, MGenerator, Instances, Tail) :-
	'$check_list_arg'(Instances, findall, 4),
	'$strip_module_nonvar'(MGenerator, Module, Generator),
	'$findall'(Template, Generator, Module, Instances, Tail, findall, 4).

'$findall'(Template, Generator, Module, Instances, Tail, Func, Arity) :-
	'$store_solutions'(Template, Generator, Module, Stop, Func, Arity),
	'$call_c_test'('Pl_Recover_Solutions_4'(Stop, 0, Instances, Tail)).




setof(Template, MGenerator, Instances) :-
	'$check_list_arg'(Instances, setof, 3),
	'$strip_module_nonvar'(MGenerator, Module, Generator),
	'$bagof'(Template, Generator, Module, Instances, setof, 3).
%	sort(Instances).




bagof(Template, MGenerator, Instances) :-
	'$check_list_arg'(Instances, bagof, 3),
	'$strip_module_nonvar'(MGenerator, Module, Generator),
	'$bagof'(Template, Generator, Module, Instances, bagof, 3).


'$bagof'(Template, Generator, Module, Instances, Func, Arity) :-
	'$call_c_test'('Pl_Free_Variables_4'(Template, Generator, Generator1, Key)), !,
	'$store_solutions'(Key - Template, Generator1, Module, Stop, Func, Arity),
	set_bip_name(Func, Arity),   % for error too_many_variables in C function
	'$call_c_test'('Pl_Recover_Solutions_4'(Stop, 1, AllInstances, [])),
	(   Func = bagof ->
	    keysort(AllInstances)
	;   sort(AllInstances)
	),
	'$group_solutions'(AllInstances, Key, Instances).

'$bagof'(Template, _, Module, Instances, Func, Arity) :-
	'$call_c'('Pl_Recover_Generator_1'(Generator)),
	'$findall'(Template, Generator, Module, Instances, [], Func, Arity),
	Instances \== [],
	(   Func = bagof ->
	    true
	;   sort(Instances)
	).




'$store_solutions'(Template, Generator, Module, Stop, Func, Arity) :-
	'$call_c'('Pl_Stop_Mark_1'(Stop)),
	(   '$call'(Generator, Module, Func, Arity),
	    '$call_c'('Pl_Store_Solution_1'(Template)),
	    fail
	;   true
	).




'$group_solutions'(AllInstances1, Key, Instances) :-
	'$call_c_test'('Pl_Group_Solutions_3'(AllInstances1, Key, Instances)).

'$group_solutions_alt' :-           % used by C code to create a choice-point
	'$call_c_test'('Pl_Group_Solutions_Alt_0').




         % Args testing

'$check_list_arg'(List, Func, Arity) :-
	set_bip_name(Func, Arity),
	'$check_list_or_partial_list'(List).
