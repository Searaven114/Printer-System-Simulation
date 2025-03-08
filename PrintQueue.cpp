#include "PrintQueue.h"
#include <iostream>

using namespace std;


//#define DEBUG

//************************************************
// Constructor. Generates an empty queue         *
//************************************************
PrintQueue::PrintQueue()
{
	front = nullptr;
	rear = nullptr;
	currentSize = 0;
}


void PrintQueue::enqueue(int num, int numberOfPages)
{
	if (isEmpty())   //if the queue is empty
	{	//make it the first element
		front = new PrintJobNode(num, numberOfPages);
		rear = front;
		currentSize++;

	}
	else  //if the queue is not empty
	{	//add it after rear
		rear->next = new PrintJobNode(num, numberOfPages);
		rear = rear->next;
		currentSize++;
	}
}

void PrintQueue::dequeue(int& num, int & pages)
{
	PrintJobNode* temp;
	if (isEmpty())
	{
		cout << "Attempting to dequeue on empty queue, exiting program...\n";
		exit(1);
	}
	else //if the queue is not empty
	{	//return front's value, advance front and delete old front
		num = front->PrintJobId;
		pages = front->numberOfPages;
		temp = front;
		front = front->next;
		delete temp;
		currentSize--;
	}
}

int PrintQueue::getCurrentSize() const
{
	return currentSize;
}

bool PrintQueue::isEmpty() const
{
	if (front == nullptr)
		return true;
	else
		return false;
}



//------------------------------------------------------------------------------------------------------------------------
// Newly added functions below

//Prints the queue info in the requested format of THE4.
void PrintQueue::DisplayPrintQueue() const
{
    if (isEmpty()) {
        cout << "The print queue is empty." << endl;
    }
    else {
        PrintJobNode* current = front;
        cout << "The print queue contains:" << endl;
        while (current != nullptr) {
            cout << "Print Job ID: " << current->PrintJobId << ", Pages: " << current->numberOfPages << endl;
            current = current->next;
        }
        //cout << "currentsize: " << currentSize << endl;
    }
}


//Deletes the queue, seperate from implicit destructor
void PrintQueue::DeletePrintQueue(){

    if(!isEmpty()){

        //Apply linear search and delete each node unill front is null, also set the rear to null afterwards.
        PrintJobNode* temp;
        while(front != nullptr){
            temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
        currentSize = 0;
    } else {
        #ifdef DEBUG
        cout << "There is nothing to delete ! \n";
        #endif
    }
}


// implicit Destructor
PrintQueue::~PrintQueue() {
    #ifdef DEBUG
    cout << "DESTRUCTOR INVOKED ! \n";
    #endif

    PrintJobNode* ptr;
    while (front != nullptr) {
        ptr = front;
        front = front->next;
        delete ptr;
    }
    rear = nullptr;

    currentSize = 0;
}


// Assignment operator overload
const PrintQueue& PrintQueue::operator= (const PrintQueue &rhs){

    //If not self assigment
    if (this != &rhs){

        //Delete lhs
        DeletePrintQueue();

        //copy contents
        front = rhs.CreateClone();
        rear = FindTail(front);
        currentSize = rhs.getCurrentSize();
    }

    return *this; //returning the object

}


//Helper function to be used in overloads, usually to locate rear
PrintJobNode* PrintQueue::FindTail(PrintJobNode* head) const {

    if (head == nullptr) { return nullptr; }

    PrintJobNode* temp = head;
    while (temp->next != nullptr) {

        temp = temp->next;
    }
    return temp;
}


//Clones the queue via head(front)
PrintJobNode* PrintQueue::CreateClone() const {

    if (front == nullptr) { return nullptr; } //queue is empty, return null

    //Handle the initial node
    PrintJobNode* headClone = new PrintJobNode(front->PrintJobId, front->numberOfPages);
    PrintJobNode* ptr = front->next;
    PrintJobNode* ptrClone = headClone;

    //Handle the rest via linear iteration
    while (ptr != nullptr) {
        ptrClone->next = new PrintJobNode(ptr->PrintJobId, ptr->numberOfPages);
        ptr = ptr->next;
        ptrClone = ptrClone->next;
    }

    ptrClone->next = nullptr;

    return headClone;
}


//Copy Constructor
PrintQueue::PrintQueue(const PrintQueue &copy){

    #ifdef DEBUG
    cout << "COPY CONSTRUCTOR INVOKED ! \n";
    #endif

    //deep copy the rhs with all its data members
    front = copy.CreateClone();
    rear = FindTail(front);
    currentSize = copy.getCurrentSize();
}


//Overload of + operator
PrintQueue PrintQueue::operator+ (const PrintJobNode &rhs){

    //Generate a new PrintQueue as a copy of lhs
    PrintQueue result(*this);

    //Enqueue rhs to that PrintQueue
    result.enqueue(rhs.PrintJobId, rhs.numberOfPages); // Enqueue the new print job

    return result; // Return the newly created PrintQueue object, (still dont know why we didnt alter the original queue)
}


//Overload of - operator
PrintQueue PrintQueue::operator- (PrintJobNode &printJob) {
    PrintQueue result(*this); // Create a copy of the current object

    if (!result.isEmpty()) { //ifnot empty guard
        result.dequeue(printJob.PrintJobId, printJob.numberOfPages); // <---- load the dequeued elements info to newly created PrintJobNode and return a new instance of the used PrintQueue object

    } else {
        cout << "Attempting to dequeue on empty queue.\n";
    }

    return result; //Return the new "dequeued" instance
}


//Overloaded << operator, will utilize the DisplayPrintQueue function to match the requirements of THE4 sample runs
ostream& operator<<(ostream& os, const PrintQueue& q) {
    q.DisplayPrintQueue();
    return os;
}
