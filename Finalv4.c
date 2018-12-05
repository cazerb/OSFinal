
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>

#define KEYSIZE 8
#define DATASIZE 55

pthread_cond_t startup = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int started = 0;
int toStart;
int numRecords;
pthread_t *threadID;
typedef struct P {
	int threadNum;
	int size;
	void* startIndex;
	int recordsInGroup;
}threadParam;

/*typedef struct Record {
		char key[KEYSIZE];
		char data[DATASIZE];
};*/

void * threadFunc(void*);

int compareFunc();

void merge();

int main()
{
	printf("Starting\n");
	//Initialize variables: var & init (precord where file mapped,pointer to thr arg,#rec,recsize,array of tid's)etc

	char * addr;
	int fd;
	struct stat sb;

	//get number of cores
	//get number of threads to run off of, throw error if greater than cores
	int numThreads = 32;
	toStart = numThreads;
	//pthread_t threadID[numThreads];
	threadParam Params[numThreads];
	threadID = calloc(numThreads, sizeof(pthread_t));
	//Final test may be a string
	//May be read and write
	//O_RDONLY (Read only)
	//O_RDWR (May be Read and Write)
	fd = open("finaltest.txt", O_RDWR);


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

	numRecords = sb.st_size / (DATASIZE + KEYSIZE);

	//printf("File loaded, number of records is: %d\n",);

	//Saves the pointer to the mapped file into an integer

	pthread_mutex_lock(&lock);
	for (int i = 0; i < numThreads; i++)
	{
		Params[i].size = sb.st_size / numThreads;
		Params[i].startIndex = addr + (Params[i].size *i);
		Params[i].threadNum = i;
		Params[i].recordsInGroup = numRecords / numThreads;
		if (pthread_create(&threadID[i], NULL, threadFunc, &Params[i]))
		{
			printf("Thread create failed \n");
			exit(EXIT_FAILURE);
		}
		started++;
		//printf("starded = %d",started);
	}

	//printf("after creation of the threads\n");
	pthread_cond_signal(&startup);
	pthread_mutex_unlock(&lock);

	//join thread
	if (pthread_join(threadID[0], NULL))
	{
		printf("Thread join failed \n");
		exit(EXIT_FAILURE);
	}

	//printf("After threads are complete:\n");

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
	//printf("Starting threadFunc\n");
	pthread_mutex_lock(&lock);
	while (started < toStart) {
		pthread_cond_wait(&startup, &lock);
	}
	threadParam *params = (threadParam *)param;
	int tToi = 1;
	int recordsInGroup = params->recordsInGroup;
	int groupSize = 2;
	//qsort causes segmentation fault, likely because of how it handles the data
	qsort(params->startIndex, recordsInGroup, KEYSIZE + DATASIZE, compareFunc);
	//printf("Sort Complete\n");
	//printf("%d done\n",params->threadNum);
	pthread_mutex_unlock(&lock);
	while (params->threadNum % groupSize == 0 && recordsInGroup < numRecords)
	{
		if (pthread_join(threadID[params->threadNum + tToi], NULL))
		{
			printf("Thread join failed \n");
			exit(EXIT_FAILURE);
		}
		//printf("joining %d and %d\n", params->threadNum, params->threadNum+tToi);
		merge(params, recordsInGroup);
		tToi *= 2;
		recordsInGroup *= 2;
		groupSize *= 2;
	}
}

//We need to figure out how to compare these two
int compareFunc(const void *a, const void *b)
{
	return (strncmp((char*)a, (char*)b, KEYSIZE));
}

void merge(void* param, int numOfRecs)
{
	threadParam *params = (threadParam *)param;
	int sizeOfData = numOfRecs * (KEYSIZE + DATASIZE) * 2;
	char * temp = malloc(sizeOfData);
	//printf("sizeofdata: %d\n",numOfRecs);
	int adis = 0;
	int bdis = 0;
	void* indexA = params->startIndex;
	void* indexB = indexA + (numOfRecs * (KEYSIZE + DATASIZE));
	int displacement = 0;
	for (int i = 0; i < (numOfRecs * 2); i++)
	{
		if (compareFunc(indexB, indexA) > 0 && adis < numOfRecs)
		{
			memcpy(temp + displacement, indexA, 63);
			indexA += 63;
			adis++;
		}
		else if (compareFunc(indexA, indexB) > 0 && bdis < numOfRecs)
		{
			memcpy(temp + displacement, indexB, 63);
			indexB += 63;
			bdis++;
		}
		else if (adis == numOfRecs && bdis < numOfRecs)
		{
			memcpy(temp + displacement, indexB, 63);
			indexB += 63;
			bdis++;
		}
		else
		{
			memcpy(temp + displacement, indexA, 63);
			indexA += 63;
			adis++;
		}
		displacement += 63;
	}
	//printf("%d displacement",displacement);
	//printf("a %d b %d",adis, bdis);
	memcpy(params->startIndex, temp, sizeOfData);
	/*printf("for: %d\n",params->threadNum);
	if (write(STDOUT_FILENO, temp, displacement) != displacement)
	{
			printf("Write to STDOUT FAILED \n");
			exit(EXIT_SUCCESS);
	}*/
}
