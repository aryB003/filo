# Concept 1: The Dining Philosophers Problem

This is the scenario the `philo` program simulates. The picture comes first;
every implementation choice flows from understanding it.

## The setup

- **N philosophers** sit around a **round table**.
- Between each pair of neighbours: **one fork**. So N philosophers ⇒ N forks.
- Each fork is shared with exactly one neighbour. Your left fork is also your
  left-neighbour's right fork.
- Each philosopher cycles through three states: **eat → sleep → think → eat → ...**
- To eat, a philosopher must be holding **both** their left and right fork.
- If a philosopher hasn't started a new meal within `time_to_die` ms of starting
  their previous meal (or of the simulation start, for their first meal), they
  **die** and the simulation ends.

Visual (N = 5):

```
              P1
          /        \
        F5          F1
       /              \
      P5               P2
      |                 |
      F4               F2
       \              /
        P4 --- F3 --- P3
```

## Why it's a *problem*, not just a simulation

Two facts combine to make this hard:

1. **Forks are shared resources.** Two neighbours both wanting to eat want the
   same fork — only one can have it at a time.
2. **Philosophers act independently.** No coordination, no communication. Each
   one runs its own little eat-sleep-think loop.

That combination produces three classic concurrency failure modes:

### Race condition
Two philosophers reach for the same fork at the same instant without any
synchronization, and both end up thinking they have it. Shared state is now
corrupted. **Fix:** a mutex per fork.

### Deadlock
Every philosopher grabs their left fork at the exact same instant. Now every
philosopher holds one fork and is waiting for their right fork — which is held
by their neighbour, who is also waiting. Nobody ever lets go. The system stalls
*permanently* — there's no path forward. Everyone eventually dies from
`time_to_die`.

This is qualitatively different from "being slow". **Fix:** break the symmetry
in how forks are picked up (e.g. some philosophers grab right-first, others
left-first; or always lock the lower-numbered fork first).

### Starvation
No deadlock — things are moving — but one philosopher's neighbours keep grabbing
the shared forks first. That philosopher never gets a turn to eat and eventually
dies. **Fix:** ordering and/or thinking-time tuning so everyone gets a fair shot.

## Key facts to internalize

- With N philosophers, the **maximum** number that can be eating simultaneously
  is `⌊N / 2⌋`. Two **adjacent** philosophers can never both eat at the same time.
- With **N = 1**, there is only **one fork**. The philosopher can never assemble
  two forks, so they must die after `time_to_die` ms. The program has to handle
  this without hanging.
- Each fork is shared between exactly two adjacent philosophers; each
  philosopher shares one fork with their left neighbour and one with their right.

## What comes next

The next concepts are the *tools* needed to actually implement this simulation:
- **Threads** — so each philosopher acts independently and simultaneously.
- **Mutexes** — so forks can't be grabbed by two philosophers at once.
- **Timing** — so death detection and meal durations are accurate.

Each of those concepts maps directly onto solving one of the problems above.
