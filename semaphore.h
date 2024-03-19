#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "pcb.h" // Include the PCB definition for managing process queues

#define MAX_QUEUE_SIZE 10 // Maximum queue size for each semaphore
#include "list.h"
List **Scheduler_getPriorityQueues();

typedef struct Semaphore
{
    int value;                  // The semaphore value
    PCB *queue[MAX_QUEUE_SIZE]; // Queue of PCBs waiting on this semaphore
    int queueSize;              // Number of PCBs in the queue
} Semaphore;

// Function prototypes
void initializeSemaphore(Semaphore *semaphore, int initialValue);
void semaphoreP(Semaphore *semaphore, PCB *process);
void semaphoreV(Semaphore *semaphore);

#endif // SEMAPHORE_H
