#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define KEYSIZE 8
#define DATASIZE 55

typedef struct P{
        int threadNum;
        int size;
        void* startIndex;
        int recordNums;
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
        clock_t start_t, end_t;
        double total_t;

        //get number of cores
        //get number of threads to run off of, throw error if greater than cores
//      int numThreads = 16;

//      pthread_t threadID[numThreads];
//      threadParam Params[numThreads];

        //Final test may be a string
        //May be read and write
        //O_RDONLY (Read only)
        //O_RDWR (May be Read and Write)
for(int numThreads=2; numThreads<=32; numThreads*=2){

        fd = open("data_128", O_RDWR);


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

        int numRecords = sb.st_size / (DATASIZE + KEYSIZE);

        //printf("File loaded, number of records is: %d\n",);

        //Saves the pointer to the mapped file into an integer
        //may not be neccisary

//for(int numThreads=2; numThreads<=32; numThreads*=2){
pthread_t threadID[numThreads];
threadParam Params[numThreads];
//printf("%d\n",numThreads);
        printf("Starting Timer\n");
        start_t = clock();

        for (int i = 0; i < numThreads; i++)
        {
                Params[i].size = sb.st_size / numThreads;
                Params[i].startIndex =  addr + (Params[i].size *i);
                Params[i].threadNum = i;
                Params[i].recordNums = numRecords/numThreads;
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

        //calculates the time (and prints it for now)
        end_t = clock();
        total_t = (double)(end_t - start_t)/ CLOCKS_PER_SEC;
        printf("Total CPU time: %f sec.\n",total_t);

//      close(fd);
//      exit(EXIT_SUCCESS);
}//end thread for
int final_test = open("test.txt", O_WRONLY);
if (write(final_test, addr, sb.st_size) != sb.st_size)
        {
                printf("Write to STDOUT FAILED \n");
                exit(EXIT_SUCCESS);
        }


close(fd);
exit(EXIT_SUCCESS);

}//end main
void * threadFunc(void *param)
{
//      printf("Starting threadFunc\n");
        threadParam *params = (threadParam *)param;

        //qsort causes segmentation fault, likely because of how it handles the data
        qsort(params->startIndex , params->recordNums, KEYSIZE + DATASIZE, compareFunc);
//      printf("Sort Complete\n");

}

//We need to figure out how to compare these two
int compareFunc(const void *a, const void *b)
{
        return (strncmp((char*)a, (char*)b, KEYSIZE));
}
