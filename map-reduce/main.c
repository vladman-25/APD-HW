#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct myfile {
	char filename[20];
	int size;
	int ogidx;
};

struct thread_args {
	int id;
	int mappers;
	int reducers;
	int nrFiles;
	struct myfile* customfiles;
	int** indexes;
	int ***partialLists;
	int* fileIndex;
};

pthread_barrier_t barrier;
pthread_mutex_t mutex;

int get_file_length(char* fileName) {
	FILE *f = NULL;
	f = fopen(fileName,"rw");
    if(f == NULL)
	{
		printf("Nu exista fisierul de intrare '%s'\n",fileName);   
		exit(1);             
	}

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	fclose(f);
	return size;
}

void *thread_function_mappers(void *arg)
{
	struct thread_args* argstruct = (struct thread_args*)arg;
	int thread_id = argstruct->id;
	int nrFiles = argstruct->nrFiles;
	int reducers = argstruct->reducers;
	struct myfile* customfiles = argstruct->customfiles;
	int** indexes = argstruct->indexes;
	int ***partialLists = argstruct->partialLists;
	int* fileIndex = argstruct->fileIndex;

	int k = 0;
	while(*fileIndex < nrFiles) {
		///////////////
		pthread_mutex_lock(&mutex);
		int currFile = *fileIndex;
		(*fileIndex)++;
		pthread_mutex_unlock(&mutex);
		/////////////
		if(currFile >= nrFiles) {
			break;
		} 
		FILE *f = NULL;
		f = fopen(customfiles[currFile].filename,"rw");
		if(f == NULL)
		{
			printf("Nu exista fisierul de intrare '%s'\n",customfiles[currFile].filename);   
			exit(1);             
		}

		int nrmax;
		fscanf(f,"%d",&nrmax);
		int x = 0;
		

		int tempx;
		int count;
		int pow;
		int base;
		int increment;
		for(int p = 0; p < nrmax; p++) {
			
			fscanf(f,"%d",&x);				
			if(x == 1) {
				for(pow = 2; pow < reducers + 2; pow++) {
					partialLists[thread_id][pow][indexes[thread_id][pow]] = x;
					indexes[thread_id][pow]++;
				}
				continue;
			} else {
				base = 2;
				increment = 2;
				if(x % 2 == 1) {
					base = 3;
				}
				for(; base * base <= x; base += increment) {
		
					tempx = x;
					count = 0;
					while(tempx % base == 0 && count < reducers + 1) {
						tempx /= base;
						count++;
					}
					if (count == 1) {
						int tmpb = 0;
						int a = 2;
						for (; a * a <= base; a++){
							if (base % a == 0)
							{
								tmpb++;
								break;
							}
						} 
						if (tmpb == 0)
						{
							break;
						}		
					}
					if(tempx == 1)  {
						partialLists[thread_id][count][indexes[thread_id][count]] = x;
						indexes[thread_id][count]++;				
					}
				}
			}
		}
		fclose(f);
		k++;
	}
	pthread_barrier_wait(&barrier);
	pthread_exit(NULL);
}


int cmp(const void *a, const void *b) {
	int A = *(int*)a;
	int B = *(int*)b;
	return A - B;
}


