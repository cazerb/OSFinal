#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define KEYSIZE 8
#define DATASIZE 56

typedef struct P{
	int threadNum;
	int size;
	int startIndex;
}threadParam;

typedef struct Record {
	char key[KEYSIZE];
	char data[DATASIZE];
};

void * threadFunc(void*);

int compareFunc();

int main()
{
	printf("Starting\n");
	//Initialize variables: var & init (precord where file mapped,pointer to thr arg,#rec,recsize,array of tid's)etc
	
	char * addr;
	int fd;
	struct stat sb; 

	//get number of cores
	//get number of threads to run off of, throw error if greater than cores
	int numThreads = 2;

	pthread_t threadID[numThreads];
	threadParam Params[numThreads];

	//Final test may be a string
	//May be read and write
	//O_RDONLY (Read only)
	//O_RDWR (May be Read and Write)
	fd = open("finaltest.txt", O_RDONLY)); 


	if (fd == -1)
	{
		printf("Error, File open failure\n");
		exit(EXIT_FAILURE);
	}

	if (fstat(fd, &sb) == -1)
	{
		printf("fstat error \n");
		exit(EXIT_FAILURE);
	}
	
	addr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED)
	{
		printf("mmap failed \n");
		exit(EXIT_FAILURE);
	}

	printf("File loaded\n");

	//Saves the pointer to the mapped file into an integer
	//may not be neccisary 

	for (int i = 0; i < numThreads; i++)
	{
		Params[i].size = sb.st_size / numThreads;
		Params[i].startIndex =  + (Params[i].size *i);
		Params[i].threadNum = i;
        if (pthread_create(&threadID[i], NULL, threadFunc, &Params[i]))
        {             
			printf("Thread create failed \n");
			exit(EXIT_FAILURE);
		}
	}

	printf("after creation of the threads\n");

	//join threads
	for (int i = 0; i < numThreads; i++)
	{
		if (pthread_join(threadID[i], NULL))
		{
			printf("Thread join failed \n");
			exit(EXIT_FAILURE);
		}
	}

	printf("After threads are complete:\n");

	if (write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size)
	{
		printf("Write to STDOUT FAILED \n");
		exit(EXIT_SUCCESS);
	}

	close(fd);
	exit(EXIT_SUCCESS);
}

void * threadFunc(void *param)
{
	printf("Starting threadFunc\n");
	threadParam *params = (threadParam *)param;

	//qsort causes segmentation fault, likely because of how it handles the data
	qsort(params->startIndex , KEYSIZE+DATASIZE, KEYSIZE, compareFunc);
	printf("Sort Complete\n");
}

//We need to figure out how to compare these two
int compareFunc(const void *a, const void *b)
{
	return (*(long*)a - *(long*)b);
}