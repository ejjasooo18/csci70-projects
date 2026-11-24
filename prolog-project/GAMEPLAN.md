# CSCI 70 Prolog Project Game Plan

We submit one zip with `firm1.pl`, `firmqry1.txt`, `firm2.pl`, `firmqry2.txt` and our COA. Fix predicate names together first, then each member builds one slice in parallel.

## Team split

Each member owns one slice end to end: the rules, the queries and testing them. Member C also packages the submission.

| Member | Owns | Files | Done when |
| --- | --- | --- | --- |
| A | Part 1 facts + rules for queries a–f | `firm1.pl` (facts and rules a–f), `firmqry1.txt` items a–f | All facts from the description are encoded, and a–f give the answers listed in Part 1 |
| B | Part 1 rules for queries g–l + Part 1 QA | `firm1.pl` (rules g–l), `firmqry1.txt` items g–l | g–l give the expected answers, and all 12 Part 1 queries pass in a fresh SWI-Prolog session |
| C | Part 2 + packaging | `firm2.pl`, `firmqry2.txt`, the zip | Part 2 a–f give the spec's answers after `[firm1]. [firm2].`, and the zip has all 5 items |
| All | COA + final review | COA | Everyone has signed the COA and run every query once on their own machine |

A and B both edit `firm1.pl`. A writes the facts first, then B adds rules under a `% --- Rules g–l ---` comment so the two halves don't collide.

## Shared naming contract

Agree on these names in the first meeting. Everyone codes against them, so B and C can start before A finishes. All atoms are lowercase, as in the lecture slides.

| Predicate | Meaning | Defined in | Owner |
| --- | --- | --- | --- |
| `employee(E)` | E is an employee | `firm1.pl` (fact) | A |
| `department(D)` | D is a department | `firm1.pl` (fact) | A |
| `works_in(E, D)` | E is under department D | `firm1.pl` (fact) | A |
| `project(P)` | P is a project | `firm1.pl` (fact) | A |
| `assigned(E, P)` | E is assigned to project P | `firm1.pl` (fact) | A |
| `same_department(X, Y)` | X and Y are under the same department | `firm1.pl` (rule) | A |
| `same_project(X, Y)` | X and Y share at least one project | `firm1.pl` (rule) | B |
| `has_assigned(D)` | Department D has an employee on a project | `firm1.pl` (rule) | B |
| `has_employees(D)` | Department D has at least one employee | `firm1.pl` (rule) | B |
| `status(P, S)` | Project P is `ongoing` or `completed` | `firm2.pl` (fact) | C |
| `head(D, E)` | E heads department D | `firm2.pl` (fact) | C |
| `busy(D)` | D has employees, and all of them have projects | `firm2.pl` (rule) | C |
| `efficient(E)` | E has projects, and all of them are completed | `firm2.pl` (rule) | C |

## Part 1: firm1.pl and firmqry1.txt

The knowledge base needs 7 employee facts, 4 department facts, 6 `works_in` facts, 3 project facts and 6 `assigned` facts. Martha has no department, and accounting has no employees. Queries e and l depend on that.

Rule sketches, using the same style as the lecture's `planet` and `satellite` rules:

```prolog
same_department(X, Y) :- works_in(X, D), works_in(Y, D), X \= Y.
same_project(X, Y)    :- assigned(X, P), assigned(Y, P), X \= Y.
has_assigned(D)       :- works_in(E, D), assigned(E, _).
has_employees(D)      :- works_in(_, D).
```

Expected answers. Whoever tests checks every row against SWI-Prolog.

| Item | Query in firmqry1.txt | Expected | Owner |
| --- | --- | --- | --- |
| a | `employee(martha).` | true | A |
| b | `employee(X).` | mike, mary, martha, john, josh, julia, jack | A |
| c | `works_in(X, marketing).` | julia, jack | A |
| d | `same_department(julia, jack), !.` | true | A |
| e | `same_department(julia, martha).` | false | A |
| f | `assigned(john, proj2).` | false | A |
| g | `assigned(martha, P).` | proj1, proj2 | B |
| h | `same_project(martha, josh), !.` | true (both on proj2) | B |
| i | `assigned(X, proj1).` | mary, martha | B |
| j | `setof(D, has_assigned(D), Ds).` | Ds = [marketing, production, sales] | B |
| k | `has_employees(marketing), !.` | true | B |
| l | `has_employees(accounting).` | false | B |

