#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

typedef struct {
    int iPID;
    int iStart;
    int iEnd;
} Gantt;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time of Process P%d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}

void printGanttChart(int count, Gantt C[]) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < count; i++) {
        printf("| P%d ", C[i].iPID);
    }
    printf("|\n");
    for (int i = 0; i < count; i++) {
        printf("%d   ", C[i].iStart);
    }
    printf("%d\n", C[count - 1].iEnd);
}

void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    printf("Waiting Time: ");
    for (int i = 0; i < n; i++) {
        printf("P%d = %d | ", P[i].iPID, P[i].iWaiting);
        totalWaiting += P[i].iWaiting;
    }
    printf("\n");
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    printf("Turnaround Time: ");
    for (int i = 0; i < n; i++) {
        printf("P%d = %d | ", P[i].iPID, P[i].iTaT);
        totalTaT += P[i].iTaT;
    }
    printf("\n");
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[MAX_PROCESSES];
    Gantt Chart[MAX_PROCESSES * 10];  // Allow enough space for multiple switches
    int iNumberOfProcess, nGannt = 0;
    int timeQuantum;

    printf("Please input number of Processes: ");
    scanf("%d", &iNumberOfProcess);
    printf("Enter time quantum: ");
    scanf("%d", &timeQuantum);

    inputProcess(iNumberOfProcess, Input);

    int rt[MAX_PROCESSES];  // Remain time
    int completed = 0, curtime = 0, lastid = -1;
    int queue[MAX_PROCESSES], front = 0, rear = -1;
    int visited[MAX_PROCESSES] = {0};

    for (int i = 0; i < iNumberOfProcess; i++) {
        rt[i] = Input[i].iBurst;
    }

    // Add initial processes to the queue if they've arrived
    for (int i = 0; i < iNumberOfProcess; i++) {
        if (Input[i].iArrival <= curtime) {
            queue[++rear] = i;
            visited[i] = 1;
        }
    }

    while (completed != iNumberOfProcess) {
        if (front > rear) {
            curtime++;
            for (int i = 0; i < iNumberOfProcess; i++) {
                if (Input[i].iArrival <= curtime && !visited[i]) {
                    queue[++rear] = i;
                    visited[i] = 1;
                }
            }
            continue;
        }

        int index = queue[front++];

        // Mark start and response time if it's the first time the process is scheduled
        if (rt[index] == Input[index].iBurst) {
            Input[index].iStart = curtime;
            Input[index].iResponse = curtime - Input[index].iArrival;
        }

        // Add to Gantt chart if process switches
        if (lastid != Input[index].iPID) {
            if (nGannt > 0) {
                Chart[nGannt - 1].iEnd = curtime;
            }
            Chart[nGannt].iPID = Input[index].iPID;
            Chart[nGannt].iStart = curtime;
            nGannt++;
        }

        // Execute the process for either timeQuantum or remaining burst time
        int execTime = (rt[index] < timeQuantum) ? rt[index] : timeQuantum;
        curtime += execTime;
        rt[index] -= execTime;

        // If process is completed
        if (rt[index] == 0) {
            Input[index].iFinish = curtime;
            Input[index].iTaT = Input[index].iFinish - Input[index].iArrival;
            Input[index].iWaiting = Input[index].iTaT - Input[index].iBurst;
            completed++;

            // Mark end of the current Gantt segment
            Chart[nGannt - 1].iEnd = curtime;
        } else {
            // Requeue the process if it's not complete
            queue[++rear] = index;
        }

        lastid = Input[index].iPID;

        // Add new processes to queue if they have arrived
        for (int i = 0; i < iNumberOfProcess; i++) {
            if (Input[i].iArrival <= curtime && !visited[i]) {
                queue[++rear] = i;
                visited[i] = 1;
            }
        }
    }

    printf("\n===== Round Robin Scheduling =====\n");
    printGanttChart(nGannt, Chart);
    calculateAWT(iNumberOfProcess, Input);
    calculateATaT(iNumberOfProcess, Input);

    return 0;
}
