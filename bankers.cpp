// Nicholas A Moore
// Banker's Algorithm
// Operating Systems - Project II
// Dr. Guan
// 04/26/2023

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>


// Globals
const int NUMBER_OF_PROCESSES = 5; // n
const int NUMBER_OF_RESOURCES = 3; // m

// available -> vector of size m
// max -> n x m matrix
// allocation -> n x m matrix
// need -> n x m matrix
// need[i,j] = max[i,j] - allocation[i,j]

// my text file: first block if allocation
// ''  ''   ''   second block is max
// ''  ''   ''   third block is available


void printMatrix(int[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES]);
std::vector<int> getProcess(int, int[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES]);
bool rowIsLess(std::vector<int>, std::vector<int>);
void getNewAvailable(std::vector<int>&, std::vector<int>);

int main(int argc, char* argv[]) {
	// checking for file name
	if (argc < 2) {
		std::cout << "Error, include text file at execution." << std::endl;
		return -1;
	}

	std::ifstream input("input.txt"); 
	// checking that file was opened
	if (!input.is_open()) {
		std::cout << "Error opening selected file." << std::endl;
		return -1;
	}

	// our data structures, using a simple array for available
	int allocation[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES];
	int max[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES];
	int available[NUMBER_OF_PROCESSES];
	int need[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES];

	// Newly available will be held in vector
	std::vector<int> newAvailable;
	newAvailable.resize(NUMBER_OF_RESOURCES);

	// Using vector for safeSequence as we do not know the size, if it isn't "safe" it won't fill up
	std::vector<int> safeSequence;
	// queue for revisiting processes with failed evaluation
	std::queue<int> revisit;

	// extract data from file in appropriate matrix
	for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			input >> allocation[i][j];
		}
	}

	for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			input >> max[i][j];
		}
	}

	for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			need[i][j] = max[i][j] - allocation[i][j];
		}
	}

	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
		input >> available[i];

	
	// close the file
	input.close();


	// print extracted date
	std::cout << "Our current data is as follows:" << std::endl;
	std::cout << "Available Resouces:" << std::endl;
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
		std::cout << available[i] << ' ';
	std::cout << std::endl;

	std::cout << "Allocation Matrix:" << std::endl;
	printMatrix(allocation);

	std::cout << "Max Matrix:" << std::endl;
	printMatrix(max);

	std::cout << "Need Matrix (max[i,j] - allocation[i,j]):" << std::endl;
	printMatrix(need);

	std::cout << std::endl;

	std::cout << "Now we will compare our Need Matrix to our Available array." << std::endl
		<< "If our Available array is less than a row in our Need Matrix, we calculate" << std::endl
		<< "a New Available array. This has the value of our current Available Array" << std::endl
		<< "plus the corresponding row of the Allocation matrix." << std::endl << std::endl;

	// moving array to vector for funcs
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
		newAvailable[i] = available[i];

	int process = 0; // first process
	while (process < NUMBER_OF_PROCESSES) {
		// getProcess() returns corresponding row in vector form
		std::vector<int> currNeed = getProcess(process, need);

		std::cout << "P" << process << " of need: ";
		for (auto e : currNeed) 
			std::cout << e << ' ';
		std::cout << std::endl;

		std::cout << "Currently Available: ";
		for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
			std::cout << newAvailable[i] << ' ';
		std::cout << std::endl;

		std::cout << "Checking if Need <= Available: ";
		bool result = rowIsLess(currNeed, newAvailable);
		result == 0 ? std::cout << "False" : std::cout << "True"; // result will be a 0 or 1, conditional op for easy read
		std::cout << std::endl;

		if (result) { // we need to change newAvailable
			safeSequence.push_back(process); // this process order was safe, add it
			auto allocProcess = getProcess(process, allocation); // get same row from allocation matrix
			getNewAvailable(newAvailable, allocProcess); // calculate new available
			std::cout << "Currently Available is now: ";
			for (auto e : newAvailable)
				std::cout << e << ' ';
			std::cout << std::endl;
		}

		if (!result) // failed, add it to queue
			revisit.push(process);
		
		std::cout << std::endl;
		++process;
	}


	int trap = 0; // to ensure no infinite loops in (!result) if statement
	while (!revisit.empty() || trap > NUMBER_OF_PROCESSES) {

		int revisitProcess = revisit.front(); // get first failed process
		std::vector<int> currNeed = getProcess(revisitProcess, need);

		// print info
		std::cout << "P" << revisitProcess << " of need: ";
		for (auto e : currNeed)
			std::cout << e << ' ';
		std::cout << std::endl;

		std::cout << "Currently Available: ";
		for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
			std::cout << newAvailable[i] << ' ';
		std::cout << std::endl;

		// check again
		std::cout << "Checking if Need <= Available: ";
		bool result = rowIsLess(currNeed, newAvailable);
		result == 0 ? std::cout << "False" : std::cout << "True";
		std::cout << std::endl;

		if (result) { // passed this time
			safeSequence.push_back(revisit.front()); // add it to safe sequence
			auto allocProcess = getProcess(revisit.front(), allocation); // get corresponding row
			revisit.pop(); // remove it from queue
			getNewAvailable(newAvailable, allocProcess); // calculate new available
			std::cout << "Currently Available is now: ";
			for (auto e : newAvailable)
				std::cout << e << ' ';
			std::cout << std::endl;
		}

		if (!result) { // failed again
			int temp = revisit.front(); // temp is holder
			revisit.pop(); // remove it
			revisit.push(temp); // put it back on end
			++trap; // infinite loop prevention
		}
		std::cout << std::endl;

	}

	// if trap < NUMBER_OF_PROCESSES that means queue was empty() and there is a safe sequence
	if (trap < NUMBER_OF_PROCESSES) {
		std::cout << "The safe sequence is: ";
		for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) // print safe sequence
			std::cout << "P" << safeSequence[i] << " ";
		std::cout << std::endl;
	}
	else {
		std::cout << "There was no safe sequence";
	}

	return 0;
}


void printMatrix(int arr[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES]) {
	// simply prints passed matrix
	for (int i = 0; i < NUMBER_OF_PROCESSES; ++i) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			std::cout << arr[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}

std::vector<int> getProcess(int process, int matrix[NUMBER_OF_PROCESSES][NUMBER_OF_RESOURCES]) {
	std::vector<int> retVal;
	// returns desired row
	for (int i = process; i <= process; ++i) {
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			retVal.push_back(matrix[i][j]);
		}
	}
	return retVal;
}

bool rowIsLess(std::vector<int> currNeed, std::vector<int> avail) {
	// compares two rows
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i) {
		if (currNeed[i] > avail[i])
			return false;
	}
	return true;
}

void getNewAvailable(std::vector<int>& avail, std::vector<int> alloc) {
	// calculates new available
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)
		avail[i] += alloc[i];
}