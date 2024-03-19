#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "list.h" // Make sure to include the correct header for the list implementation
#include "pcb.h"  // Assuming PCB structure is defined in pcb.h
void Scheduler_setCurrentProcess(PCB* process);

#define NUM_PRIORITIES 3
// Initialize the scheduler. This should be called before any other scheduler function.
void Scheduler_init();

// Schedule a process. Adds the process to the scheduler in the appropriate priority queue.
void Scheduler_scheduleProcess(PCB* process);

// Get the next process to run based on priority and round-robin scheduling.
PCB* Scheduler_getNextProcess();

// Called when the time quantum for the currently running process expires.
void Scheduler_timeQuantumExpired();

#endif // SCHEDULER_H
