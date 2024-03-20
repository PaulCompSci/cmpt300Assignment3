#ifndef COMMANDS_H
#define COMMANDS_H

// Include other necessary headers here, such as those for PCB and scheduler
#include "pcb.h"
#include "scheduler.h"

// Function to handle the 'Create' command which creates a new process
int Commands_CreateProcess(int priority);

int Commands_Fork();


int Commands_Kill(int pid);
int Commands_Exit();
#endif // COMMANDS_H
