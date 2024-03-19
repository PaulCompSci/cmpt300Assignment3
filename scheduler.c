#include "scheduler.h"
#include "list.h"
#include <stdlib.h> // For NULL definition

// Define the number of priority levels
#define NUM_PRIORITIES 3

// Array of lists, one for each priority level
List *priorityQueues[NUM_PRIORITIES];
extern void *currentProcess;

// Current priority being served and index for round-robin within the queue
int currentPriority = 0;
void *currentProcess = NULL; // Pointer to current process for round-robin within a priority

List **Scheduler_getPriorityQueues()
{
    return priorityQueues;
}

void Scheduler_init()
{
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        priorityQueues[i] = List_create();
        if (priorityQueues[i] == NULL)
        {
            // Handle error: unable to create list for priority queue
        }
    }
    currentPriority = 0;
    currentProcess = NULL;
}

void Scheduler_scheduleProcess(PCB *process)
{
    if (process == NULL || process->priority < 0 || process->priority >= NUM_PRIORITIES)
    {
        // Invalid process or priority
        return;
    }

    // Set the process state to READY when it's scheduled.
    process->state = READY;
    List_append(priorityQueues[process->priority], process);
}

PCB *Scheduler_getNextProcess()
{
    // Iterate over priority levels from highest to lowest
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        if (List_count(priorityQueues[i]) > 0)
        {
            PCB *prevProcess = (PCB *)currentProcess;
            if (prevProcess)
            {
                prevProcess->state = READY; // Previous process is now ready
            }

            // Found the next process to run
            currentProcess = List_trim(priorityQueues[i]); // Get the first process from the queue
            if (currentProcess)
            {
                ((PCB *)currentProcess)->state = RUNNING; // New process is now running
            }
            return (PCB *)currentProcess;
        }
    }
    return NULL; // No process found, system idle
}

void Scheduler_timeQuantumExpired()
{
    PCB *currentPCB = (PCB *)currentProcess;
    if (currentPCB != NULL)
    {
        // Before moving to the next process, set the state of the current process to READY.
        currentPCB->state = READY;

        // Remove the current process from the front and re-insert it at the end of its priority queue for round-robin scheduling.
        List_append(priorityQueues[currentPCB->priority], currentPCB);
        currentProcess = NULL; // Clear the current process pointer
    }

    // Now, get the next process to run. The getNextProcess function will set the state of the chosen process to RUNNING.
    currentPCB = Scheduler_getNextProcess();

    // If there's no other process to run, the init process (which should always be available) is scheduled to run.
    if (currentPCB == NULL)
    {
        // Code to schedule the init process
        // Note: The init process should be maintained somewhere to be used here.
        // Set the state of the init process to RUNNING and update currentProcess pointer.
    }
}

// Function to get the currently running process
PCB *Scheduler_getCurrentProcess()
{
    return (PCB *)currentProcess; // Cast the void pointer to PCB*
}

int Scheduler_removeProcess(PCB *process)
{
    if (process == NULL)
    {
        return -1;
    }
    // Set the process state to indicate it is no longer scheduled
    process->state = TERMINATED; // Assuming TERMINATED is a defined state
    if (process == NULL)
    {
        return -1;
    }
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        List *queue = priorityQueues[i];
        for (Node *node = queue->head; node != NULL; node = node->next)
        {
            if (node->item == process)
            {
                // Found the process node, remove it
                if (node == queue->head)
                {
                    queue->head = node->next;
                }
                else
                {
                    node->previous->next = node->next;
                }
                if (node == queue->tail)
                {
                    queue->tail = node->previous;
                }
                else
                {
                    node->next->previous = node->previous;
                }
                // Free the node
                pushToFreeNodeStack(node);
                queue->size--;
                return 0;
            }
        }
    }
    return -1;
}

void Scheduler_setCurrentProcess(PCB *process)
{
    // If there's a process currently running, change its state to READY
    PCB *oldProcess = (PCB *)currentProcess;
    if (oldProcess)
    {
        oldProcess->state = READY;
    }

    currentProcess = process; // Update the global pointer to the current process

    // Set the new current process's state to RUNNING
    if (process)
    {
        process->state = RUNNING;
    }
}