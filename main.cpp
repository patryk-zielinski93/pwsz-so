#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cv;

int maxProducts = 100;
int product = 0;
int producersNumber = 1;
int consumersNumber = 30;

void consume(int orderNumber) {
    unique_lock<mutex> lck(mtx);
    while (product == 0) cv.wait(lck);
    cout << "Order: " << orderNumber << " released. ";
    cout << product - 1 << " products ready." << endl;
    product--;
}

void produce() {
    while (product <= maxProducts) {
        unique_lock<mutex> lck(mtx);
        product++;
        cv.notify_one();
    }
}

int main() {
    thread producers[producersNumber];
    thread consumers[consumersNumber];

    for (int order = 0; order < consumersNumber; order++) {
        consumers[order] = thread(consume, order);
    }

    for (int i = 0; i < producersNumber; i++) {
        producers[i] = thread(produce);
    }


    for (int order = 0; order < consumersNumber; order++) {
        consumers[order].join();
    }

    for (int order = 0; order < producersNumber; order++) {
        producers[order].join();
    }

    return 0;
}