For j, use `setof` so marketing is listed once, not twice (julia and jack are both assigned).

## Part 2: firm2.pl and firmqry2.txt

The spec only gives proj3's status, so we pick the rest to match its answers. proj1 and proj2 must be completed, or martha can't be efficient. `firm2.pl` adds new predicates only and never repeats a `firm1.pl` predicate.

```prolog
status(proj1, completed).
status(proj2, completed).
status(proj3, ongoing).

head(marketing, mike).   % stated by the spec, even though mike is in production

% busy: has employees, and no employee of D lacks a project
busy(D)      :- has_employees(D), \+ (works_in(E, D), \+ assigned(E, _)).

% efficient: has projects, and none of them is ongoing
efficient(E) :- assigned(E, _), \+ (assigned(E, P), status(P, ongoing)).
```

The "all" in busy and efficient is written as "no counterexample exists" using `\+` (negation as failure).

| Item | Query in firmqry2.txt | Expected | Why |
| --- | --- | --- | --- |
| a | `status(proj3, S).` | S = ongoing | Fact |
| b | `head(marketing, H).` | H = mike | Fact |
| c | `busy(marketing), !.` | true | julia is on proj2, jack is on proj3 |
| d | `busy(sales).` | false | john has no project |
| e | `efficient(jack).` | false | proj3 is ongoing |
| f | `efficient(martha), !.` | true | proj1 and proj2 are completed |

Run all of these after `[firm1]. [firm2].` in the same session.

## Work order

The project is due **Tue, Dec 1**. We aim to submit on **Mon, Nov 30**, keeping Dec 1 as a buffer day. Steps 2–4 run in parallel once the names are agreed.

1. **Wed, Nov 25 — Kickoff, all (30 min).** Agree on the naming contract above and the query-file format. Everyone installs SWI-Prolog, or uses SWISH.
2. **Thu, Nov 26 — A:** write the `firm1.pl` facts and push them first (by noon), then the rules and queries for a–f by end of day.
3. **Thu, Nov 26 – Fri, Nov 27 — B:** write the rules and queries for g–l against the contract, then add them under A's facts.
4. **Thu, Nov 26 – Fri, Nov 27 — C:** write `firm2.pl` and `firmqry2.txt`. Test against a local copy of A's facts until the real ones land.
5. **Sat, Nov 28 — B:** run all 12 Part 1 queries in a fresh session and fill in any mismatches.
6. **Sat, Nov 28 — C:** run Part 2 after `[firm1]. [firm2].` and confirm no redefinition warnings.
7. **Sun, Nov 29 — All:** cross-review by swapping slices (A checks C, B checks A, C checks B). Sign the COA.
8. **Mon, Nov 30 — C:** zip the four files with the signed COA and submit on Canvas. Check Canvas for spec updates first.
9. **Tue, Dec 1 — Deadline.** Buffer only, for fixes if anything slipped.

## Pitfalls and final checklist

The biggest risk is `firm2.pl` redefining a `firm1.pl` predicate. SWI-Prolog then replaces the firm1 clauses with firm2's, and Part 1 facts disappear.

- [ ] `firm2.pl` defines only new predicates (`status`, `head`, `busy`, `efficient`) and never adds `assigned` or `works_in` clauses
- [ ] `firm1.pl` was not changed for Part 2, as the spec requires
- [ ] Clauses with the same head are grouped together, as the lecture tip says, so there are no discontiguous warnings
- [ ] All names are lowercase atoms, and variables start with an uppercase letter
- [ ] Yes/no queries end with `, !.` so they stop after the first true
- [ ] Each query in the .txt files sits under a comment with its item letter and question, e.g. `% a. Is martha an employee?`
- [ ] Every expected answer in the Part 1 and Part 2 tables matches a fresh SWI-Prolog run
- [ ] The zip holds `firm1.pl`, `firmqry1.txt`, `firm2.pl`, `firmqry2.txt` and the signed COA
