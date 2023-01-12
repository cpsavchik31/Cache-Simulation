/**
 * memory.c - Memory abstraction for ECE/CS 250 Project 4 (Fall 2020)
 * Simulates physical memory
 * 
 * Author: Anshu Dwibhashi
 * Last Updated: 27th Oct, 2020
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

unsigned char* memory __attribute__ ((visibility ("hidden")));
int read_calls __attribute__ ((visibility ("hidden")));
int write_calls __attribute__ ((visibility ("hidden")));
int bytes_read __attribute__ ((visibility ("hidden")));
int bytes_written __attribute__ ((visibility ("hidden")));


// Definitions ================================================================
void init_memory() {
	memory = (unsigned char*) malloc(16777216 * sizeof(unsigned char)); // 16 M
	memset(memory, 0, 16777216);
}

void destroy_memory() {
	free(memory);
}

/**
 * Reads "num_bytes" bytes from memory starting at "address" and stores
 * the result in buffer
 */
void read_from_memory(unsigned char* buffer, int address, int num_bytes) {
	for(int i = 0; i < num_bytes; i++){
		*(buffer + i) = *(memory + address + i);
	}

	read_calls++;
	bytes_read += num_bytes;
}

void write_to_memory(unsigned char* buffer, int address, int num_bytes) {
	for(int i = 0; i < num_bytes; i++){
		*(memory + address + i) = *(buffer + i);
	}
	write_calls++;
	bytes_written += num_bytes;
}
// ============================================================================

