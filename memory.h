/**
 * memory.h - Memory abstraction for ECE/CS 250 Project 4 (Fall 2020)
 * Simulates physical memory
 * 
 * Author: Anshu Dwibhashi
 * Last Updated: 27th Oct, 2020
 **/

// Signatures =================================================================
void init_memory();
void destroy_memory();
void read_from_memory(unsigned char*, int, int);
void write_to_memory(unsigned char*, int, int);
// ============================================================================

