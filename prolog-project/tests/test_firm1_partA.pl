/*
 * test_firm1_partA.pl - Tests for Member A's slice of Part 1:
 * the firm1.pl facts, same_department/2, and firmqry1.txt items a-f.
 *
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 Prolog Project: Describing a Firm
 *
 * Run from the prolog-project folder:
 *   swipl -q -g run_tests -t halt tests/test_firm1_partA.pl
 * Exit code 0 = all tests passed.
 */

:- use_module(library(plunit)).
:- use_module(library(lists)).

/* ------------------------------------------------------------------
 * Load firm1.pl, recording any warning or error printed while loading.
 * ------------------------------------------------------------------ */

:- dynamic load_message/2.
:- dynamic loading_firm1/0.
:- multifile user:message_hook/3.

user:message_hook(Term, Kind, _) :-
    loading_firm1,
    memberchk(Kind, [warning, error]),
    assertz(load_message(Kind, Term)),
    fail.

project_file(Name, Path) :-
    source_file(project_file(_, _), ThisFile),
    file_directory_name(ThisFile, TestDir),
    file_directory_name(TestDir, ProjectDir),
    directory_file_path(ProjectDir, Name, Path).

:- project_file('firm1.pl', F),
   assertz(loading_firm1),
   consult(F),
   retractall(loading_firm1).

/* ------------------------------------------------------------------
 * Loading
 * ------------------------------------------------------------------ */

:- begin_tests(loading).

test(no_warnings_or_errors_when_consulting_firm1) :-
    findall(K-T, load_message(K, T), Msgs),
    assertion(Msgs == []).

:- end_tests(loading).

/* ------------------------------------------------------------------
 * Facts: exactly what the spec describes, nothing more or less
 * ------------------------------------------------------------------ */

:- begin_tests(facts).

test(employees_exact) :-
    findall(E, employee(E), Es),
    assertion(Es == [mike, mary, martha, john, josh, julia, jack]).

test(departments_exact) :-
    findall(D, department(D), Ds),
    assertion(Ds == [production, sales, marketing, accounting]).

test(projects_exact) :-
    findall(P, project(P), Ps),
    assertion(Ps == [proj1, proj2, proj3]).

test(works_in_exact) :-
    findall(E-D, works_in(E, D), Pairs),
    msort(Pairs, Sorted),
    assertion(Sorted == [jack-marketing, john-sales, josh-sales,
                         julia-marketing, mary-production, mike-production]).

test(assigned_exact) :-
    findall(E-P, assigned(E, P), Pairs),
    msort(Pairs, Sorted),
    assertion(Sorted == [jack-proj3, josh-proj2, julia-proj2,
                         martha-proj1, martha-proj2, mary-proj1]).

test(no_duplicate_facts) :-
    forall(member(G, [employee(_), department(_), project(_),
                      works_in(_, _), assigned(_, _)]),
           ( findall(G, G, All),
             sort(All, Set),
             length(All, N), length(Set, N) )).

test(martha_has_no_department) :-
    \+ works_in(martha, _).

test(accounting_has_no_employees) :-
    \+ works_in(_, accounting).

test(unknown_names_are_not_employees) :-
    \+ employee(bob),
    \+ employee(production),
    \+ employee(proj1).

:- end_tests(facts).

/* ------------------------------------------------------------------
 * Consistency between facts
 * ------------------------------------------------------------------ */

:- begin_tests(consistency).

test(works_in_uses_known_employees_and_departments) :-
    forall(works_in(E, D), (employee(E), department(D))).

test(assigned_uses_known_employees_and_projects) :-
    forall(assigned(E, P), (employee(E), project(P))).

test(each_employee_in_at_most_one_department) :-
    forall(employee(E),
           ( findall(D, works_in(E, D), Ds), length(Ds, N), N =< 1 )).

test(all_names_are_lowercase_atoms) :-
    forall(( member(G, [employee(A), department(A), project(A),
                        works_in(A, _), works_in(_, A),
                        assigned(A, _), assigned(_, A)]),
             call(G) ),
           ( atom(A), atom_codes(A, [C|_]), code_type(C, lower) )).

