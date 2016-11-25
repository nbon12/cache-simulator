#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //to parse command line.
#include <getopt.h>
#include <errno.h>
struct Cache {
	int S;			//numbers of sets
	int s; 			// # of set index bits
    int E; 			// Associativity
    int B; 			// size of block
    int b; 			// #of block bits
    unsigned long **tags;
};
int movedown(struct Cache ccc, int set);
void allocCache(struct Cache ccc, int numSets, int linesPerSet);
void printCache(struct Cache ccc);
unsigned long gettag(unsigned long nexti, struct Cache ccc);
unsigned long getBits(unsigned low, unsigned high, unsigned long source);
void reorder(struct Cache ccc, unsigned long set, unsigned long tag, int * miss_count, int * eviction_count, int * hit_count, int * verbose);
int inst_s(struct Cache ccc, unsigned long nexti);
int inst_b(struct Cache ccc, unsigned long nexti);
int verboseFlag = 0;
int main(int argc, char *argv[])
{
	struct Cache ccc;
    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
	char filename[256];

    int s = 4;
    int sets = 16;
	int linesPerSet = 1;
	int blockbits = 4;
	int blocksize = 2 << (blockbits-1);

	ccc.S = sets;
	ccc.s = s;
	ccc.E = linesPerSet;
	ccc.B = blocksize;
	ccc.b = blockbits;
	int aflag = 0;
	int c;
	int verbose = 0;
	opterr = 0;
	while ((c = getopt (argc, argv, "s:E:ab:c:t:v")) != -1)
    switch (c)
    {
      case 'a':
        aflag = 1;
        break;
      case 'b':
		ccc.b = atoi(optarg);
		ccc.B = 2 << (ccc.b-1);
        break;
      case 'c':
        break;
	  case 's':
		ccc.s = atoi(optarg);
		ccc.S = 2<<(ccc.s-1);
        break;
	  case 'E':
		ccc.E = atoi(optarg);
        break;
	  case 't':
		strncpy(filename, optarg, strlen(optarg));
		filename[strlen(optarg)] = '\0';
        break;
	  case'v':
		verboseFlag = 1;
		verbose = 1;
	  break;
      case '?':
        if (optopt == 'b')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
    }
    //--------------------------------------------------------------------
    //S, s, E and B have been set, so now malloc, the cache.
    ccc.tags = (unsigned long **)malloc(ccc.S*sizeof(unsigned long *));
	for(int i = 0; i < ccc.S; i++)
	{
		//printf("malloc tags[%d] with ccc.E*sizeof(unsigned long)=%d", i, ccc.E*sizeof(unsigned long));
		ccc.tags[i] = (unsigned long *)malloc(ccc.E * sizeof(unsigned long));
		if (ccc.tags[i] == NULL)
		{
			printf("Unsuccessful malloc");
			exit(EXIT_FAILURE);
		}
	}
	//Initialize all lines in cache to -1
	for(int i = 0; i < ccc.S; i++)
	{
		for(int j = 0; j < ccc.E; j++){
				unsigned long storeme = -1;
				printf("[%d][%d]=-1\n", i, j);
				ccc.tags[i][j] = storeme;
		}
	}
	
	FILE *fp;
	fp = NULL;
	fp = fopen(filename, "r");
	if(!fp)
	{
		printf("\nFailed to open %s, %s\n", filename, strerror(errno));
		exit(EXIT_FAILURE);
	}
	char line[256];
	while(fgets(line, sizeof(line), fp))
	{
		char instruction[256];
		int runs = 0;
		switch(line[1])
		{
			case ' ':
				//ignore 'I' with whitespace after.
				runs = 0;
				break;
			case 'M':
				//run this instruction twice.
				runs = 2;
				break;
			case 'L':
			case 'S':
				//run this instruction once.
				runs = 1;
				break;
			default:
				runs = 0;
		}
		int i = 3;
		for(; line[i] != ','; i++ )
		{
			instruction[i-3] = line[i];
		}
		//add null terminator at end of instruction string:
		instruction[i] = '\0'; 
		int sizeOfThisInst = 0;
		//determine size, in bytes, of the instruction:
		if((strlen(instruction) & 0x1) == 1)
		{//odd
			sizeOfThisInst = (strlen(instruction)+1)/2;	
		}
		else
		{//even
			sizeOfThisInst = strlen(instruction)/2;
		}
		unsigned long inst = strtoul(instruction, NULL, 16);
		if(verboseFlag){printf("%c %lx,%d ", line[1], inst, sizeOfThisInst);}
		//run the instruction:
		while(runs > 0)
		{
			unsigned long nexti = inst;
    
			unsigned long next_tag = gettag(nexti, ccc);
			unsigned long next_set = inst_s(ccc, nexti);
			reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count, &verbose);
			runs--;
		}
		if(verboseFlag){printf("\n");}
		//clean the instruction string.
		memset(instruction, 0, sizeof(instruction));
	}
	//close file
	if(fclose(fp))
	{
		printf("failed to close %s", filename);
	}
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
/* --------------------------------------------------------------------------------
 * function - reorder
		this function searches for the instruction with the corresponsind tag and set
		and determines whether or not it was a hit or miss.
 * @param ccc is the cache pointer
 * @param set is the set number
 * @param tag is the tag of the instruction to search for
 * @param hit, miss, eviction count are pointers to the counters.
 * @param verbose is unused.
 --------------------------------------------------------------------------------*/
