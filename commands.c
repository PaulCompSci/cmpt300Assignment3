#include "commands.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h> // for malloc and free

// Global variable to keep track of the next PID to assign
int nextPid = 1;
// Assuming the PID for the 'init' process is defined globally
extern const int INIT_PROCESS_PID;
// Assuming there is a function to get the currently running process
extern PCB *Scheduler_getCurrentProcess();
// Assuming there is a function to get the next process ID
extern int get_next_pid();
// Assuming there is a function to schedule a process
extern void Scheduler_scheduleProcess(PCB *process);

// You'll need a function to remove a process from its list
extern int Scheduler_removeProcess(PCB *process);

// Helper function to duplicate PCB, needs to be implemented
static PCB *duplicate_pcb(const PCB *pcb);
int get_next_pid();

// Function that handles creating a new process
int Commands_CreateProcess(int priority)
{
    if (priority < 0 || priority >= NUM_PRIORITIES)
    {
        printf("Invalid priority level. Must be between 0 (high) and 2 (low).\n");
        return -1;
    }

    int pid = get_next_pid();               // Correctly generate and increment PID
    PCB *newPcb = createPCB(pid, priority); // Use the generated PID
    if (newPcb == NULL)
    {
        printf("Failed to create a new process.\n");
        return -1;
    }

    Scheduler_scheduleProcess(newPcb);
    printf("Process created successfully with PID: %d\n", newPcb->pid);

    return newPcb->pid;
}

// Function to handle forking of the current process
int Commands_Fork()
{
    PCB *parentProcess = Scheduler_getCurrentProcess();
    if (parentProcess == NULL)
    {
        printf("No current process to fork.\n");
        return -1;
    }
    if (parentProcess->pid == INIT_PROCESS_PID)
    {
        printf("Cannot fork the 'init' process.\n");
        return -1;
    }

    // The createPCB function should assign a new unique PID for the new process.
    PCB *childProcess = createPCB(get_next_pid(), parentProcess->priority);
    if (childProcess == NULL)
    {
        printf("Failed to create a new process.\n");
        return -1;
    }

    // Duplicate the PCB's state, excluding the PID
    *childProcess = *parentProcess;
    childProcess->pid = get_next_pid(); // Ensure the child gets a unique PID

    // Ensure the child's message queue is a new, empty list
    childProcess->messageQueue = List_create();

    // Schedule the child process
    Scheduler_scheduleProcess(childProcess);

    printf("Forked process %d into new process %d.\n", parentProcess->pid, childProcess->pid);
    return childProcess->pid;
}

// Helper function that duplicates the given PCB
static PCB *duplicate_pcb(const PCB *pcb)
{
    if (!pcb)
    {
        return NULL;
    }

    PCB *newPcb = (PCB *)malloc(sizeof(PCB));
    if (!newPcb)
    {
        perror("Failed to allocate memory for new PCB");
        return NULL;
    }
    *newPcb = *pcb; // Start with a shallow copy

    // Update the PID for the child process
    newPcb->pid = get_next_pid();

    // Initialize or duplicate any fields that shouldn't be shared between processes
    // Depending on your PCB definition, this could include various resources
    // Example: If your PCB has a message queue, you'd need to initialize it here for the new PCB

    return newPcb;
}

int get_next_pid()
{
    return nextPid++; // Return the current value of nextPid, then increment it
}

// Helper function to find a process by PID in the scheduler's queues
static PCB *find_process_by_pid(int pid)
{
    List **priorityQueues = Scheduler_getPriorityQueues();
    PCB *foundProcess = NULL;
    for (int i = 0; i < NUM_PRIORITIES; i++)
    {
        List *queue = priorityQueues[i];
        for (Node *node = queue->head; node != NULL; node = node->next)
        {
            PCB *process = (PCB *)node->item;
            if (process->pid == pid)
            {
                foundProcess = process;
                break;
            }
        }
        if (foundProcess != NULL)
        {
            break;
        }
    }
    return foundProcess;
}

// Implementation of the Kill command
int Commands_Kill(int pid)
{
    PCB *processToKill = find_process_by_pid(pid);
    if (processToKill == NULL)
    {
        printf("Process with PID %d not found.\n", pid);
        return -1;
    }

    // Additional checks and cleanup (e.g., semaphore queues) before killing the process
    // ...

    // Remove the process from the scheduler
    int result = Scheduler_removeProcess(processToKill);
    if (result != 0)
    {
        printf("Failed to remove process with PID %d from scheduler.\n", pid);
        return -1;
    }

    // Free the process's resources, such as its message queue
    destroyPCB(processToKill);

    printf("Process with PID %d killed successfully.\n", pid);
    return 0;
}

int Commands_Exit() {
    PCB *currentProcess = Scheduler_getCurrentProcess();
    if (currentProcess == NULL) {
        printf("There is no currently running process to exit.\n");
        return -1;
    }

    List **priorityQueues = Scheduler_getPriorityQueues(); // Retrieve the array of priority queues
    bool areOtherProcessesActive = false;

    // Check if there are other processes ready to run or blocked
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        List *queue = priorityQueues[i];
        if (List_count(queue) > 0 && ((PCB *)List_first(queue))->pid != INIT_PROCESS_PID) {
            areOtherProcessesActive = true;
            break;
        }
    }
    
    // Check if the current process is the 'init' process and there are no other active processes
    if (currentProcess->pid == INIT_PROCESS_PID && !areOtherProcessesActive) {
        // It is safe to exit the 'init' process
        printf("Exiting the 'init' process.\n");
    } else if (currentProcess->pid == INIT_PROCESS_PID) {
        // If there are other active processes, do not exit the 'init' process
        printf("Cannot exit the 'init' process while other processes are active.\n");
        return -1;
    }

    // Remove the currently running process
    int result = Scheduler_removeProcess(currentProcess);
    if (result != 0) {
        printf("Failed to remove the current process with PID %d.\n", currentProcess->pid);
        return -1;
    }

    // Free the resources of the current process
    destroyPCB(currentProcess);

    // Now, decide the next course of action based on the state of the system
    PCB *nextProcess = Scheduler_getNextProcess();
    if (nextProcess) {
        // If there's another process ready to run, schedule it
        Scheduler_setCurrentProcess(nextProcess);
        printf("Process with PID %d is now running.\n", nextProcess->pid);
    } else {
        // If no other processes are ready to run, the system is idle or the simulation should terminate
        if (!areOtherProcessesActive) {
            printf("No other processes are active. Terminating simulation.\n");
            exit(0); // Or handle the end of simulation without exiting the entire program
        } else {
            printf("No more processes to run; the system is idle.\n");
            // Here you might want to handle the case where all processes are blocked and cannot proceed
            // For example, detect deadlock or wait for an event to unblock processes
        }
    }

    return 0; // Return success
}
