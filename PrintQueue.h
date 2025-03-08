#ifndef PrintQueue_H
#define PrintQueue_H

#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <time.h>

using namespace std;

struct PrintJobNode {

    int PrintJobId, numberOfPages;

    PrintJobNode* next;

    PrintJobNode(int num = 0, int nPages = 0, PrintJobNode* ptr = nullptr)
    {
        PrintJobId = num;
        numberOfPages = nPages;
        next = ptr;
    }
};


class PrintQueue{
private:

    PrintJobNode* front;

    PrintJobNode* rear;

    int currentSize;

    friend ostream& operator<<(ostream& os, const PrintQueue& q); // << overload

public:

    PrintQueue();
    ~PrintQueue();
    int getCurrentSize() const;
    void enqueue(int id, int numberOfPages);
    void dequeue(int& id, int & pages);
    bool isEmpty() const;

    // you can add other functions if you need to, but you should not modify anything else neither in the class nor in the struct

    PrintQueue(const PrintQueue &copy); // Copy Constructor

    PrintJobNode* CreateClone() const; //utility

    void DisplayPrintQueue() const; //utility

    void DeletePrintQueue(); //mutator

    PrintJobNode* FindTail(PrintJobNode* head) const; //utility

    const PrintQueue& operator= (const PrintQueue &rhs); // = overload

    PrintQueue operator+ (const PrintJobNode &rhs); // + overload

    PrintQueue operator- (PrintJobNode &printJob); // - overload

};

#endif