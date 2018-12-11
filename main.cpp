#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>
#include<unistd.h>

using namespace std;

//make the MAX variable
const int MAX = 20;

//****************CLASS DECLARATION****************
class TSQueue {
	//create sync variables
	mutex mutexLock;

	//create actual variables
	int items[MAX];
	int front;
	int nextEmpty;

public:
	TSQueue();
	~TSQueue();
	bool tryInsert(int passItem);
	bool tryRemove(int *passItem);
};

TSQueue::TSQueue() {
	front = 0;
	nextEmpty = 0;
}

TSQueue::~TSQueue() {};

bool TSQueue::tryInsert(int passItem) {
	bool success = false;

	mutexLock.lock();
	if ((nextEmpty - front) < MAX) {
		items[nextEmpty % MAX] = passItem;
		nextEmpty++;
		success = true;
	}
	mutexLock.unlock();
	return success;
};

bool TSQueue::tryRemove(int *passItem) {
	bool success = false;

	mutexLock.lock();
	if (front < nextEmpty) {
		*passItem = items[front % MAX];
		front++;
		success = true;
	}
	mutexLock.unlock();

	return success;
};
//**********END CLASS DECLARATION********************


//Funciton for producer threads to add an item to the queue
void produceFunction(TSQueue *queue, int item, int tp) {
	this_thread::sleep_for(chrono::seconds(tp));

	//anounce that the thread has been created
	printf("\nProducer thread created.\n");

	//variable to hold the current thread id
	thread::id threadID = this_thread::get_id();

	//produce item and put it in the queue
	if (queue->tryInsert(item)) {
		printf("Item ID %d produced by thread number: %d\n", item, threadID);
	}

	//this_thread::sleep_for(chrono::milliseconds(tp * 500));
}

//function for the consumer threads to remove an item from the queue
void consumeFunction(TSQueue *queue, int item, int tc) {	
	this_thread::sleep_for(chrono::seconds(tc));

	
	//announce that the thread has been created
	printf("\nConsumer thread created.\n");

	//variable to hold the current thread id
	thread::id threadID = this_thread::get_id();

	//consume item from the queue
	if (queue->tryRemove(&item)) {
		printf("Item ID %d consumed by thread number: %d\n", item, threadID);
	};

	//this_thread::sleep_for(chrono::milliseconds(tc * 500));
}

int main(int argc, char *argv[]) {
	//convert the TP and TC values to Integers
	int tpRange = (*argv[1] - '0');
	int tcRange = (*argv[2] - '0');

	//create initial queue structure
	TSQueue queue;

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