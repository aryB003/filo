# Session Summary — `philo` project

Handoff document so a future session (with **no prior context**) can pick up.
Read this together with `CLAUDE.md` (working style) and the subject PDF.

Last updated: **2026-05-27**

---

## Where the project stands

`philo` = 42 common core Philosophers, **mandatory part only**. Repo:
`/home/aryan/philosopher`. Developing at the **repo root** (not in a `philo/`
subdir — see caveat below). Build is clean (`-Wall -Wextra -Werror -pthread`)
and Norminette passes on everything written so far.

> ## ⚠ GENUINE GAP — read this first
>
> The simulation has **NEVER been verified end-to-end.** Death detection does
> not exist yet because `monitor.c` is still a stub. What's confirmed: the
> philosopher routine runs **deadlock-free** with correct concurrency and log
> format. What's **completely unverified**: "a philosopher dies on time",
> "nobody dies when they shouldn't", and the `must_eat` stop condition — because
> nothing currently detects death or ends the simulation. This is the real
> remaining risk. The first job next session is `monitor_routine`; until it
> exists and the testing recipe below passes, the project is NOT working.

### File status

| File | Status | Contents |
|------|--------|----------|
| `Makefile` | DONE | NAME=philo, `-pthread`, all rules, no relinking. `SRCS = main.c parsing.c utils.c init.c log.c routine.c monitor.c` |
| `philo.h` | DONE | Forward typedefs + `struct s_philo` / `struct s_table`; all fields; all forward decls. |
| `parsing.c` | DONE, tested | `is_digit_str`, `ft_atoi_safe`, `parse_required`, `parse_args`. 20+ edge cases pass. |
| `utils.c` | DONE | `now_ms`, `is_stopped`, `stop_simulation`, `smart_sleep`. |
| `init.c` | DONE | `init_forks`, `init_philos`, `init_table`, `cleanup_table`. |
| `log.c` | DONE | `log_state` (mutex print + stop-check), `log_death` (prints "died" + sets stop atomically). |
| `main.c` | DONE | `print_usage`, `spawn_threads`, `join_threads`, `main` (parse→init→spawn→join→cleanup). |
| `routine.c` | DONE, tested | `take_forks`, `eat`, `drop_forks`, `philosopher_routine`. Runs with NO deadlock; non-adjacent philos eat in parallel; output format correct. |
| `monitor.c` | **STUB — NEXT TASK** | `monitor_routine` returns immediately. No death detection yet → currently nobody ever dies / nothing stops. |

### Remaining work
1. **Implement `monitor_routine`** (the last core piece — see details below).
2. Edge-case testing (recipe below).
3. `README.md` (subject requires one).
4. Resolve the `philo/` submission-directory question.
5. valgrind (leaks) + helgrind (races) pass.
6. Commit / submit.

---

## How the user wants to work

- **Strict tutor mode (default):** Claude does NOT write the user's code. Explain
  in plain English, give ONE task at a time, provide **Google search queries** so
  the user finds answers themselves. Review what they write (correctness, Norm,
  allowed functions); don't rewrite it. Ask Socratic check-questions.
- **Hybrid exception (invoked 2026-05-26, deadline pressure):** Claude wrote the
  boilerplate (structs, init/cleanup, helpers, main wiring, file scaffolds); the
  **user writes the concurrency core** (philosopher routine, fork ordering,
  monitor). If resuming, confirm whether to stay hybrid or return to strict tutor.
- Notes go in `notes/`, written *after* a concept is discussed.

---

## Struct shape (philo.h)

