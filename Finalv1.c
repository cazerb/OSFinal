#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys / mman.h>
#include <sys / stat.h>
#include <sys / fcntl.h>

#define KEYSIZE 8;
#define DATASIZE 56;

typedef struct P*{
	int threadNum;
	int size;
	int startIndex;
}ThreadParam;

void * threadFunc(void*);

int compareFunc();

int main()
{
	//Initialize variables: var & init (precord where file mapped,pointer to thr arg,#rec,recsize,array of tid's)etc
	
	char * addr;
	int fd;
	struct stat sb; 

	
	//get number of cores
	//get number of threads to run off of, throw error if greater than cores
	numThreads = 2;

	threadParam Params[numThreads];
	//Final test may be a string
	//May be read and write
	//O_RDONLY (Read only)
	fd = open("finaltest.txt", PROT_WRITE); 

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
	
	for (int i = 0; i < numThreads; i++)
	{
		Params[i].size = sb.st_size / numThreads;
		Params[i].startIndex = addr + (Params[i].size *i);
		Params[i].ThreadNum = i;
        if (pthread_create(&threadID[i], NULL, threadFunc, &Params[i]))
        {             
			printf("Thread create failed \n");
			exit(EXIT_FAILURE);
		}
	}
}

void * threadFunc(void *param)
{
	threadParam *params = (threadParam *)param;

	qsort(params->startIndex , KEYSIZE+DATASIZE, KEYSIZE, compareFunc));

}

int compareFunc(const void *a, const void *b)
{
	return (*(int*)a - *(int*)b);
}