void *thread_function_reducers(void *arg)
{
	struct thread_args* argstruct = (struct thread_args*)arg;
	int thread_id = argstruct->id;
	int mappers = argstruct->mappers;
	int ***partialLists = argstruct->partialLists;


	int assignedpow = thread_id - mappers + 2;
	pthread_barrier_wait(&barrier);


	char str[16];
	sprintf(str,"out%d.txt",assignedpow);

	FILE *out;
	out = fopen(str, "w");
	if(out == NULL)
    {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

	int *newList;
	newList	= calloc(1000,sizeof(int));
	int idxList = 0;
	for(int i = 0; i < mappers; i++)  {
		int k = 0;
		while(partialLists[i][assignedpow][k]!=0) {
			newList[idxList] = partialLists[i][assignedpow][k];
			idxList++;
			k++;
		}
	}

	qsort(newList, idxList, sizeof(int), cmp);

	int res = 0;
    for (int i = 0; i < idxList; i++) {

        while (i < idxList - 1 && newList[i] == newList[i + 1])
            i++;
 
        res++;
    }

	free(newList);
	fprintf(out,"%d",res);
	fclose(out);
	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
	if(argc < 4) {
		printf("Numar insuficient de parametri: ./tema1 <numar_mapperi> <numar_reduceri> <fisier_intrare>\n");
		exit(1);
	}
	char *infile;
	infile = calloc(16, sizeof(char));

	/* se citesc argumentele din linia de comanda */
	int mappers = atoi(argv[1]);
	int reducers = atoi(argv[2]);
	strcpy(infile, argv[3]);

	FILE *fptr = NULL;
	fptr = fopen(infile,"r");
    if(fptr == NULL)
	{
		printf("Nu exista fisierul de intrare '%s'\n",infile);   
		exit(1);             
	}

	/* se citesc argumentele din fisier */
	int nrFiles;
	fscanf(fptr, "%d",&nrFiles);
	char *auxString = calloc(20, sizeof(char));
	struct myfile* customfiles;
	customfiles = calloc(nrFiles, sizeof(struct myfile));
	for(int i = 0; i < nrFiles; i++) {
		fscanf(fptr,"%s", auxString);
		strcpy(customfiles[i].filename, auxString);
		customfiles[i].size = get_file_length(customfiles[i].filename);
		customfiles[i].ogidx = i;
	}
	/* se sorteaza fisierele */
	for (int i = 0; i < nrFiles - 1; i++) {
        for (int j = 0; j < nrFiles - i - 1; j++) {
            if (customfiles[j].size < customfiles[j+1].size) {
				int tsize;
				int tidx;
				char tname[20];

				strcpy(tname,customfiles[j].filename);
				tsize = customfiles[j].size;
				tidx = customfiles[j].ogidx;

				strcpy(customfiles[j].filename,customfiles[j+1].filename);
				customfiles[j].size = customfiles[j+1].size;
				customfiles[j].ogidx = customfiles[j+1].ogidx;

				strcpy(customfiles[j+1].filename, tname);
				customfiles[j+1].size = tsize;
				customfiles[j+1].ogidx = tidx;
			}
		}
	}
	fclose(fptr);

	/* se aloca memoria necesara */
	int** indexes;
	int ***partialLists;
	indexes = (int**)calloc(mappers,sizeof(int*));
    for (int i = 0; i < mappers; i++) {
        indexes[i] = (int*)calloc(reducers+2,sizeof(int));
	}

	partialLists = (int ***)malloc(mappers * sizeof(int**));
	for (int i = 0; i < mappers; i++) {
		partialLists[i] = (int **) malloc((reducers + 2) * sizeof(int *));
		for (int j = 0; j < reducers + 2; j++) {
			partialLists[i][j] = (int *)malloc(1000 * sizeof(int));
		}
	}

	/* initializam elementele de sincronizare */
	pthread_barrier_init(&barrier, NULL, mappers+reducers);
	pthread_mutex_init(&mutex, NULL);

	/* construim argumentele pentru thread-uri */
	pthread_t tid[mappers+reducers];
	int fileIndex = 0;
	struct thread_args* args;
	args = calloc(mappers + reducers, sizeof(struct thread_args));
	for (int i = 0; i < mappers+reducers; i++) {
		args[i].id = i;
		args[i].mappers = mappers;
		args[i].reducers = reducers;
		args[i].nrFiles = nrFiles;
		args[i].customfiles = customfiles;
		args[i].indexes = indexes;
		args[i].partialLists = partialLists;
		args[i].fileIndex = &fileIndex;
	}


	for (int i = 0; i < mappers+reducers; i++) {
		if(i < mappers) {
			pthread_create(&tid[i], NULL, thread_function_mappers, &args[i]);
		} else {
			pthread_create(&tid[i], NULL, thread_function_reducers, &args[i]);
		}
	}

	
	for (int i = 0; i < mappers+reducers; i++) {
		pthread_join(tid[i], NULL);
	}

	pthread_barrier_destroy(&barrier);
	pthread_mutex_destroy(&mutex);


	/* se elibereaza memoria */
    for (int i = 0; i < mappers; i++) {
        free(indexes[i]);
	}
	free(indexes);
	for (int i = 0; i < mappers; i++) {
		for (int j = 0; j < reducers + 2; j++) {
			free(partialLists[i][j]);
		}
		free(partialLists[i]);
	}
	free(partialLists);


	return 0;
}