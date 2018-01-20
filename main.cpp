#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include <random>

using namespace std;

mutex mtx;
const int philosophers_number = 5;

struct fork {
    mutex mutex;
};

struct table {
    atomic<bool> ready{false};
    array<fork, philosophers_number> forks;
};

struct philosopher {
private:
    string const name;
    table const &dinnertable;
    fork &left_fork;
    fork &right_fork;
    thread lifethread;
    mt19937 rng{random_device{}()};
public:
    philosopher(string_view n, table const &t, fork &l, fork &r) :
            name(n), dinnertable(t), left_fork(l), right_fork(r), lifethread(&philosopher::dine, this) {
    }

    ~philosopher() {
        lifethread.join();
    }

    void dine() {
        while (!dinnertable.ready);

        do {
            think();
            eat();
        } while (dinnertable.ready);
    }

    void print(string_view text) {
        lock_guard<mutex> cout_lock(mtx);
        cout << name << text << endl;
    }

    void eat() {
        lock(left_fork.mutex, right_fork.mutex);

        lock_guard<mutex> left_lock(left_fork.mutex, adopt_lock);
        lock_guard<mutex> right_lock(right_fork.mutex, adopt_lock);

        print(" started eating.");

        static thread_local uniform_int_distribution<> dist(1, 6);
        this_thread::sleep_for(chrono::milliseconds(dist(rng) * 50));

        print(" finished eating.");
    }

    void think() {
        static thread_local uniform_int_distribution<> wait(1, 6);
        this_thread::sleep_for(chrono::milliseconds(wait(rng) * 150));

        print(" is thinking.");
    }
};

void dine() {
    cout << "Dinner started!" << endl;

    {
        table table;
        array<philosopher, philosophers_number> philosophers
                {
                        {
                                {"P1", table, table.forks[0], table.forks[1]},
                                {"P2", table, table.forks[1], table.forks[2]},
                                {"P3", table, table.forks[2], table.forks[3]},
                                {"P4", table, table.forks[3], table.forks[4]},
                                {"P5", table, table.forks[4], table.forks[0]},
                        }
                };

        table.ready = true;
        this_thread::sleep_for(chrono::seconds(5));
        table.ready = false;
    }

    cout << "Dinner done!" << endl;
}

int main() {
    dine();

    return 0;
}