:- end_tests(consistency).

/* ------------------------------------------------------------------
 * same_department/2
 * ------------------------------------------------------------------ */

:- begin_tests(same_department).

test(julia_and_jack, [nondet]) :-
    same_department(julia, jack).

test(julia_and_martha, [fail]) :-
    same_department(julia, martha).

test(martha_with_anyone, [fail]) :-
    same_department(martha, _).

test(different_departments, [fail]) :-
    same_department(mike, john).

test(not_reflexive, [fail]) :-
    same_department(mike, mike).

test(symmetric) :-
    forall(same_department(X, Y), same_department(Y, X)).

test(all_pairs_exact) :-
    findall(X-Y, same_department(X, Y), Pairs),
    msort(Pairs, Sorted),
    assertion(Sorted == [jack-julia, john-josh, josh-john,
                         julia-jack, mary-mike, mike-mary]).

test(julia_coworkers, [true(Ys == [jack])]) :-
    findall(Y, same_department(julia, Y), Ys).

:- end_tests(same_department).

/* ------------------------------------------------------------------
 * firmqry1.txt: read each query from the file, run it, and compare
 * the full list of answers with the expected answers for a-f.
 * ------------------------------------------------------------------ */

% expected(Item, Question, Answers)
%   Answers = list of variable bindings, one per solution.
%   [[]] = true once (yes/no query), [] = false.
expected(a, '% a. Is martha an employee?', [[]]).
expected(b, '% b. List all employees.',
         [['X'=mike], ['X'=mary], ['X'=martha], ['X'=john],
          ['X'=josh], ['X'=julia], ['X'=jack]]).
expected(c, '% c. List all those under the marketing department.',
         [['X'=julia], ['X'=jack]]).
expected(d, '% d. Are julia and jack under the same department?', [[]]).
expected(e, '% e. Are julia and martha under the same department?', []).
expected(f, '% f. Is john assigned to proj2?', []).

read_queries(Queries) :-
    project_file('firmqry1.txt', F),
    setup_call_cleanup(open(F, read, S),
                       read_all(S, Queries),
                       close(S)).

read_all(S, Queries) :-
    read_term(S, T, [variable_names(Vs)]),
    (   T == end_of_file
    ->  Queries = []
    ;   Queries = [T-Vs | Rest],
        read_all(S, Rest)
    ).

answers(Query-Vs, Answers) :-
    findall(Vs, call(Query), Answers).

query_file_lines(Lines) :-
    project_file('firmqry1.txt', F),
    read_file_to_string(F, Str, []),
    split_string(Str, "\n", "\r ", Lines).

:- begin_tests(firmqry1).

test(file_has_six_queries) :-
    read_queries(Qs),
    length(Qs, N),
    assertion(N == 6).

test(each_query_labelled_with_its_question_in_order) :-
    query_file_lines(Lines),
    findall(Q, expected(_, Q, _), Questions),
    include([L]>>(sub_string(L, 0, 4, _, P), member(P, ["% a.", "% b.", "% c.", "% d.", "% e.", "% f."])),
            Lines, Labels),
    maplist([A, S]>>atom_string(A, S), Questions, Labels).

test(query_answers_match_expected) :-
    read_queries(Qs),
    findall(I-Ans, expected(I, _, Ans), Expected),
    pairs_keys(Expected, Items),
    pairs_values(Expected, ExpectedAnswers),
    length(Qs, N), length(Items, N),
    maplist(answers, Qs, Actual),
    forall(nth1(K, Items, Item),
           ( nth1(K, ExpectedAnswers, Exp),
             nth1(K, Actual, Act),
             assertion(Item-Act == Item-Exp) )).

test(yes_no_queries_answer_true_exactly_once) :-
    read_queries(Qs),
    forall(( nth1(K, Qs, Q-[]), nth1(K, [a,b,c,d,e,f], Item),
             memberchk(Item, [a, d]) ),
           ( findall(x, call(Q), Xs), assertion(Xs == [x]) )).

:- end_tests(firmqry1).
