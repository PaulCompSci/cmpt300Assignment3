#include "scheduler.h"
#include "commands.h"
#include "list.h"
#include <stdio.h>

const int INIT_PROCESS_PID = 1;
const int INIT_PRIORITY = 0; // or whatever priority level you decide for "init"
extern int get_next_pid(void);
extern int nextPid;

int main()
{
    char command;
    int priority;
    int pid;

    // Initialization
    Scheduler_init();

    PCB *initProcess = createPCB(INIT_PROCESS_PID, INIT_PRIORITY); // Only pass priority, as createPCB now generates PID internally
    if (initProcess)
    {
        initProcess->pid = INIT_PROCESS_PID; // Explicitly set PID for init process
        initProcess->state = RUNNING;
        Scheduler_scheduleProcess(initProcess);
        Scheduler_setCurrentProcess(initProcess);
        nextPid = INIT_PROCESS_PID + 1;
        printf("Init process created and running with PID: %d and Priority: %d\n", INIT_PROCESS_PID, INIT_PRIORITY);
    }
    else
    {
        printf("Failed to create init process.\n");
        return -1;
    }
    nextPid = INIT_PROCESS_PID + 1;

     printf("Enter command (C - Create, F - Fork, K - Kill, E - Exit, Q - Quit): ");

    while (scanf(" %c", &command) == 1)
    { // Note the space before %c to skip any leading whitespace
        switch (command)
        {
        case 'C':
        case 'c': // For case-insensitivity
            printf("Enter priority (0=high, 1=norm, 2=low): ");
            if (scanf("%d", &priority) != 1)
            {
                printf("Invalid input for priority.\n");
                // Clean stdin buffer
                while (getchar() != '\n')
                    ;
                break;
            }
            Commands_CreateProcess(priority);
            break;
        case 'F':
        case 'f':
            Commands_Fork();
            break;
        case 'K':
        case 'k':
            printf("Enter PID of process to kill: ");
            if (scanf("%d", &pid) != 1)
            {
                printf("Invalid input for PID.\n");
                // Clean stdin buffer
                while (getchar() != '\n')
                    ;
                break;
            }
            Commands_Kill(pid);
            break;
        case 'Q':
        case 'q':
            printf("Exiting program.\n");
            return 0;

        case 'E':
        case 'e':
            Commands_Exit();
            break;
        default:
            printf("Invalid command.\n");
        }
        // In main.c, within the main loop where you prompt for user commands
        printf("Enter command (C - Create, F - Fork, K - Kill, E - Exit, Q - Quit): ");
    }

    return 0;
}