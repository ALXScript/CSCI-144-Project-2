#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>
#include<condition_variable>

using namespace std;

//make the MAX variable
//Buffer Sizes - 25, 50, 100
const int MAX = 25;

//****************CLASS DECLARATION****************
class BBQ {
	//create sync variables
	mutex mutexLock;
	condition_variable itemAdded;
	condition_variable itemRemoved; 

	//create actual variables
	int items[MAX];
	int front;
	int nextEmpty;

public:
	BBQ();
	~BBQ();
	bool insert(int passItem, thread::id passThreadID);
	bool remove(int *passItem, thread::id passThreadID);
};

//Constructor to set the front and next empty variables to 0
BBQ::BBQ() {
	front = 0;
	nextEmpty = 0;
}

//Destructor variable
BBQ::~BBQ() {};

//Wait until there is room and hten insert an item.
// bool BBQ::insert(int passItem) {
// 	bool success = false;

// 	mutexLock.lock();
// 	if ((nextEmpty - front) < MAX) {
// 		items[nextEmpty % MAX] = passItem;
// 		nextEmpty++;
// 		success = true;
// 	}
// 	mutexLock.unlock();
// 	return success;
// };
bool BBQ::insert(int passItem, thread::id passThreadID){
	bool success = false;

	//acquire the lock
	//mutexLock.lock();
	unique_lock<mutex> uLock(mutexLock);

	//while loop
	while ((nextEmpty - front) == MAX){
		itemRemoved.wait(uLock);
		printf("\nWaiting to produce by thread number: %d\n", passThreadID);
	}

	//execute the insertion
	items[nextEmpty % MAX] = passItem;
	nextEmpty++;
	success = true;

	printf("\nItem ID %d produced by thread number: %d\n", passItem, passThreadID);
	
	//signal to the waiting thread
	itemAdded.notify_one();

	//release the lock
	//mutexLock.unlock();
	uLock.unlock();

	return success;
}

// bool BBQ::remove(int *passItem) {
// 	bool success = false;

// 	mutexLock.lock();
// 	if (front < nextEmpty) {
// 		*passItem = items[front % MAX];
// 		front++;
// 		success = true;
// 	}
// 	mutexLock.unlock();

// 	return success;
// };
bool BBQ::remove(int *passItem, thread::id passThreadID){
	bool success = false;

	//acquire the lock
	unique_lock<mutex> uLock(mutexLock);

	//while loop
	while (front == nextEmpty){
		itemAdded.wait(uLock);
		printf("\nWaiting to consume by thread number: %d\n", passThreadID);
	}

	//execute the insertion
	*passItem = items[front % MAX];
	front++;
	success = true;

	printf("\nItem ID %d consumed by thread number: %d\n", item, threadID);
	
	//signal to the waiting thread
	itemRemoved.notify_one();

	//release the lock
	//mutexLock.unlock();
	uLock.unlock();

	return success;
}
//**********END CLASS DECLARATION********************


//Funciton for producer threads to add an item to the queue
void produceFunction(BBQ *queue, int item, int tp) {
	this_thread::sleep_for(chrono::seconds(tp));

	//anounce that the thread has been created
	printf("\nProducer thread created.\n");

	//variable to hold the current thread id
	thread::id threadID = this_thread::get_id();

	//produce item and put it in the queue
	if (queue->insert(item, threadID)) {
		//printf("\nItem ID %d produced by thread number: %d\n", item, threadID);
	}

	//this_thread::sleep_for(chrono::milliseconds(tp * 500));
}

//function for the consumer threads to remove an item from the queue
void consumeFunction(BBQ *queue, int item, int tc) {	
	this_thread::sleep_for(chrono::seconds(tc));

	//announce that the thread has been created
	printf("\nConsumer thread created.\n");

	//variable to hold the current thread id
	thread::id threadID = this_thread::get_id();

	//consume item from the queue
	if (queue->remove(&item, threadID)) {
		//printf("\nItem ID %d consumed by thread number: %d\n", item, threadID);
	};

	//this_thread::sleep_for(chrono::milliseconds(tc * 500));
}

int main(int argc, char *argv[]) {
	//convert the TP and TC values to Integers
	int tpRange = (*argv[1] - '0');
	int tcRange = (*argv[2] - '0');

	//create initial queue structure
	BBQ queue;

	

	//create sleeping interval
	srand(time(NULL));
	int TP = rand() % (tpRange + 1);
	int TC = rand() % (tcRange + 1);

	//generate item
	int genItem[MAX];
	for (int i = 0; i < MAX; i++)
	{
		//generate item
		genItem[i] = i + 20;
	}

	//create my threads
	thread producer[10];
	thread consumer[10];

	//create counter for item
	int itemCounter = 0;

	//program runs forever
	while (1)
	{		
		//start loop 
		for (int i = 0; i < 10; i++)
		{
			//reset itemCounter if itemCounter reaches MAX
			if(itemCounter == MAX){
				itemCounter = 0;
			}

			//reference producer thread
			producer[i] = thread (produceFunction, &queue, genItem[itemCounter], TP);

			//create a consumer thread if there are less than 10 threads
			consumer[i] = thread (consumeFunction, &queue, genItem[itemCounter], TC);

			//incrememnt itemCoutner
			itemCounter++;
		}
		
		//start other loop
		for (int i = 0; i < 10; i++)
		{
			producer[i].join();
			consumer[i].join();
		}
	}
}