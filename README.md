*This project has been created as part of the 42 curriculum by abardhan*

# Philosophers

## Description

Philosophers is a 42 systems-programming project based on the classic **Dining
Philosophers problem**, originally formulated by Edsger Dijkstra. Its goal is to
learn the fundamentals of **concurrent programming**: running multiple threads in
parallel, sharing resources safely with **mutexes**, and reasoning about the
hazards that come with concurrency — **data races**, **deadlock**, and
**starvation**.

A number of philosophers sit around a round table. Between each pair of
philosophers lies one right fork, so there are exactly as many forks as philosophers.
A philosopher needs **two forks** (their left and right) to eat. The rest of the
time they sleep, then think. The simulation must keep every philosopher alive: a
philosopher who does not start eating within `time_to_die` milliseconds of their
last meal **dies**, and the simulation ends.

This repository contains the **mandatory part**, written in C using POSIX
**threads** (`pthread`) and **mutexes**:

- one thread per philosopher;
- one mutex lock per fork, guarding that fork's state;
- a dedicated monitor that watches for death and for the optional "everyone has
  eaten enough" stop condition;
- mutex-protected logging so the output is never interleaved.

The implementation avoids deadlock with an asymmetric fork-pickup order and keeps
philosophers fed by acquiring forks fairly, so the standard test cases run
indefinitely without anyone dying.

## Instructions

### Requirements

- A Unix-like system (developed and tested on Linux).
- A C compiler (`cc` or `gcc`) and `make`.
- The POSIX threads library (`pthread`), available by default on Linux.

### Build

The program and its `Makefile` live in the `philo/` directory:

```sh
cd philo
make
```

The executable produced by a successful `make` is called `philo`.

### Run

```sh
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

Important arguments:

`number_of_philosophers`: Number of philosophers (and of forks)
`time_to_die` (ms): If a philosopher does not start eating within this delay since their last meal (or since the start), they die.
`time_to_eat` (ms): Time a philosopher spends eating, holding both forks.
`time_to_sleep` (ms): Time a philosopher spends sleeping.
`number_of_times_each_philosopher_must_eat`: If every philosopher eats at least this many times, the simulation stops. This is an optional argument which can be used if needed to test certain cases.

Every state change is logged as:

```
timestamp_in_ms   philosopher_id   action
```

Where an `action` is one of the following:
- has taken a fork
- is eating
- is sleeping
- is thinking
- died

Once a philosopher dies, no further line is printed.

## Resources

### References

- E. W. Dijkstra, *Hierarchical ordering of sequential processes* — the origin of
  the Dining Philosophers problem.
- Dining philosophers problem — <https://en.wikipedia.org/wiki/Dining_philosophers_problem>
- Allen B. Downey, *The Little Book of Semaphores* — concise treatment of
  synchronization problems, including the dining philosophers.
- POSIX threads manual pages: `man pthread_create`, `man pthread_mutex_init`,
  `man pthread_mutex_lock`, `man gettimeofday`, `man usleep`.
- *Programming with POSIX Threads*, David R. Butenhof — reference for the pthreads
  API and concurrency patterns.

### Use of AI

AI was used during development in the following ways:

- **Learning and concept explanation** — understanding deadlock, starvation,
  mutex fairness, data races, memory visibility, and the project's timing model
  (death detection, the 10 ms detection tolerance, staggering, usleep delays).
- **Testing and validation** — building stress-test scripts, running and
  interpreting `valgrind` (memory leaks) and `helgrind` (data races), checking
  death-timing precision, and helping distinguish genuine bugs from
  environment-induced timing artifacts.
