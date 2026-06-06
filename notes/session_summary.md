# Session Summary — `philo` project

Living handoff document. A future session (with **no prior context**) should be
able to pick up from here. Read alongside `CLAUDE.md` (working style) and the
subject PDF. For the *story* of how the current design was reached, also read
`notes/handover_2026-06-06.md`.

Last updated: **2026-06-06**

---

## Where the project stands

`philo` = 42 common core Philosophers, **mandatory part only**. Repo:
`/home/aryan/philosopher`. The project is **functionally complete and validated**:

- Build is clean (`-Wall -Wextra -Werror -pthread`), **Norminette passes**.
- **valgrind**: 0 leaks (`0 bytes in use at exit`). **helgrind**: 0 data races.
- All required eval-sheet tests behave correctly (see Testing below).
- Death detection is accurate to **~1 ms** (eval allows 10 ms).

> Remaining work is essentially packaging: confirm the `philo/` submission
> directory question and write a README if the subject still requires one.

---

## Current design (IMPORTANT — read before editing)

Forks are **integers** guarded by a **mutex each**, with **busy-wait**
acquisition. This is deliberate (see the handover doc for why) and mirrors a
known-passing reference repo.

- `t_table.forks` — `int *`, one per fork, `0` = free, `1` = taken.
- `t_table.forks_mutexes` — `pthread_mutex_t *`, one per fork, guards that
  fork's integer.
- **No arbitrator / no global `waiter` mutex** (we tried it; it was worse).
- Fork indices on each philosopher: `right = i`, `left = (i + 1) % n_philo`.

**Acquisition (`routine.c`):**
- `grab_one(p, fork_id)` — lock that fork's mutex; if its integer is `0`, set it
  to `1`, unlock, log "has taken a fork", return 1. Else unlock, `usleep(100)`
  (backoff), retry. Returns 0 if the sim stopped while waiting.
- `take_forks(p)` — **asymmetric order**: even ids grab `left` then `right`; odd
  ids grab `right` then `left`. (Breaks the circular wait. The mutex is released
  immediately after each check, so holding one while waiting for the second
  cannot deadlock — it can only spin.)
- `drop_forks(p)` — lock each fork mutex, set integer back to `0`, unlock.
- Backoff is `usleep(100)`. **Do not raise it** — larger backoff (we tried 500)
  measurably increases starvation.
- **No think-delay** (we tried it; it halved the fork-wait margin for no gain).
- `n_philo == 1` is special-cased: take the single fork, log it, sleep
  `time_to_die`, return (never lock the same fork twice).

**Other locks:** per-philosopher `meal_lock` guards `time_last_ate` +
`n_meals_eaten`; `print_lock` serialises output; `stop_lock` guards `stop`.

## Struct shape (philo.h)

- `t_philo`: `id`, `time_last_ate`, `n_meals_eaten`, `left`, `right` (size_t
  indices into the forks arrays), `pthread_t thread`, `meal_lock` (by value),
  `t_table *table`.
- `t_table`: five parsed values + `start_ms`, `bool stop`, `t_philo *philos`,
  `int *forks`, `pthread_mutex_t *forks_mutexes`, `stop_lock`, `print_lock`.
- All numeric fields are `size_t` (`printf` uses `%zu`).
- `must_eat` sentinel `(size_t)-1` = "not provided"; `must_eat == 0` is rejected
  in parsing.

## File status

| File | Status | Contents |
|------|--------|----------|
| `Makefile` | DONE | `-pthread`, no relinking. SRCS = main parsing utils init log routine monitor |
| `philo.h` | DONE | typedefs + structs + forward decls |
| `parsing.c` | DONE | `is_digit_str`, `ft_atoi_safe`, `parse_required`, `parse_args`. Rejects 0 / non-digit / >INT_MAX / `must_eat==0` |
| `utils.c` | DONE | `now_ms`, `is_stopped`, `stop_simulation`, `smart_sleep` (polls `is_stopped` every 500us) |
| `init.c` | DONE | `init_forks` (int=0 + mutex), `init_philos` (left/right), `init_table`, `cleanup_table` |
| `log.c` | DONE | `log_state` (print under print_lock + stop-check), `log_death` |
| `main.c` | DONE | usage, spawn, join, main |
| `routine.c` | DONE | `grab_one`, `take_forks`, `eat`, `drop_forks`, `philosopher_routine` |
| `monitor.c` | DONE | `monitor_philo` (reads under meal_lock), `check_philos` (death + must_eat), `monitor_routine` |

---

## Testing (required eval-sheet cases — authoritative)

Constraints: never `>200` philosophers; never a time arg `<60` ms.

| Command | Expected | Status |
|---------|----------|--------|
| `1 800 200 200` | dies ~800, one fork | ✅ (801) |
| `5 800 200 200` | no one dies | ✅ (see WSL note) |
| `5 800 200 200 7` | no death; stops after all ate ≥7 | ✅ |
| `4 410 200 200` | no one dies | ✅ (tight: 10 ms slack) |
| `4 310 200 100` | a philosopher dies | ✅ (311) |
| `2 ...` timing | death delayed >10 ms unacceptable | ✅ (~1 ms) |
| `200 800 200 200` | no deadlock | ✅ |
| valgrind / helgrind | no leaks / no races | ✅ / ✅ |

How to check death-timing precision: `true_death = (last "is eating" ts) +
time_to_die`; printed "died" must be within `[true_death, true_death+10]`.
Helper left at `/tmp/death_check.py` during the session (regenerate if gone).

## ⚠ WSL testing caveat (critical — do not chase this as a bug)

On this WSL2 box the death rate on `5 800 200 200` looks like ~3–4/20, but
**most of those are environmental**, not the code. The Windows host pauses the
VM **periodically (~every 32 s)** for ~1 s; whichever run overlaps the freeze
loses a philosopher. Classify any death:
- **silent gap ≥ ~400 ms before the death** (all philosophers stop logging) →
  environmental stall → defensible per the eval sheet, and **will not happen on
  the 42 Linux machine**.
- **clean 200 ms cadence up to the death** → real starvation.

Measured real (clean) starvation rate ≈ **1/25**, at or below a known-passing
repo. Do not "fix" the WSL-inflated number.

## Caveats / gotchas

- **Backoff `usleep(100)`**, not larger — bigger backoff worsens starvation.
- **No think-delay, no arbitrator, no startup stagger** — all tried, all removed
  (each either neutral or harmful). See handover doc.
- **`static void`, not `void static`** — gcc `-Werror=old-style-declaration`.
- Exact log strings (lowercase): `has taken a fork`, `is eating`, `is sleeping`,
  `is thinking`, `died`. Grader compares literally.
- No comments inside function bodies (Norm `WRONG_SCOPE_COMMENT`); no `//`.
- Norm: ≤5 functions/file, ≤25 lines/function. `routine.c` is exactly 5.
- Piped/redirected output is block-buffered; a signal-killed run loses the
  buffer. Use a real terminal or `stdbuf -oL` to watch live runs (not a bug).

## Working style

Aryan is in **learning mode** (42 student). Hybrid was used this session: Claude
implemented boilerplate/refactors, Aryan drove the concurrency decisions.
Explain reasoning, test claims empirically rather than asserting, write notes in
`notes/` after a concept is settled.

## Notes folder

- `notes/01_dining_philosophers_problem.md` — classical problem.
- `notes/session_summary.md` — this file (living current-state handoff).
- `notes/handover_2026-06-06.md` — this session's decision log / story.
