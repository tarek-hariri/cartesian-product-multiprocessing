#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct pair_t{
    int first;
    int second;
}pair;

// Making these global makes it less obnoxious to pass in these tables to every function
int* leftTable;
int* rightTable;

int leftTableSize;
int rightTableSize;

// inclusivity: [lowerBound, upperBound)
// parameters specify the portion of outputTable that will be calculated by this function call
void cartesianProduct(int lowerBound, long upperBound, pair* outputTable){
	for(long i = lowerBound; i < upperBound; i++){
		pair newPair = {leftTable[i/rightTableSize],rightTable[i%rightTableSize]};
        	memcpy(outputTable+i, &newPair, 8);
	}
}

void printOutput(pair* outputTable){
    	for(long i = 0; i < leftTableSize * rightTableSize; i++){
        	printf("(%d, %d)\n", (outputTable+i)->first, (outputTable+i)->second);
    	}
}

void* create_shared_memory(size_t size){
	int protection = PROT_READ | PROT_WRITE;

	int visibility = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(){
    	leftTableSize = 50000;
    	rightTableSize = 100000;
    	long outputTableSize = leftTableSize * rightTableSize;

    	leftTable = (int*) calloc(leftTableSize, sizeof(int));
    	rightTable = (int*) calloc(rightTableSize, sizeof(int));

	for(int i = 0; i < leftTableSize; i++){
		*(leftTable+i) = i;
	}

	for(int i = 0; i < rightTableSize; i++){
		*(rightTable+i) = i;
	}

	pair* shmem = (pair*) create_shared_memory(8*outputTableSize);

	bool runParallel = true;
	clock_t begin = clock();

	// For performance comparison
	if(!runParallel){
		cartesianProduct(0, outputTableSize, shmem);
		
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("Elapsed (single process): %f seconds\n", time_spent);

		return 0;
	}

	// Fork twice and divy work up between the process
	// if outputTableSize/numProcesses does not  divide evenly, make the last process (parent) also take on the remainder of the work	
	int numProcesses = 3; // 2 children + 1 parent
	int pid;
	int wstatus;

	// child 1 process
	if((pid = fork())==0){
		cartesianProduct(0, 1*outputTableSize/numProcesses, shmem);
    		return 0;
	}
	// parent process
	else{	
		// child 2 process
		if(fork()==0){
			cartesianProduct(1*outputTableSize/numProcesses, 2*outputTableSize/numProcesses, shmem);
			return 0;
		}
		// parent
		else{
			cartesianProduct(2*outputTableSize/numProcesses, outputTableSize, shmem);

			waitpid(-1, &wstatus, 0);
		}

	}

	clock_t end = clock();

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Elapsed (multiprocess): %f seconds\n", time_spent);

	//printOutput(shmem);
    return 0;
}
