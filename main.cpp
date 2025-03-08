#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>

#include "PrintQueue.h"

//#define DEBUG

using namespace std;

mutex mtx; //General synchronization mutex to be used in functions.
mutex coutMtx; //Mutex to synchronize cout operations among threads, making sure that the output from different threads doesn't get mixed up.

void GeneratePJ(PrintQueue &queue, int userid,int maxPrintJobs, int minValRange, int maxValRange, int minValNumOfPages, int maxValNumOfPages);
void Printer(PrintQueue& printQueue, int maxPrintJobs);
int random_range(const int & min, const int & max);
void SayCurrentTime();

int ID = 0; // ID is made global variable for the sake of multi thread access.


int main() {
    
    //Necessary inputs taken from the user, the initial timestamp is made afterwards.
    int maxPrintJobs;
    int minValRange;
    int maxValRange;
    int minValNumOfPages;
    int maxValNumOfPages;
    
    cout << "Please enter the max number of print jobs:" << endl;
    cin >> maxPrintJobs;
    
    cout << "Please enter the min and max values for the time range between two print jobs:" << endl;
    cout << "Min: "; cin >> minValRange;
    cout << "Max: "; cin >> maxValRange;
    
    cout << "Please enter the min and max values for the number of pages in a print job:" << endl;
    cout << "Min number of pages: "; cin >> minValNumOfPages;
    cout << "Max number of pages: "; cin >> maxValNumOfPages; 
    
    cout << "Simulation starts: "; SayCurrentTime();
    
    // PrintQueue object is initialized and passed into the threads as reference so that they will be able to alter it directly.
    PrintQueue mainQueue;
    
    //Set of threads for function "GeneratePJ"
    thread user1(&GeneratePJ, ref(mainQueue), 1, maxPrintJobs, minValRange, maxValRange, minValNumOfPages, maxValNumOfPages);
    thread user2(&GeneratePJ, ref(mainQueue), 2, maxPrintJobs, minValRange, maxValRange, minValNumOfPages, maxValNumOfPages);
    thread user3(&GeneratePJ, ref(mainQueue), 3, maxPrintJobs, minValRange, maxValRange, minValNumOfPages, maxValNumOfPages);
    
    //Printer thread
    thread printer(&Printer, ref(mainQueue), maxPrintJobs);
    
    //Joining the threads
    user1.join();
    user2.join();
    user3.join();
    printer.join();
    
    //Finish timestamp
    cout << "End of the simulation at: "; SayCurrentTime();
    
    return 0;
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/* @brief Main processing function that will act as generator of PJ's with randomized fields,
 *        and lastly it will append the PrintJobNode to the queue for printer to dequeue and process afterwards.
 * @param "queue" [in][out] Reference to the main dynamic queue object which the enqueue operation will be applied on.
 * @param "userid" [in] Tracking id for the thread number.
 * @param "maxPrintJobs" [in] Upper limit for creating fresh PJ's. 
 * @param "minValRange" [in] Required value for minimum limit to create random time range.
 * @param "maxValRange" [in] Required value for maximum limit to create random time range.
 * @param "minValNumOfPages" [in] Required value for minimum limit to create random page numbers.
 * @param "maxValNumOfPages" [in] Required value for maximum limit to create random page numbers. 
 * @note  Debug code included.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void GeneratePJ(PrintQueue &queue, int userid,int maxPrintJobs, int minValRange, int maxValRange, int minValNumOfPages, int maxValNumOfPages){
    
    //I've used an static vector of type "id" from <thread>, algorithm will decide if the current unique thread is executing its first print job or not depending on
    //its id's availability in the sleptThreads vector.
    static vector<thread::id> sleptThreads;
    static mutex sleptThreadsMtx; //specific mutex for the operation of initialization sleeping.
    
    thread::id currID = this_thread::get_id();

    bool didSleep = false;
    
    {
        sleptThreadsMtx.lock(); // Manually lock the mutex
        for (thread::id thr : sleptThreads) {
            if (thr == currID) { //If current thread is present, do this:
                didSleep = true;
                break; // -> found, stop the iteration.
            }
        }
        sleptThreadsMtx.unlock(); // Manually unlock the mutex
    }

    if (didSleep == false) {
        
        int randomSleepTime1 = random_range(minValRange, maxValRange); // <--- Randomized sleep value for a thread to sleep for its initialization, kept seperate from mutexed blocks.
        this_thread::sleep_for(chrono::seconds(randomSleepTime1));     // <--- Initial Sleeping operation.
        
        #ifdef DEBUG
        cout << "(DEBUG)  Random FirstPrint SleepTime: " << randomSleepTime1 << endl;
        #endif
        
        //Ensuring push_back operation doesnt get interrupted by context switch
        sleptThreadsMtx.lock(); 
        sleptThreads.push_back(currID);
        sleptThreadsMtx.unlock(); 
    }
    
    while (ID < maxPrintJobs) { //If there arent sufficent ID's (ID track is below the upper limit), keep on generating new ones.
        
        mtx.lock();
        
        //Receiving random values from the randomizer function.
        int randomSleepTime2 = random_range(minValRange,maxValRange);
        int randomPageAmount = random_range(minValNumOfPages,maxValNumOfPages);
        
        #ifdef DEBUG
        cout << "(DEBUG) Random Sleep Time: " << randomSleepTime2 <<"  Random Page Amount: " << randomPageAmount << endl;
        #endif
        
        //Setting up a new PrintJobNode with the now-increased ID and the randomPageAmount that the randomizer provided.
        PrintJobNode PJ;
        PJ.PrintJobId = ++ID;
        PJ.numberOfPages = randomPageAmount;
        
        //enqueue-ing the PJ to the queue.
        queue = queue + PJ;
        
        coutMtx.lock();
        cout << "User " << userid << " sent new print job with ID " << ID << " sent to the printer queue, number of pages: "<< randomPageAmount << " (print queue size: " << queue.getCurrentSize() << ") "; SayCurrentTime(); 
        coutMtx.unlock();
        
        mtx.unlock();
        
        this_thread::sleep_for(chrono::seconds(randomSleepTime2)); //Sleeping is kept seperate from mutexed blocks as the THE4 paper requested.
    }
}


/* @brief Function will dequeue from the printQueue after that it will sleep the thread depending on dequeued objects "aaNumOFPages" data member. (aaNumOFPages x 1 second)
 * @param "printQueue" [in][out] Reference to the main dynamic queue object which the dequeue operation will be applied on.
 * @param "maxPrintJobs" [in] Again, a upper limit value on how many prints can be done.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Printer(PrintQueue& printQueue, int maxPrintJobs) {
    //Initial check to ensure that there wont be more print operations that the allowed ID numbers there are.
    while ((ID < maxPrintJobs) || (printQueue.isEmpty() == false)) {
        
        mtx.lock();
        
        //Should not operate if the queue is empty, which might be the case if the Printer thread wins the favor of Scheduler, so this a condition check for that.
        if (printQueue.isEmpty() == true) {
            mtx.unlock();
            continue; 
        }
        
        //dequeue and dissect the data members to aaID and aaNumOFPages variables which will be  used in cout lines as print info.
        int aaID, aaNumOFPages;
        printQueue.dequeue(aaID, aaNumOFPages);
        
        mtx.unlock();
        
        //Timestamp for the initialization of the current print operation.
        coutMtx.lock();
        cout << "The printer started to print the job with ID: " << aaID << ", number of pages: " << aaNumOFPages << " (queue size is: " << printQueue.getCurrentSize() << ") ";
        SayCurrentTime(); cout << printQueue << endl;
        coutMtx.unlock();
            
        this_thread::sleep_for(chrono::seconds(aaNumOFPages)); //Sleeping is kept seperate from mutexed blocks as the THE4 paper requested.
        
        //Timestamp for the finalization of the current print operation.
        coutMtx.lock();
        cout << "The printer finished printing the job with ID: " << aaID << ", number of pages: " << aaNumOFPages << " ";
        SayCurrentTime();
        coutMtx.unlock();
    }
}



//Function to timestamp the current time. 
void SayCurrentTime(){
    time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
    cout << put_time(localtime(&tt), "%X") << endl;
}



//Randomizer function, provided by THE4.
int random_range(const int & min, const int & max) {
    
    static mt19937 generator(time(0));
    
    uniform_int_distribution<int> distribution(min, max);
    
    return distribution(generator);

}