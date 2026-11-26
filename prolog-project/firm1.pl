/*
 * firm1.pl - Knowledge base for the firm (Part 1).
 *
 * Authors: Keith Ayeras, Elijem Timothy Jaso, Dave Predigua
 * CSCI 70 Prolog Project: Describing a Firm
 *
 * Load with:  [firm1].
 * Queries for Part 1 are in firmqry1.txt.
 */

/* ------------------------------------------------------------------
 * Facts
 * ------------------------------------------------------------------ */

% employee(E): E is an employee of the firm.
employee(mike).
employee(mary).
employee(martha).
employee(john).
employee(josh).
employee(julia).
employee(jack).

% department(D): D is a department of the firm.
department(production).
department(sales).
department(marketing).
department(accounting).

% works_in(E, D): employee E is under department D.
% martha is not under any department; accounting has no employees.
works_in(mike, production).
works_in(mary, production).
works_in(john, sales).
works_in(josh, sales).
works_in(julia, marketing).
works_in(jack, marketing).

% project(P): P is a project the firm engages in.
project(proj1).
project(proj2).
project(proj3).

% assigned(E, P): employee E is assigned to project P.
assigned(mary, proj1).
assigned(martha, proj1).
assigned(martha, proj2).
assigned(josh, proj2).
assigned(julia, proj2).
assigned(jack, proj3).

/* ------------------------------------------------------------------
 * Rules a-f
 * ------------------------------------------------------------------ */

% same_department(X, Y): X and Y are two different employees
% under the same department.
same_department(X, Y) :- works_in(X, D), works_in(Y, D), X \= Y.

/* ------------------------------------------------------------------
 * Rules g-l
 * ------------------------------------------------------------------ */

% (Member B: add same_project/2, has_assigned/1, has_employees/1 here.)
