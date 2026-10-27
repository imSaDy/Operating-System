# Operating Systems

My individual assignments for the Operating Systems course taught by Prof. Mehdi Kargahi at the University of Tehran, Fall 2025. All programs are written for Linux in C and C++. The assignment statements (in Persian) are in each folder.

The kernel project for this course, an extended xv6, was done in a team and is in a separate repository: [Enhanced-xv6-Operating-System](https://github.com/ParsaBukani/Enhanced-xv6-Operating-System).

| Folder | Topic | What I did |
| --- | --- | --- |
| [CA1](CA1) | Sockets | A flight-booking system with three programs: a central server, airlines that publish flights, and customers who reserve and buy tickets. They talk over TCP, with UDP broadcast for announcements. Each process handles several connections at once with `select()`, and `alarm()` is used for timeouts. |
| [CA2](CA2) | Pipes | A small shell that runs commands chained with `\|`, built on `fork()`, `pipe()` and `dup2()`. A second part benchmarks unnamed pipes against named pipes (FIFOs); the scripts, results and plots are in the folder and the analysis is in `report.ipynb`. |
| [CA3](CA3) | Threads and scheduling | Shows unpredictable thread interleaving, reproduces a race condition on a shared counter and fixes it with a mutex, and compares a multithreaded array sum on Linux and Windows. Part 2 is a CPU scheduling simulator that runs FCFS, SJF and Round Robin on the same workloads and compares them. |

`CA1/OS-CA1-810102588` is a copy of the CA1 submission as it was handed in.

## Building

Each part has its own Makefile. Run `make` in `CA1`, `CA2/Part1`, `CA3/Part1` or `CA3/Part2`.
