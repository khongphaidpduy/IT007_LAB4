#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

typedef struct {
    int iPID;         // ID của tiến trình
    int iStart;    // Thời điểm bắt đầu thực thi
    int iEnd;      // Thời điểm kết thúc hoặc bị gián đoạn
} Gantt;

void inputProcess(int n, PCB P[]) {
    //srand(time(NULL));
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time of Process P%d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
    }
}
void printProcess(int n, PCB P[]) {
    printf("| PID | Arrival | Burst |\n");
    for (int i = 0; i < n; i++) {
        printf("| P%d  |\t%d\t|  %d\t|\n", 
	P[i].iPID, P[i].iArrival, P[i].iBurst);
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
    printf("%d\n", C[count - 1].iEnd); // Thời điểm kết thúc của lần cuối
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}
void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}
int partition (PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (iCriteria == SORT_BY_ARRIVAL) {
            if ((P[j].iArrival < pivot.iArrival) || (P[j].iArrival == pivot.iArrival && P[j].iBurst < pivot.iBurst)) {
                i++;
                swapProcess(&P[i], &P[j]);
            }
        }

        if (iCriteria == SORT_BY_PID && P[j].iPID < pivot.iPID) {
            i++;
            swapProcess(&P[i], &P[j]);
        }

        if (iCriteria == SORT_BY_BURST && P[j].iBurst < pivot.iBurst) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
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
    PCB Input[100];
    Gantt Chart[100];  // Tối đa 100 lần chuyển đổi tiến trình trong biểu đồ Gantt
    int iNumberOfProcess, nGannt = 0;

    printf("Please input number of Processes: ");
    scanf("%d", &iNumberOfProcess);

    inputProcess(iNumberOfProcess, Input);

    int rt[100]; // remain time
    int ok[100]; // Mảng kiểm xem tiến trình đã bắt đầu hay chưa

    for (int i = 0; i < iNumberOfProcess; i++) {
        rt[i] = Input[i].iBurst;
        ok[i] = 0;
    }

    int complete = 0, curtime = 0, lastid = -1;
    while (complete != iNumberOfProcess) {
        int minid = -1;
        int minrm = 1e9;

        // Tìm tiến trình có Remain time nhỏ nhất chưa được hoàn thành
        for (int i = 0; i < iNumberOfProcess; i++) {
            if ((Input[i].iArrival <= curtime) &&(rt[i] < minrm) && rt[i] > 0) {
                minid = i;
                minrm = rt[i];
            }
        }


        if (minid == -1) {
            curtime++;
            continue;
        }

        // Tiến trình bắt đầu lần đầu tiên
        if (ok[minid] == 0) {
            ok[minid] = 1;
            Input[minid].iStart = curtime;
            Input[minid].iResponse =  Input[minid].iStart - Input[minid].iArrival;
        }

        // Tiến trình mới thay thế tiến trình trước đang chạy
        if (lastid != minid) {
            if (nGannt > 0) {
               Chart[nGannt- 1].iEnd = curtime;
            }
            Chart[nGannt].iPID = Input[minid].iPID;
            Chart[nGannt].iStart = curtime;
            nGannt++;
        }

        // Giảm thời gian thực hiện còn lại của tiến trình
        rt[minid]--;

        // Xử lý khi tiến trình hoàn thành
        if (rt[minid] == 0) {
            Input[minid].iFinish = curtime + 1;
            Input[minid].iTaT = Input[minid].iFinish - Input[minid].iArrival;
            Input[minid].iWaiting = Input[minid].iTaT - Input[minid].iBurst;
            complete++;
            if (complete == iNumberOfProcess)
                Chart[nGannt - 1].iEnd = curtime + 1;
        }

        lastid = minid;
        curtime++;
    }

    printf("\n===== SRTF Scheduling =====\n");
    printGanttChart(nGannt, Chart);
    calculateAWT(iNumberOfProcess, Input);
    calculateATaT(iNumberOfProcess, Input);
    return 0;
}
