# Handoff: Member A → Members B and C

Member A's slice of Part 1 is done and tested (26/26 tests pass on SWI-Prolog 11.1.0). B can start adding rules g–l to `firm1.pl`, and C can code `firm2.pl` against the real facts.

## What's done

| File | Contents |
| --- | --- |
| `firm1.pl` | All Part 1 facts, the `same_department/2` rule, and an empty `Rules g-l` section for B |
| `firmqry1.txt` | Queries a–f, each under a `% <letter>. <question>` comment |
| `tests/test_firm1_partA.pl` | 26 plunit tests for the facts, `same_department/2` and queries a–f |

Facts, using the names from the naming contract in `GAMEPLAN.md`:

- `employee/1`: mike, mary, martha, john, josh, julia, jack
- `department/1`: production, sales, marketing, accounting
- `works_in/2`: mike, mary → production; john, josh → sales; julia, jack → marketing
- `project/1`: proj1, proj2, proj3
- `assigned/2`: mary, martha → proj1; martha, josh, julia → proj2; jack → proj3

martha has no department, and accounting has no employees. Both are on purpose: queries e and l depend on them.

`same_department(X, Y)` requires `X \= Y`, so nobody counts as sharing a department with themselves.

## Query results (checked at the real `?-` prompt)

| Item | Query | Answer |
| --- | --- | --- |
| a | `employee(martha).` | `true.` |
| b | `employee(X).` | mike, mary, martha, john, josh, julia, jack (press `;`) |
| c | `works_in(X, marketing).` | julia, jack (press `;`) |
| d | `same_department(julia, jack), !.` | `true.` |
| e | `same_department(julia, martha).` | `false.` |
| f | `assigned(john, proj2).` | `false.` |

## How to run the tests

From the `prolog-project` folder:

```
swipl -q -g run_tests -t halt tests/test_firm1_partA.pl
```

Exit code 0 means everything passed. On Windows, if `swipl` isn't on your PATH, use `"C:\Program Files\swipl\bin\swipl.exe"`.

The tests check:

- **Loading:** `firm1.pl` consults with zero warnings or errors.
- **Facts:** each predicate holds exactly the spec's facts, with no extras or duplicates. Every name is a lowercase atom.
- **Consistency:** `works_in` and `assigned` only use known employees, departments and projects. Nobody is in two departments.
- **`same_department/2`:** julia/jack is true, julia/martha is false, the rule is symmetric and not reflexive, and exactly 6 ordered pairs come back.
- **`firmqry1.txt`:** the tests read the file itself and run each query. There are exactly 6, labelled a–f in order, and each one's full list of answers matches the table above.

I also broke the code on purpose to confirm the tests notice. Adding a department for martha, removing an assignment, adding an extra or duplicate fact, letting `same_department` match X with itself, changing query b, a syntax error in the query file, and splitting a predicate's clauses apart all made the run fail.

## For Member B

- Add `same_project/2`, `has_assigned/1` and `has_employees/1` under the `Rules g-l` comment at the bottom of `firm1.pl`. Don't add facts there. All facts stay grouped at the top, or SWI-Prolog prints discontiguous warnings and the loading test fails.
- Add queries g–l to the end of `firmqry1.txt`, in the same `% <letter>. <question>` format.
- Run my test file after your changes. The `firmqry1` tests currently expect exactly 6 queries and items a–f only. When you add g–l, extend `expected/3` and the item lists in `tests/test_firm1_partA.pl` (or move those tests into your own file), or they will fail on the new queries.

## For Member C

- `firm2.pl` must not add clauses to any `firm1.pl` predicate, especially `assigned/2` or `works_in/2`. If it does, consulting it replaces the Part 1 clauses.
- `busy/1` relies on B's `has_employees/1`. Until B lands it, test with a local copy.

## Not in the submission

`tests/` and this file are for the team only. The zip holds just `firm1.pl`, `firmqry1.txt`, `firm2.pl`, `firmqry2.txt` and the COA.