- `t_philo`: `id`, `time_last_ate`, `n_meals_eaten` (`size_t`), `pthread_t thread`,
  `pthread_mutex_t *r_fork`, `*l_fork` (**pointers** into table's forks array),
  `pthread_mutex_t meal_lock` (**by value**), `t_table *table` (back-pointer).
- `t_table`: five parsed values + `start_ms` (`size_t`), `bool stop`,
  `t_philo *philos`, `pthread_mutex_t *forks` (malloc'd), `stop_lock`, `print_lock`.

## Key design decisions

- **All numeric fields `size_t`** (user's choice; Claude advised `long long`,
  user kept `size_t`). `printf` uses `%zu`.
- **`must_eat` sentinel = `(size_t)-1`** = "not provided".
- **Fork ordering (deadlock prevention):** even-id locks `l_fork` then `r_fork`;
  odd-id locks `r_fork` then `l_fork`. Asymmetry breaks the circular wait.
- **Per-philosopher `meal_lock`** guards `time_last_ate` + `n_meals_eaten`.
- **`print_lock`** serialises output; **`stop_lock`** guards `stop`.
- **`smart_sleep`** polls `is_stopped` every 500µs (precision + early wake).
- **`n_philo == 1`** special-cased: lock the single fork ONCE, log, sleep
  `time_to_die`, return. Never lock the same mutex twice.
- Struct tags are `s_philo` / `s_table` (Norm requires `s_` prefix).

## Caveats / gotchas learned

- **`static void`, NOT `void static`** — gcc `-Werror=old-style-declaration`
  rejects `void static` even though **Norminette accepts it**. Norm-clean ≠
  compiles; check BOTH.
- **`&` rule:** forks are pointers → `p->l_fork` (no `&`). `meal_lock` is
  by-value → `&p->meal_lock`. Easy to mix up.
- **Exact log strings** (lowercase): `has taken a fork`, `is eating`,
  `is sleeping`, `is thinking`, `died`. Grader compares literally.
- **No comments inside function bodies** (Norm `WRONG_SCOPE_COMMENT`); no `//`.
- **`write` count** = visible bytes, NOT including `\0` (`"gettimeofday() error\n"`
  = 21, not 22).
- **Output buffering:** piped/redirected `printf` block-buffers; if killed by a
  signal the buffer is lost. Use `stdbuf -oL` or a real terminal to see live
  output. Not a code bug.
- **Possible starvation tuning:** `philosopher_routine` currently has NO sleep
  after "is thinking". For odd N with tight timings this can starve someone.
  Watch tight `./philo 5 ...` cases; may need a think-delay
  `(time_to_die - time_to_eat - time_to_sleep)/2`.

---

## Monitor task (next to build)

Algorithm is in the comment block atop `monitor.c`. Key points:
- Loop while `!is_stopped(t)`. For each philo: lock `meal_lock`, copy out
  `time_last_ate` and `n_meals_eaten`, unlock, THEN act. Never call `log_death`
  while holding `meal_lock` (deadlock risk).
- Death: `now_ms() - time_last_ate > time_to_die` → `log_death(&philos[i])`; return.
- `must_eat` completion (only if `must_eat != (size_t)-1`): if EVERY philo has
  `n_meals_eaten >= must_eat` → `stop_simulation(t)`; return.
- Poll `usleep(500)` per outer iteration.
- **Open Socratic question posed to user (awaiting answer):** why must monitor AND
  philosopher use the *same* `meal_lock` — what breaks if the monitor reads
  without locking even though the writer locks?

## Testing recipe (once monitor is done)

- `./philo 1 800 200 200` → dies ~800ms, only one fork-pickup logged.
- `./philo 5 800 200 200` → runs forever, nobody dies.
- `./philo 5 800 200 200 7` → stops after every philo eats 7×.
- `./philo 4 310 200 100` → someone should die (tight).
- `./philo 200 800 200 200` → stress: no deadlock.
- `valgrind --leak-check=full` and `valgrind --tool=helgrind`.
- Verify: no message after "died"; timestamps monotonic; messages never overlap.

## Notes folder

- `notes/01_dining_philosophers_problem.md` — classical problem, deadlock,
  starvation, `⌊N/2⌋` parallelism, the 1-philosopher case.
- `notes/session_summary.md` — this file.