void reorder(struct Cache ccc, unsigned long set, unsigned long tag, int * hit_count, int * miss_count, int * eviction_count, int * verbose)
{
	if(set > (ccc.S-1))
	{
		printf("\nreorder set out of bounds, with %d sets, you can access 0-%d, not %lu\nExiting...\n", ccc.S, ccc.S-1, set);
		exit(EXIT_FAILURE);
	}
	int lineOfTag = -1;
	//iterate through set to find the tag index
	for(int i = 0; i < ccc.E; i++)
	{
		if(ccc.tags[set][i] == tag)//if this line is our tag then its a hit
		{
			lineOfTag = i;			//remember line index.
			(*hit_count)++;
			if(verboseFlag){printf("hit");}
		}
	}
	if(lineOfTag != -1)
	{
		//if it was a hit, reorder so that MOST RECENTLY USED is at tags[set][0]:
		for(int i = lineOfTag; i > 0; i--)
		{
			ccc.tags[set][i] = ccc.tags[set][i-1];
		}
		ccc.tags[set][0] = tag;
	}
	else //it was a miss:
	{
		(*miss_count)++;
		if(verboseFlag){printf("miss ");}
		(*eviction_count) = (*eviction_count) + movedown(ccc, set);
		ccc.tags[set][0] = tag;
		//insert it into cache.
	}
}
/* --------------------------------------------------------------------------------
 * function - movedown
 *		this function moves all entries in a set down 1 entry in the array and evicts
		the last entry if necessary, returns 1 if eviction occurred else returns 0.
 * @param ccc is the cache pointer
 * @param set is the set number
 * @return 1 if eviction occurred, else returns 0
 --------------------------------------------------------------------------------*/
int movedown(struct Cache ccc, int set)
{
	if(set > (ccc.S-1))
	{
		printf("\nmovedown set out of bounds, with %d sets, you can access 0-%d, not %d\nExiting...\n", ccc.S, ccc.S-1, set);
		exit(EXIT_FAILURE);
	}
	int eviction = 0;
	if(ccc.tags[set][ccc.E-1] != -1)
	{
		eviction = 1;
		if(verboseFlag){printf("eviction ");}
	}
	//evict the last guy.
	//start from the last position in the array and pull everything over
	for(int j = ccc.E-1; j > 0; j--)
	{
		ccc.tags[set][j] = ccc.tags[set][j-1];
	}
	return eviction;
}

/* --------------------------------------------------------------------------------
 * function - gettag
 *		given an instruction and a cache, returns the tag of the instruction
 * @param ccc is the cache pointer
 * @param nexti is the instruction to parse
 * @return t, the tag of the instruction
 --------------------------------------------------------------------------------*/
unsigned long gettag(unsigned long nexti, struct Cache ccc)
{
	//count a b + s and deliver everything after that.
	int index = ccc.s + ccc.b;	
	unsigned long t = getBits(index, (sizeof(unsigned long)*8)-1, nexti);
	return t;
}
/* --------------------------------------------------------------------------------
 * function - inst_b
 *		given an instruction and a cache, returns the byte offset of the instruction
 * @param ccc is the cache pointer
 * @param nexti is the instruction to parse
 * @return the byte offset of the instruction
 --------------------------------------------------------------------------------*/
int inst_b(struct Cache ccc, unsigned long nexti)
{
	return getBits(0, ccc.b-1, nexti);
}
/* --------------------------------------------------------------------------------
 * function - inst_s
 *		given an instruction and a cache, returns the set of the instruction
 * @param ccc is the cache pointer
 * @param nexti is the instruction to parse
 * @return the set of the instruction
 --------------------------------------------------------------------------------*/
int inst_s(struct Cache ccc, unsigned long nexti)
{
	return getBits(ccc.b, ccc.b+ccc.s-1, nexti);
}
/* --------------------------------------------------------------------------------
 * function - printCache
 *		prints the state of the cache
 * @param ccc is the cache pointer
 --------------------------------------------------------------------------------*/
void printCache(struct Cache ccc)
{
	printf("\n------ printing cache -----\n");
	printf("ccc.s = %d", ccc.s);
	for(int i = 0; i < ccc.S; i++){
      	printf("\nset %d[", i);
		for(int j = 0; j < ccc.E; j++){
				printf("line %d: %lx, ", j, (unsigned long)ccc.tags[i][j]);
        }
		printf("]");
    }
    printf("\n------ end printing cache -----\n");
}
/*-----------------------------------------------------------------------------------------------
    getBits from Tools.cpp
	Extracts bits 'low' to 'high' (inclusive) from 'source' and returns the result as
	an 64-bit integer. 
	Example:
	
	getBits(12,15,0x0123456789abcdef);
	
	would return the value:
	
	0x00000000000000c;
	
	(Bits 12-15 of the 64-bit word, counting from the right correspond to hex digit 'c' in the
	source word. Note that any number of bits from 1 to 64 could be specified depending on the values
	of 'low' and 'high'.
	The value of high must be greater than or equal to low and high must be in the range 0 to 63. 
------------------------------------------------------------------------------------------------*/
unsigned long getBits(unsigned low, unsigned high, unsigned long source)
{
    unsigned long populationOfRegion = high - (low - 1);
    unsigned long unmaskedResult = source >> low; 
    unsigned long mask = -1;
    mask = mask << (populationOfRegion - 1);
    mask = mask << 1;
    mask = ~mask;
    unsigned long result = unmaskedResult & mask;
    return result;
}
		
		
		