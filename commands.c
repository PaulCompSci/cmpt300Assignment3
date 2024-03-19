#include "commands.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h> // for malloc and free

// Global variable to keep track of the next PID to assign
static int nextPid = 1;
// Assuming the PID for the 'init' process is defined globally
extern const int INIT_PROCESS_PID;
// Assuming there is a function to get the currently running process
extern PCB* Scheduler_getCurrentProcess();
// Assuming there is a function to get the next process ID
extern int get_next_pid();
// Assuming there is a function to schedule a process
extern void Scheduler_scheduleProcess(PCB* process);


// You'll need a function to remove a process from its list
extern int Scheduler_removeProcess(PCB* process);

// Helper function to duplicate PCB, needs to be implemented
static PCB* duplicate_pcb(const PCB* pcb); 
int get_next_pid(); 


// Function that handles creating a new process
int Commands_CreateProcess(int priority)
{
    // Ensure the priority is valid
    if (priority < 0 || priority >= NUM_PRIORITIES)
    {
        printf("Invalid priority level. Must be between 0 (high) and 2 (low).\n");
        return -1; // Return -1 on failure
    }

    // Create a new PCB for the process
    PCB *newPcb = createPCB(nextPid, priority); // Assumed createPCB function
    if (newPcb == NULL)
    {
        printf("Failed to create a new process.\n");
        return -1; // Return -1 on failure
    }

    // Add the new process to the scheduler
    Scheduler_scheduleProcess(newPcb);

    // Output the success message with the new process's PID
    printf("Process created successfully with PID: %d\n", nextPid);

    // Increment the PID for the next process
    nextPid++;

    return newPcb->pid; // Return the PID on success
}


// Function to handle forking of the current process
int Commands_Fork() {
    PCB* parentProcess = Scheduler_getCurrentProcess();
    if (parentProcess == NULL) {
        printf("No current process to fork.\n");
        return -1;
    }
    if (parentProcess->pid == INIT_PROCESS_PID) {
        printf("Cannot fork the 'init' process.\n");
        return -1;
    }

    // The createPCB function should assign a new unique PID for the new process.
    PCB* childProcess = createPCB(get_next_pid(), parentProcess->priority);
    if (childProcess == NULL) {
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
static PCB* duplicate_pcb(const PCB* pcb) {
    if (!pcb) {
        return NULL;
    }

    PCB* newPcb = (PCB*)malloc(sizeof(PCB));
    if (!newPcb) {
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


int get_next_pid() {
    return nextPid++; // Return the current value of nextPid, then increment it
}

// Helper function to find a process by PID in the scheduler's queues
static PCB* find_process_by_pid(int pid) {
    List** priorityQueues = Scheduler_getPriorityQueues();
    PCB* foundProcess = NULL;
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        List* queue = priorityQueues[i];
        for (Node* node = queue->head; node != NULL; node = node->next) {
            PCB* process = (PCB*)node->item;
            if (process->pid == pid) {
                foundProcess = process;
                break;
            }
        }
        if (foundProcess != NULL) {
            break;
        }
    }
    return foundProcess;
}

// Implementation of the Kill command
int Commands_Kill(int pid) {
    PCB* processToKill = find_process_by_pid(pid);
    if (processToKill == NULL) {
        printf("Process with PID %d not found.\n", pid);
        return -1;
    }

    // Additional checks and cleanup (e.g., semaphore queues) before killing the process
    // ...

    // Remove the process from the scheduler
    int result = Scheduler_removeProcess(processToKill);
    if (result != 0) {
        printf("Failed to remove process with PID %d from scheduler.\n", pid);
        return -1;
    }

    // Free the process's resources, such as its message queue
    destroyPCB(processToKill);

    printf("Process with PID %d killed successfully.\n", pid);
    return 0;
}
