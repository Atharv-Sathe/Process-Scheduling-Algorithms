#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <tuple>
using namespace std;


class Process {
  private:
  int pid; 
  int arrivalTime;
  int burstTime;
  int priority;
  int completionTime;
  int turnAroundTime;
  int waitTime;
  int responseTime;
  int remainingTime;
  
  public:
  Process(int pid, int arrT, int bT, int pty): pid(pid), arrivalTime(arrT), burstTime(bT), priority(pty), remainingTime(bT) {}
  Process(int pid, int arrT, int bT): pid(pid), arrivalTime(arrT), burstTime(bT), remainingTime(bT) {}
  
  int getPid() const {return pid;}
  int getArrivalTime() const {return arrivalTime;}
  int getBurstTime() const {return burstTime;}
  int getPriority() const {return priority;}
  int getCompletionTime() const {return completionTime;}
  int getTurnAroundTime() const {return turnAroundTime;}
  int getWaitTime() const {return waitTime;}
  int getRemainingTime() const {return remainingTime;}
  int getResponseTime() const {return responseTime;}
  
  void setPid(int val) {pid = val;}
  void setArrivalTime(int val) {arrivalTime = val;}
  void setBurstTime(int val) {burstTime = val;}
  void setPriority(int val) {priority = val;}
  void setCompletionTime(int val) {completionTime = val;}
  void setTurnAroundTime(int val) {turnAroundTime = val;}
  void setWaitTime(int val) {waitTime = val;}
  void setRemainingTime(int val) {remainingTime = val;}
  void setResponseTime(int val) {responseTime = val;}
  
};

// NOTE : The PID of the process is equal to its index in the processess vector.

// Global Processes Vector
vector<Process> processes;

void assignPrioritiesBasedOnBT() {
  // Higest Burst Time Gets Least Priority
  vector<pair<int, int>> processBurstWithIndex;

  for (int i = 0; i < processes.size(); i++) {
    processBurstWithIndex.push_back({processes[i].getBurstTime(), i});
  } 

  sort(begin(processBurstWithIndex), end(processBurstWithIndex));

  for (int i = 0; i < processBurstWithIndex.size(); i++) {
    processes[processBurstWithIndex[i].second].setPriority(i + 1);
  }
  
}

void getUserInput() {
  int pNum;
  cout << "Enter Number of Processes:- ";
  cin >> pNum;

  cout << "How would you like to assign priorities?\n";
  cout << "1. Burst Time \n2. Self Assign\n";
  cout << "Enter choice (1 or 2): ";
  int choice;
  cin >> choice;

  if (choice == 1) {
    cout << "Enter " << pNum << " process details ('Arrival Time' 'Burst Time'):\n";
    for (int i = 0; i < pNum; i++) {
      int arT, bT;
      cin >> arT >> bT;
      Process newP(i, arT, bT);
      processes.emplace_back(newP);
    }
    assignPrioritiesBasedOnBT();
  } else {
    cout << "Enter " << pNum << " process details ('Arrival Time' 'Burst Time' 'Priority'):\n"; 
    for (int i = 0; i < pNum; i++) {
      int arT, bT, pty;
      cin >> arT >> bT >> pty;
      Process newP(i, arT, bT, pty);
      processes.emplace_back(newP);
    }
  }
}

// TC: n(log n)
int getProcessToExecute(int currentTime, vector<Process>& processes, vector<bool>& completed) {
  // Filtering for elligible processs
  // Processes which have arrived at the current instant and are not completed 
  // are eligible to get scheduled at the current instant
  priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<tuple<int, int, int>>> elligibleProcesses;
  // The elligibleProcesses is equivalent to ready queue
  for (const auto& process: processes) {
    if (!completed[process.getPid()] && process.getArrivalTime() <= currentTime) {
      elligibleProcesses.push({process.getPriority(), process.getArrivalTime(), process.getPid()});
    } 
  }
  return (elligibleProcesses.size() > 0) ? get<2>(elligibleProcesses.top()) : -1;
}

vector<int> priorityScheduling() {
  int currentTime = 0;
  int completedProcesses = 0;
  int totalProcesses = processes.size();
  vector<bool> completed(totalProcesses, false);
  vector<int> executionSequence;

  while(completedProcesses < totalProcesses) {
    int pid = getProcessToExecute(currentTime, processes, completed);
    executionSequence.push_back(pid);
    currentTime++;
    if (pid != -1) {
      int remTime = processes[pid].getRemainingTime() - 1;
      processes[pid].setRemainingTime(remTime);
      if (remTime == 0) {
        completed[pid] = true;
        processes[pid].setCompletionTime(currentTime);
        completedProcesses++;
      } 
    }
  }
  return executionSequence;
}

int main() {
  getUserInput();
  vector<int> ganttChart = priorityScheduling();
  for (int i: ganttChart) {
    cout << i << " ";
  }
  return 0;
}