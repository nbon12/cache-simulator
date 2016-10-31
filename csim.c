#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //to parse command line.
#include <ctype.h>

struct Cache {
	int S;			//numbers of sets
	int s; 			// # of set index bits
    int E; 			// Associativity
    int B; 			// size of block
    int b; 			// #of block bits
    unsigned long **tags;
};
int movedown(struct Cache ccc, int set);
void allocCache(struct Cache ccc, int numSets, int linesPerSet);//TODO: will allocate the tags array in this cache.
void printCache(struct Cache ccc);
unsigned long gettag(unsigned long nexti, struct Cache ccc);
unsigned long getBits(unsigned low, unsigned high, unsigned long source);
void reorder(struct Cache ccc, unsigned long set, unsigned long tag, int * miss_count, int * eviction_count, int * hit_count);
int inst_s(struct Cache ccc, unsigned long nexti);
int inst_b(struct Cache ccc, unsigned long nexti);
int main(int argc, char *argv[])
{
    													//unsigned long hello = 5;
    													//hello = sizeof(unsigned long);
    													//printf("HELLO: %lu", hello);
    struct Cache ccc;
    unsigned long **tags;
    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
    //GET S, s, E, B, and B in between here...----------------------------
    //ccc.S = S(command line)
    //ccc.s = s(command line)
    //ccc.E = E(command line)
    //ccc.B = B(command line)
    //ccc.b = b(command line)
    //Temporary:
    int s = 2;
    int sets = 4;//set this from the args array
	int linesPerSet = 2;
	int blockbits = 4; //2>>blockbits = blocksize;
	int blocksize = 2 << (blockbits-1);
	// End Temporary
	ccc.S = sets;
	ccc.s = s;
	ccc.E = linesPerSet;
	ccc.B = blocksize;
	ccc.b = blockbits;
	//and here...
	int bool = 0;
	/*
  	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-s")){
			ccc.s = atoi(argv[i+1]);
			//ccc.S = 2 << (ccc.s-1); //
		} else if (strcmp(argv[i], "-E") == 0) {
			//ccc.E = atoi(argv[i+1]);
		} else if (strcmp(argv[i], "-b") == 0) {
			//ccc.b = atoi(argv[i+1]);
			//ccc.B = 2 << (ccc.b-1);
		} else if (strcmp(argv[i], "-t") == 0) {
			//bool = 1;
		}
	}
	*/
	
    //--------------------------------------------------------------------
    //okay now assuming we have S, s, E and B, lets construct,alloc, the cache.
    ccc.tags = (unsigned long **)malloc(sets*sizeof(unsigned long *));
	for(int i = 0; i < sets; i++)
	{
		//tags[i] = (unsigned long *)malloc(linesPerSet * sizeof(unsigned long));
		ccc.tags[i] = (unsigned long *)malloc(linesPerSet * sizeof(unsigned long));
		//unsigned long happy2 = -1;
		//tags[i][0+1] = happy2;
		//printf("SNAPPY: %lu", tags[i][0+1]);
	}
    for(int i = 0; i < sets; i++){
      	//printf("\nset %d[", i);
		for(int j = 0; j < linesPerSet; j++){
      			unsigned long storeme = -1;
      			ccc.tags[i][j] = storeme;
				//printf("line %d: %lu, ", j, tags[i][j]);
        }
		//printf("]");
    }
    //unsigned long nextInstruction = getNextInstruction();
    unsigned long nexti = 0x10;
    //unsigned long nexti = 0xff;
    
    
	//printf("The tag of %x is %lu where s=%d b=%d and ", nexti, next_tag, ccc.s, ccc.b);
    //unsigned long next_set = inst_s(ccc, nexti);
    
// printf("\nnext_set=%lu", next_set);
    //while(//there is another instruction)
    //{
    	unsigned long next_tag = gettag(nexti, ccc);
    	unsigned long next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    //}
    //----tests
    	nexti=0x20;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
		nexti=0x20;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
		
		
		nexti=0x22;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    
    	nexti=0x18;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    
    	nexti=0x110;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    	
    	nexti=0x210;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    
    	nexti=0x12;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    	nexti=0x12;
    	next_tag = gettag(nexti, ccc);
    	next_set = inst_s(ccc, nexti);
		reorder(ccc, next_set, next_tag, &hit_count, &miss_count, &eviction_count);
    
    	
    //---endtests
    //unsigned long tag = gettag(nexti);
    //ccc.tags = tags;
    //printf("Ttags[0][0] = %d\n", ttags[0][0]);
    //ccc.S = 4;
    //eviction_count += movedown(ccc, 3);
    printCache(ccc);
    //movedown(ccc, 1);
    //printSummary(0, 0, 0);
    //int sets = 4;//set this from the args array
	//int linesPerSet = 2;
	//int blockbits = 4; //2>>blockbits = blocksize;
	//int blocksize = 2 << (blockbits-1);
    printSummary(hit_count, miss_count, eviction_count);
    printf("\nnext_tag=%lu", next_tag);
    printf("\nnext_set=%lu", next_set);
    return 0;
}
void reorder(struct Cache ccc, unsigned long set, unsigned long tag, int * hit_count, int * miss_count, int * eviction_count)
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
			printf("HIT. LINE:%d, THE TAG THAT WAS HERE WAS %lu", i, ccc.tags[set][i]);
			lineOfTag = i;			//remember line index.
			(*hit_count)++;
		}
	}
	if(lineOfTag != -1)//if it was a hit, reorder so that MOST RECENTLY USED is at tags[set][0]...
	{
		for(int i = lineOfTag; i > 0; i--)
		{
			ccc.tags[set][i] = ccc.tags[set][i-1];
		}
		ccc.tags[set][0] = tag;
	}
	else //it was a miss...
	{
		
		(*miss_count)++;
		(*eviction_count) = (*eviction_count) + movedown(ccc, set);
		ccc.tags[set][0] = tag;
		//insert it into cache.
	}
}
int movedown(struct Cache ccc, int set)
{
	if(set > (ccc.S-1))
	{
		printf("\nmovedown set out of bounds, with %d sets, you can access 0-%d, not %d\nExiting...\n", ccc.S, ccc.S-1, set);
		exit(EXIT_FAILURE);
	}
	int eviction = 0;
	int linesPerSet = ccc.E;
	unsigned long **tags = ccc.tags;
	//int starter = 0;
	//int temp0 = tags[set][0];
	unsigned long writeMe = 0;
	unsigned long temp = tags[set][0];
	unsigned long temp2 = 0;
	//check for eviction first...
	printf("ccc.tags[set][ccc.E-1]=%lu", ccc.tags[set][ccc.E-1]);
	if(ccc.tags[set][ccc.E-1] != -1)
	{
		eviction = 1;
		//printf("E-E-EVICTION");
	}
	tags[set][0] = writeMe;
	for(int j = 1; j < linesPerSet-1; j++)
	{
		temp2 = tags[set][j];
		if(j == 1)
		{
			tags[set][j] = temp;
		}
		else
		{
			tags[set][j] = temp2;
		}
	}
	return eviction;
}
//given a struct and ccc, returns the tag.
unsigned long gettag(unsigned long nexti, struct Cache ccc)
{
	//todo, count a b + s and deliver everything after that.
	int index = ccc.s + ccc.b;
	//printf("\nINNNDDDEEEXXXXX:, %d\n", index);
	unsigned long t = getBits(index, (sizeof(unsigned long)*8)-1, nexti);
	//printf("THE TAG IS %lu", t);
	return t;
}
int inst_b(struct Cache ccc, unsigned long nexti)
{
	return getBits(0, ccc.b-1, nexti);
}
int inst_s(struct Cache ccc, unsigned long nexti)
{
	return getBits(ccc.b, ccc.b+ccc.s-1, nexti);
}
void printCache(struct Cache ccc)
{
	printf("\n------ printing cache -----\n");
	printf("ccc.s = %d", ccc.s);
	for(int i = 0; i < ccc.S; i++){
      	printf("\nset %d[", i);
		for(int j = 0; j < ccc.E; j++){
      			//tags[i][j] = -1;
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
    //assert(high < 64 && (low <= high));
    //the amount of values between high and low inclusive are
    //high - (low - 1), a.k.a. population.
    unsigned long populationOfRegion = high - (low - 1);
    //TODO testing std::cout << "the population is: " << populationOfRegion << endl;
    //-------------------------------------
    // shift the region down to the lowest significant bit(so we can
    // mask and return faster because the answer will have the bit
    // in the lowest significant place already.
    // So, first shift the source down >> by low. If we getBits(9,10, 1000000000)
    // get our source to go from 1000000000 to 0000000010
    // a.k.a our unmasked result. Then we
    // need to clear(mask) out everything to the left of the population
    unsigned long unmaskedResult = source >> low; 
    // -----> clearing out extra junk to the left of population:
    // -----> the way to clear out extra junk is to make a mask of
    // -----> 1111111111 (-1) and shift it << by population which gives
    // -----> us 1111111100 then we COMPLEMENT that to get 0000000011
    // -----> with the mask of 1s we can now AND with our unmasked result.
    // -----------------------
    unsigned long mask = -1;
   //TODO testing std::cout << hex << "the mask is: " << mask << endl;
    mask = mask << (populationOfRegion - 1);
    mask = mask << 1;
    //TODO testing std::cout << hex << "the mask inverse is" << mask << endl;
    mask = ~mask;
    //TODO testing std::cout << hex << "the real mask is: " << mask << endl;
    // so we AND the mask with the unmasked result.
    // 0000000010
    // 0000000011 AND
    // --------------
    unsigned long result = unmaskedResult & mask;
    // 0000000010 is our result. which is the bits from 9-10
    // ----------------
   //TODO testing std::cout << hex << "the result is: " << result << endl; 
    
    return result;
}
