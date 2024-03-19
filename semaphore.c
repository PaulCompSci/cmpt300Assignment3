#include "semaphore.h"
#include <stdlib.h>

// Initializes a semaphore with a given value
void initializeSemaphore(Semaphore *semaphore, int initialValue)
{
    if (semaphore != NULL)
    {
        semaphore->value = initialValue;
        semaphore->queueSize = 0; // No process is waiting initially
    }
}

// P (Wait) operation on a semaphore
void semaphoreP(Semaphore* semaphore, PCB* process) {
    if (semaphore == NULL || process == NULL) return;

    semaphore->value--;
    if (semaphore->value < 0) {
        // Block the process if semaphore value is negative
        if (semaphore->queueSize < MAX_QUEUE_SIZE) {
            semaphore->queue[semaphore->queueSize++] = process;
            process->state = BLOCKED_ON_SEMAPHORE;
        }
        // Else, you might want to handle the case where the semaphore queue is full.
    } else {
        // If the semaphore is not negative, the process continues without blocking.
        process->state = RUNNING;
    }
}


// V (Signal) operation on a semaphore
void semaphoreV(Semaphore* semaphore) {
    if (semaphore == NULL) return;

    semaphore->value++;
    if (semaphore->value <= 0 && semaphore->queueSize > 0) {
        // Unblock the first process in the queue if the semaphore value is non-positive
        PCB* process = semaphore->queue[0];
        
        // Shift the queue
        for (int i = 1; i < semaphore->queueSize; i++) {
            semaphore->queue[i - 1] = semaphore->queue[i];
        }
        semaphore->queueSize--;

        // Update the process state and reschedule it.
        process->state = READY;
        Scheduler_scheduleProcess(process);
        
        // Optionally, if you have a function to immediately switch to this process, call it here
        // e.g., Scheduler_setCurrentProcess(process);
    }
    // No need to wake up the process explicitly if it will be handled by the scheduler.
}
