#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
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
  int waitCount;
  
  public:
  Process(int pid, int arrT, int bT, int pty): pid(pid), arrivalTime(arrT), burstTime(bT), priority(pty), remainingTime(bT), waitCount(0) {}
  Process(int pid, int arrT, int bT): pid(pid), arrivalTime(arrT), burstTime(bT), remainingTime(bT), waitCount(0) {}
  
  int getPid() const {return pid;}
  int getArrivalTime() const {return arrivalTime;}
  int getBurstTime() const {return burstTime;}
  int getPriority() const {return priority;}
  int getCompletionTime() const {return completionTime;}
  int getTurnAroundTime() const {return turnAroundTime;}
  int getWaitTime() const {return waitTime;}
  int getRemainingTime() const {return remainingTime;}
  int getResponseTime() const {return responseTime;}
  int getWaitCount() const {return waitCount;}
  
  void setPid(int val) {pid = val;}
  void setArrivalTime(int val) {arrivalTime = val;}
  void setBurstTime(int val) {burstTime = val;}
  void setPriority(int val) {priority = val;}
  void setCompletionTime(int val) {completionTime = val;}
  void setTurnAroundTime(int val) {turnAroundTime = val;}
  void setWaitTime(int val) {waitTime = val;}
  void setRemainingTime(int val) {remainingTime = val;}
  void setResponseTime(int val) {responseTime = val;}
  void setWaitCount(int val) {waitCount = val;}
  
};

// NOTE : The PID of the process is equal to its index in the processess vector.

// Global Processes Vector
vector<Process> processes;
// Global Variable to Store wheather to Implement Aging or not
int agingThreshold;

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
  cout << "Set Aging Threshold or (-1): ";
  cin >> agingThreshold;

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

// TC: n (log n)
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

  int pidSelect = (elligibleProcesses.size() > 0) ? get<2>(elligibleProcesses.top()) : -1;

  if (agingThreshold != -1) {
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<tuple<int, int, int>>> ready = elligibleProcesses;

    // Implement Aging
    while(ready.size() > 0) {
      tuple<int, int, int> p = ready.top();
      ready.pop();
      int pidNotSelect = get<2>(p);
      if (pidNotSelect != pidSelect) {
        processes[pidNotSelect].setWaitCount(processes[pidNotSelect].getWaitCount() + 1);
        // The Priority can't go beyond 0
        if (get<0>(p) > 0 && (processes[pidNotSelect].getWaitCount() >= agingThreshold)) {
          processes[pidNotSelect].setPriority(get<0>(p) - 1);
        }
      } else {
        // Resent the wait count if a process gets selected.
        processes[pidNotSelect].setWaitCount(0);
      }

    }
  }

  return pidSelect;
}

// TC : n * (n log n)
vector<int> priorityScheduling() {
  int currentTime = 0;
  int completedProcesses = 0;
  int totalProcesses = processes.size();
  vector<bool> completed(totalProcesses, false);
  vector<int> executionSequence;

  unordered_set<int> firstResponse;

  while(completedProcesses < totalProcesses) {
    int pid = getProcessToExecute(currentTime, processes, completed);
    if (firstResponse.count(pid) == 0) {
      processes[pid].setResponseTime(currentTime - processes[pid].getArrivalTime());
      firstResponse.insert(pid);
    }
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

tuple<double, double, double> calculateStats() {
  int n = processes.size();
  double tatAvg = 0, wtAvg = 0, rtAvg = 0;
  // Calculation of TAT = CT - AT, WT = TAT - BT
  for (auto& process : processes) {
    process.setTurnAroundTime(process.getCompletionTime() - process.getArrivalTime());
    tatAvg += process.getTurnAroundTime();
    process.setWaitTime(process.getTurnAroundTime() - process.getBurstTime());
    wtAvg += process.getWaitTime();
    rtAvg += process.getResponseTime();
  } 
  tatAvg /= n;
  wtAvg /= n;
  rtAvg /= n;

  return make_tuple(tatAvg, wtAvg, rtAvg);
}

void printStats(vector<int>& ganttChart, tuple<double, double, double>& stats) {
  cout << "\n<--- STATISTICS --->\n";
  cout << "Gantt Chart : ";
  for (int i: ganttChart) {
    cout << i << " ";
  }
  cout << endl;

  // Creating tabular format
  cout << "Pid\tPty\tAT\tBT\tCT\tTAT\tWT\tRT\t" << endl;
  for (const auto& process: processes) {
    cout << process.getPid() << "\t";
    cout << process.getPriority() << "\t";
    cout << process.getArrivalTime() << "\t";
    cout << process.getBurstTime() << "\t";
    cout << process.getCompletionTime() << "\t";
    cout << process.getTurnAroundTime() << "\t";
    cout << process.getWaitTime() << "\t";
    cout << process.getResponseTime() << "\t\n";
  }

  cout << "Turn Around Time Average: " << get<0>(stats) << endl;
  cout << "Wait Time Average       : " << get<1>(stats) << endl;
  cout << "Response Time Average   : " << get<2>(stats) << endl;
}

int main() {
  getUserInput();
  vector<int> ganttChart = priorityScheduling();
  tuple<double, double, double> stats = calculateStats();
  printStats(ganttChart, stats);
  return 0;
}