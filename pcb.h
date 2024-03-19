#ifndef PCB_H
#define PCB_H

#include <stdbool.h>
#include "list.h" // Include the list implementation for dynamic message queue

#define MAX_MESSAGE_LENGTH 40
typedef enum
{
    RUNNING,
    READY,
    BLOCKED_ON_SEND,
    BLOCKED_ON_RECEIVE,
    BLOCKED_ON_SEMAPHORE,
    TERMINATED 
} ProcessState;

typedef struct Message
{
    char content[MAX_MESSAGE_LENGTH]; // Assume MAX_MESSAGE_LENGTH is defined somewhere
    int senderPid;                    // Process ID of the sender
} Message;

typedef struct ProcessControlBlock
{
    int pid;
    int priority;
    ProcessState state;
    List *messageQueue;   // Dynamic queue of messages
    int waitingSemaphore; // ID of the semaphore the process is waiting on, -1 if not waiting
    int senderPid;        // PID of the process from which a reply is expected, -1 if not waiting for reply
} PCB;

// Function prototypes
PCB *createPCB(int pid, int priority);
void destroyPCB(PCB *pcb);
bool sendMessage(PCB *receiver, const char *message, int senderPid);
bool receiveMessage(PCB *pcb, char *buffer, int *senderPid);
void blockOnSemaphore(PCB *pcb, int semaphoreId);
void unblockFromSemaphore(PCB *pcb);

#endif // PCB_H