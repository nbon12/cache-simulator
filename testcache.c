//To compile: gcc =std=c99 testcache.c -o tcache
//
#include <stdio.h>
#include <stdlib.h>
//#include <cstdlib>
//#include <iostream>
//#include <cassert>
//#include <cstdint>
//hello

struct Cache {
	int S; //numbers of sets
	int s; 			// # of set index bits
    int E; 			// Associativity
    int B; 			// size of block
    int b; 			// #of block bits
    unsigned long **tags;
};

int cacheReorder(unsigned long instruction, int j); //return 1 on eviction.
int movedown(int set); 
unsigned long getBits(unsigned low, unsigned high, unsigned long source);

int main(int argc, char *argv[])
{
	printf("\nHelloooo\n");
	
	struct Cache ccc;
	ccc.S = 1;
	struct Cache myCache;
	printf("%d", ccc.S);
	
	//printf("\ntag[0]:%d\n", ccc.tag[0]);
	//int s = 4;
	//int newarray[s];
	printf("Hisizeof:---\n\n%d\n\n", sizeof(long));
	int sets = 4;//set this from the args array
	int linesPerSet = 2;
	int blockbits = 4; //2>>blockbits = blocksize;
	int blocksize = 2 << (blockbits-1);
	//int test = 2 << 4;
	printf("Blocksize: %d Test: %d", blocksize);
	unsigned long **tags;
	tags = (unsigned long **)malloc(sets*sizeof(unsigned long *));
	for(int i = 0; i < sets; i++)
	{
		tags[i] = (unsigned long *)malloc(linesPerSet * sizeof(unsigned long));
	}
	unsigned long a = 99;
	tags[0][0] = a;
	tags[0][1] = a + 1;
	printf("num sets: %d", sets);
	printf("num lines %d", linesPerSet);
	printf("bytes per set: %d", blocksize);
	for(int i = 0; i < sets; i++){
      	printf("\nset %d[", i);
		for(int j = 0; j < linesPerSet; j++){
      			tags[i][j] = -1;
				printf("line %d: %d, ", j, tags[i][j]);
        }
		printf("]");
    }
	printf("\n\ntags[0][0] = %d", a);
	unsigned long instruction = 0x10;
	//
	//mask to get b,s,t and store them here. //create function to get b, s and t
	//out of the function.
	//get_inst_b(instruction, b) //given the number of bits for b, should return this
	//								// instruction's b.
	//get_inst_s(instruction, s) //given s and an instruction, will give us the part of the instruction
	//that is the inst_s.
	//get inst_t(instruction, t) gives us the tag from the instruction.
	unsigned long inst_b = 0x0;//
	unsigned long inst_s = 0x1;
	unsigned long inst_t = 0x0;
	inst_b = 5;
	//we have smalls, so lets check cache[inst_s] for the tag match.
	int hits = 0;
	int misses = 0;
	int evicts = 0;
	//we know what set it is, so
	//tags[inst_s][j];
	
	int causedMiss = 1; //causedMiss + causedEviction == 0 if it was a hit | 1 if it was a miss | 2 if it was a miss and eviction.
	int causedEviction = 0; 
	//set = getset(
	/*
	for(int j = 0; j < linesPerSet; j++)
	{
		if(1)
		{
			causedMiss = 0;
			//printf("dot");
			//reorder the cache so that most recently used(this one) is at
			//tags[inst_s][0], and all others get shifted over by 1.
			//cacheReorder(instruction, j);
		}
	}*/
	//if(hit(instr_t, instr_s)
	//{
	//	hits++;
	//}
	//else
	//{
	//	evicts += make_room(instr_s);
	//	misses++;
	//}
	
	if(causedMiss == 1)
	{
		//put the instruction into cache,
		//re-order so that most recently used(this one) is at
		//tags[inst_s][0], and if any get evicted, let evicted++;
		evicts += cacheReorder(instruction, -1);//returns 1 if caused eviction, else 0;
		
	}
	//start to simulate what cache would do with the following thing...
}	
/*
	Moves all tags in a set down by 1.
	@return 1 if an eviction occurred.
*/
int movedown(int set)
{

}
int cacheReorder(unsigned long instruction, int j)
{
	return 55;
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
/**
	//2^b == 2 >> b
    myCache.B = 2 >> myCache.b;
  	myCache.S = 2 >> myCache.s;
  	// Create the 2d array for tag
    myCache.tag = malloc(sizeof(int*) * S);
		
    FILE *fp;
  	char buff[80];
  	
  	fp = fopen(argv[argc], "r");
  	fgets(buff, 80, (FILE*)fp);
    printf("1: %s\n", buff);
    
    
  
    printSummary(0, 0, 0);
    return 0;

*/