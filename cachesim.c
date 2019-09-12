#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cachesim.h"

int lg (int x){//utility log2 function
int i;
for (i =0;i<x;i++){
	if ((int)(pow(2,i)) == x){
		break;
	}

}
if (i==x)
	return -1;
return i;
}


int getNumLines(FILE* fp){//Function to find number lines in input text file
//FILE* trace;
  //      trace = fopen(argv[1], "r");
        int numLines = 0;
        char buff[200];
        char *p = buff;
        while (1){
                fgets(p,200,fp);
                if (feof(fp)){
                        break;
                }


                numLines++;
        }
        numLines--;
	return numLines;

}

//Make sure to include function for ensuring inputs are powers of 2

int stopAtColonIndex(char *cp){
	int i;
	for (i=0;i<strlen(cp);i++){
		if (cp[i] == ':'){
			break;
		}
	}
	return i;

}
int inCache(cacheLine**m,int setBits,int tagBits, int linesPerSet){
	int k;
	int returnVal = 0;
	for (k=0; k<linesPerSet;k++){
		if (tagBits == m[setBits][k].tagBits){
			returnVal = 1;
			break;
		}

	}


return returnVal;
}

int main(int argc, char**argv){
	//Code to feed input file pointer to getNumLines function
	FILE* preTrace;
	preTrace = fopen(argv[5],"r");
	int numLines = getNumLines(preTrace);

	//printf("%d\n",numLines);
	
	int i,j;

	int cacheHits = 0;
	int cacheMisses = 0;
	int memoryReads = 0;
	int memoryWrites = 0;

	int cacheHits2 = 0;
	int cacheMisses2 = 0;
	int memoryReads2 = 0;
	int memoryWrites2 = 0;
/*******************************************************/
	
	int numCacheLines = (atoi(argv[1]))/(atoi(argv[4]));
	char* assocType = argv[2];
	int linesPerSet = 0;
	int numSets = 0;
	if (strcmp(assocType,"direct") == 0){
		numSets = numCacheLines;
		linesPerSet = 1;
	//	printf("%d %d\n",numSets,linesPerSet);
	}
	else if (strcmp(assocType,"assoc")==0){
		numSets = 1;
		linesPerSet = numCacheLines;
	//	printf("%d %d\n",numSets,linesPerSet);
	}
	else {
		int n = assocType[6] - '0';//n for n-way associative
		linesPerSet = n;
		numSets = numCacheLines/linesPerSet;
	//	printf("%d %d\n",numSets,linesPerSet);
	}
//	printf("\n");
//	printf("\n");

//	cacheLine cache1[numSets][linesPerSet];//cache without prefetch
//STATIC STACK-BASED CACHE ABOVE


	cacheLine**cache1;
	cache1 = (cacheLine**)malloc(sizeof(cacheLine*)*numSets);

	for (i=0; i<numSets;i++){
		cache1[i] = (cacheLine*)malloc(sizeof(cacheLine)*linesPerSet);

	}
	for (i=0; i<numSets;i++){
		for (j=0; j<linesPerSet;j++){
		//	cache1[i][j].validBit = 1;
			cache1[i][j].tagBits = -1;
		//	printf("inRow ");
		}
		//printf("%d\n",cache1[0][0].validBit);
	}	

	//cacheLine cache2[numSets][linesPerSet];//cache with prefetch

	cacheLine**cache2;
	cache2 = (cacheLine**)malloc(sizeof(cacheLine*)*numSets);

	for (j=0; j<numSets; j++){
		cache2[j] = (cacheLine*)malloc(sizeof(cacheLine)*linesPerSet);

	}

	for (i=0; i<numSets; i++){
		for (j=0; j<linesPerSet;j++){
			cache2[i][j].tagBits = -1;
		}


	}

	int fifoIndices[numSets];//tracker of FIFO for cache1
	int fifoIndices2[numSets];//tracker of FIFO for cache2
	for (j=0;j<numSets;j++){
		fifoIndices[j] = 0;
		fifoIndices2[j] = 0;

	}

//	char buff2[200];
	char*ptr = (char*)malloc(sizeof(char)*100000000);
//	char*ptr = buff2;
	FILE* trace = fopen(argv[5],"r");
	for (i=0; i<numLines;i++){
		fgets(ptr,200,trace);
		int val = stopAtColonIndex(ptr);
		val+=2;
		ptr+=val;
		char operation = ptr[0];//To decide if we need to read to write
		
		ptr+=2;//Now ptr is pointing to the base of just the address that we wish to manipulate
		char* address = ptr;
		char* dummy = address;
		dummy+=2;
		int totalBits =(strlen(dummy)-1)*4;
	//	printf("%d ",totalBits);	
		int b = lg(atoi(argv[4]));
		int s = lg(numSets);
		int t = totalBits-b-s;
	//	printf("%d %d %d  ",b,s,t);	
	
	//	printf("%s",dummy);

		int addressVal = strtol(dummy,NULL,16);
		int copy1 = addressVal;//will be used to identify set bits
		int copy2 = addressVal;

		int addressVal2 = addressVal + (atoi(argv[4]));
		int copy3 = addressVal2;
		int copy4 = addressVal2;
	
		copy1 = copy1 >> b;
		copy2 = copy2 >> (b+s);

		copy3 = copy3 >> b;
		copy4 = copy4 >> (b+s);
		
		
		int mask = numSets-1;
	
		int mask2 = (int)(pow(2,t))-1;

		
		copy1 = copy1&mask;//set identifying bits for address A
		copy3 = copy3&mask;//set identifying bits for address B
//		printf("the set mapped to is: %d\n",copy1);

		copy2 = copy2&mask2;//tag identifying bits for address A
		copy4 = copy4&mask2;//tag identifying bits for address B
//		printf("****The Tag for the address is: %d\n",copy2);


/***********************************************************************************************/

//The code below deals with actuall traversing the cache, updating it, and incrementing statistics

//		for (j=0; j<linesPerSet;j++){
			if (operation == 'R'){
				if (inCache(cache1,copy1,copy2,linesPerSet)==1){
					cacheHits++;
				//	printf("hit\n");
				}
				else {
					cacheMisses++;
					memoryReads++;
				//	cache1[copy1][fifoIndices[copy1]] = (cacheLine)malloc(sizeof(cacheLine));
			//		printf("anaconda\n");
					cache1[copy1][fifoIndices[copy1]].tagBits = copy2;
	
					fifoIndices[copy1] = (fifoIndices[copy1]+1)%(linesPerSet);
				//	printf("%d\n",fifoIndices[copy1]);
	//			printf("cheddar\n");
				}

				if (inCache(cache2,copy1,copy2,linesPerSet)==1){
					cacheHits2++;

				}
				else {
					cacheMisses2++;
					memoryReads2++;
					cache2[copy1][fifoIndices2[copy1]].tagBits = copy2;
					fifoIndices2[copy1] = (fifoIndices2[copy1]+1)%(linesPerSet);
					//A has been loaded, now B is needed
					if (inCache(cache2,copy3,copy4,linesPerSet) == 0){
						memoryReads2++;
						cache2[copy3][fifoIndices2[copy3]].tagBits = copy4;
						fifoIndices2[copy3] = (fifoIndices2[copy3]+1)%(linesPerSet);
					}

				}
			}
			else if (operation == 'W'){
				 if (inCache(cache1,copy1,copy2,linesPerSet)==1){
                                         cacheHits++;
					 memoryWrites++;
 
                                 }		
				 else {
                                         cacheMisses++;
                                         memoryReads++;
                                  //       cache1[copy1][fifoIndices[copy1]] = (cacheLine)malloc(sizeof(cacheLine));
                                         cache1[copy1][fifoIndices[copy1]].tagBits = copy2;
                                         fifoIndices[copy1] = (fifoIndices[copy1]+1)%(linesPerSet);
					 memoryWrites++;
 
                               }

				if (inCache(cache2,copy1,copy2,linesPerSet) == 1){
					cacheHits2++;
					memoryWrites2++;

				}
				else {
					cacheMisses2++;
					memoryReads2++;
					cache2[copy1][fifoIndices2[copy1]].tagBits = copy2;
					fifoIndices2[copy1] = (fifoIndices2[copy1]+1)%(linesPerSet);
					if (inCache(cache2,copy3,copy4,linesPerSet) == 0){
						memoryReads2++;
						cache2[copy3][fifoIndices2[copy3]].tagBits = copy4;
						fifoIndices2[copy3] = (fifoIndices2[copy3]+1)%(linesPerSet);

					}
					memoryWrites2++;

				}
			}
		
//		}
	//	ptr = strdup(buff2);
/*
		printf("\n");
		printf("\n");
		printf("Cache hits: %d\n",cacheHits);
		printf("Cache misses: %d\n",cacheMisses);
		printf("Memory reads: %d\n",memoryReads);
		printf("Memory writes: %d\n",memoryWrites);	
*/
	}

		    printf("no-prefetch\n");
                    printf("Cache hits: %d\n",cacheHits);
                    printf("Cache misses: %d\n",cacheMisses);
                    printf("Memory reads: %d\n",memoryReads);
                    printf("Memory writes: %d\n",memoryWrites);

		    printf("with-prefetch\n");
                    printf("Cache hits: %d\n",cacheHits2);
                    printf("Cache misses: %d\n",cacheMisses2);
                    printf("Memory reads: %d\n",memoryReads2);
                    printf("Memory writes: %d\n",memoryWrites2);





	return 0;
}
