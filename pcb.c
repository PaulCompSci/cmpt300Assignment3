#include "pcb.h"
#include <stdlib.h>
#include <string.h>

// Creates a new PCB instance with specified PID and priority
#include "pcb.h"
#include <stdlib.h>
#include <string.h>
extern int get_next_pid(void);

// Creates a new PCB instance with the specified priority and assigns a unique PID
PCB *createPCB(int pid, int priority) {
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    if (pcb == NULL) {
        return NULL;
    }

    pcb->pid = pid;  // Use the pid argument to assign the PID
    pcb->priority = priority;
    pcb->state = READY;
    pcb->messageQueue = List_create();

    if (pcb->messageQueue == NULL) {
        free(pcb);
        return NULL;
    }

    return pcb;
}

// Destroys a PCB instance and frees its allocated memory, including the message queue
void destroyPCB(PCB *pcb)
{
    if (pcb != NULL)
    {
        // Free all messages in the message queue
        void *message;
        while ((message = List_trim(pcb->messageQueue)) != NULL)
        {
            free(message); // Free each message structure
        }
        // Free the list itself
        List_free(pcb->messageQueue, NULL);
        free(pcb); // Free the PCB
    }
}

// Sends a message to a process, storing it in the receiver's message queue
bool sendMessage(PCB *receiver, const char *message, int senderPid)
{
    if (receiver == NULL || message == NULL)
    {
        return false;
    }

    // Create a new message and copy the data into it
    Message *newMsg = (Message *)malloc(sizeof(Message));
    if (newMsg == NULL)
    {
        // Failed to allocate memory for the new message
        return false;
    }
    strncpy(newMsg->content, message, MAX_MESSAGE_LENGTH - 1);
    newMsg->content[MAX_MESSAGE_LENGTH - 1] = '\0'; // Ensure null-termination
    newMsg->senderPid = senderPid;

    // Add the new message to the receiver's message queue
    if (List_append(receiver->messageQueue, newMsg) != LIST_SUCCESS)
    {
        free(newMsg); // If adding to the list failed, free the allocated memory for the message
        return false;
    }

    return true;
}

// Receives a message from the process's message queue
bool receiveMessage(PCB *pcb, char *buffer, int *senderPid)
{
    if (pcb == NULL || buffer == NULL || senderPid == NULL)
    {
        return false;
    }

    // Check if there are any messages in the queue
    if (List_count(pcb->messageQueue) == 0)
    {
        // No messages to receive
        return false;
    }

    // Get the first message from the queue
    Message *msg = (Message *)List_trim(pcb->messageQueue);
    if (msg == NULL)
    {
        // No messages in the queue
        return false;
    }

    // Copy the message content and sender PID to the provided buffer and senderPid
    strncpy(buffer, msg->content, MAX_MESSAGE_LENGTH);
    *senderPid = msg->senderPid;

    // Free the message after receiving it
    free(msg);

    return true;
}

// Function to block a process on a semaphore
void blockOnSemaphore(PCB *pcb, int semaphoreId)
{
    if (pcb != NULL)
    {
        pcb->waitingSemaphore = semaphoreId;
        pcb->state = BLOCKED_ON_SEMAPHORE; // Set the process state to blocked
    }
}

// Function to unblock a process from a semaphore
void unblockFromSemaphore(PCB *pcb)
{
    if (pcb != NULL && pcb->state == BLOCKED_ON_SEMAPHORE)
    {
        pcb->waitingSemaphore = -1; // No longer waiting on a semaphore
        pcb->state = READY;         // Set the process state back to ready
    }
}