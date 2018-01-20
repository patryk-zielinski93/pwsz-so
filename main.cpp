#include <iostream>
#include <map>
#include <shared_mutex>
#include <thread>

using namespace std;

map<string, int> workers{
        {"Grzesiek", 2200},
        {"Bartek",   3450},
        {"Lukasz",   2400}
};

shared_timed_mutex mtx;

void updateWorker(const string &name, int salary) {
    lock_guard<shared_timed_mutex> writerLock(mtx);
    cout << "\nUpdating " << name;
    this_thread::sleep_for(chrono::milliseconds(1000));
    workers[name] = salary;
    cout << " ... update finished " << endl;
}

void getSalary(const string &name) {
    shared_lock<shared_timed_mutex> readerLock(mtx);
    cout << name << ": " << workers[name] << endl;
}

int main() {
    cout << endl;

    thread reader1 = thread(getSalary, "Grzesiek");
    thread reader2 = thread(getSalary, "Bartek");

    thread w1 = thread(updateWorker, "Grzesiek", 2500);

    thread reader3 = thread(getSalary, "Bob");
    thread reader4 = thread(getSalary, "Grzesiek");

    thread w2 = thread(updateWorker, "Lukasz", 4000);

    thread reader5 = thread(getSalary, "Grzesiek");
    thread reader6 = thread(getSalary, "Bartek");
    thread reader7 = thread(getSalary, "Grzesiek");
    thread reader8 = thread(getSalary, "Lukasz");

    reader1.join();
    reader2.join();
    reader3.join();
    reader4.join();
    reader5.join();
    reader6.join();
    reader7.join();
    reader8.join();
    w1.join();
    w2.join();

    cout << endl;

    cout << "\nThe new workers list" << endl;
    for (auto worker: workers) {
        cout << worker.first << ": " << worker.second << endl;
    }

    cout << endl;
}
