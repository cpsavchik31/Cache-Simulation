#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "memory.h"

// Note: You won't find <YOUR CODE HERE> comments in this file.... how lovely
// This is to get you to read and understand everything here.
// Good luck!

// - Anshu

int main(int argc, char* argv[]) {
    init_memory();
    int cacheSize, associativity, blockSize, index;
    FILE* f;
    if (argc != 5) {
        printf("%s: Wrong number of arguments, expecting 5\n", argv[0]);
        return EXIT_FAILURE;
    }


    typedef struct set_node {
        struct set_node* more_recent;
        unsigned char* data;
        int tag;
        int dirty;
        int valid;
        int lru;
    } set_node;


    // Buffer to store instruction (i.e. "load" or "store")
    char instruction_buffer[6];

    // Open the trace file in read mode
    FILE* myFile = fopen(argv[1], "r");

    // Read in the command line arguments
    sscanf(argv[2], "%d", &cacheSize);
    sscanf(argv[3], "%d", &associativity);
    sscanf(argv[4], "%d", &blockSize);


    int nsets = (cacheSize * 1024) / blockSize / associativity;
    if (nsets < 1) nsets = 1;

    int m = nsets;
    int q = 0;
    while (m >>= 1) q++;
    int ibits = q;

    int n = blockSize;
    int r = 0;
    while (n >>= 1) r++;
    int bbits = r;

    int tbits = 24 - ibits - bbits;



    set_node* cache[nsets];


    for (int i = 0; i < nsets; i++) {
        cache[i] = (set_node*)malloc(sizeof(set_node));
        set_node* head = cache[i];
        head->valid = 0;
        head->dirty = 0;
        head->data = (unsigned char*)malloc(blockSize * sizeof(unsigned char));
        head->lru = 0;
        head->more_recent = NULL;
        for (int j = 0; j < associativity - 1; j++) {
            //printf("%d\n", j);
            set_node* nset = (set_node*)malloc(sizeof(set_node));
            nset->more_recent = NULL;
            nset->valid = 0;
            nset->lru = 0;
            nset->dirty = 0;
            nset->data = (unsigned char*)malloc(blockSize * sizeof(unsigned char));
            head->more_recent = nset;
            head = head->more_recent;
        }
    }


    //int tick = 0;
    // Keep reading the instruction until end of file
    int pointless = 0;
    while (fscanf(myFile, "%s", &instruction_buffer) != EOF) {
        //tick++;
        int is_miss;
        int currAddress, accessSize;
        // Read the address and access size info
        fscanf(myFile, "%x", &currAddress);
        fscanf(myFile, "%d", &accessSize);
        unsigned char d_buff[64];
        u_int8_t val_load[blockSize];
        set_node* actually_used;

        int blockoff = (currAddress >> 0) & ((1 << bbits) - 1);
        int index = (currAddress >> bbits) & ((1 << ibits) - 1);
        //int ctag = (currAddress >> tbits) & ((1 << tbits) - 1);
        int ctag = (currAddress >> (bbits + ibits));
        //printf("the index is: %d, the blockoff is: %d, the tag is: %d\n", index, blockoff, ctag);
        set_node* lru_set = cache[index];
        set_node* temp = lru_set;
        unsigned char* target_block;
        set_node* target_index;
        int counter = 0;
        unsigned char* cold_block;
        set_node* cold_index;

        int hit = 0;
        int comp = 0;

        int max = temp->lru;
        while (temp != NULL) {
            if (temp->tag == ctag && temp->valid == 1) {
                hit = 1;
                break;
            }
            /*if (temp->tag == ctag && temp->valid == 0) {
                comp = 1;
                cold_block = temp->data;
                cold_index = temp;
            }*/

            if (temp->lru > max) {
                max = temp->lru;
                lru_set = temp;
            }
            temp = temp->more_recent;
            counter++;
        }

        if (temp != NULL) {
            target_block = temp->data;
            target_index = temp;
        }
        
      /*  else if(comp == 1){
            target_block = cold_block;
            target_index = cold_index;
        }*/

        else {
          int startAddress = currAddress - blockoff;
          read_from_memory(d_buff, startAddress, blockSize);
          unsigned char val_load[accessSize];
          for (int j = 0; j < accessSize; j++) {
              val_load[j] = d_buff[blockoff+j] ;
            }
        }



        int pointless = 0;
        if (instruction_buffer[0] == 'l') {

            if (hit == 1) { //HIT
                is_miss = 0;
                unsigned char output[(accessSize * 2) + 1];
                unsigned char* ptr = &output[0];
                for (int i = 0; i < accessSize; i++) {
                    ptr += sprintf(ptr, "%02x", target_block[blockoff + i]);
                }
                printf("load 0x%x hit %s\n", currAddress, output);
                target_index->lru = 0;
                target_index->tag = ctag;
                actually_used = target_index;
            }

            //else if (comp == 1) {  //COLD MISS
            //    printf("you detected a cold miss\n");
            //    is_miss = 0;
            //    unsigned char output[(accessSize * 2) + 1];
            //    unsigned char* ptr = &output[0];
            //    for (int i = 0; i < blockSize; i++) {
            //        target_block[i] = 0;
            //    }
            //    for (int i = 0; i < accessSize; i++) {
            //        ptr += sprintf(ptr, "%02x", target_block[blockoff + i]);
            //    }
            //    printf("load 0x%x miss %s\n", currAddress, output);
            //    target_index->lru = 0;
            //    target_index->valid = 1;
            //    target_index->tag = ctag;
            //    actually_used = target_index;
            //}
            else {  //CONFLICT MISS
                is_miss = 1;
                unsigned char* aim_block = lru_set->data;
                if (lru_set->dirty == 1) {
                    int startAddress1 = currAddress - blockoff;
                    write_to_memory(aim_block, startAddress1, blockSize);
                }

                for (int i = 0; i < blockSize; i++) {
                    *(aim_block + i) = *(d_buff + i);
                }

                unsigned char output1[(accessSize * 2) + 1];
                unsigned char* ptr1 = &output1[0];


                //printf("\n");
                if (lru_set->valid == 0) {
                    unsigned char p_block[accessSize];
                    for (int i = 0; i < accessSize; i++) {
                        p_block[i] = 0;
                    }
                    for (int i = 0; i < accessSize; i++) {
                        ptr1 += sprintf(ptr1, "%02x", p_block[i]);
                    }
                    printf("load 0x%x miss %s\n", currAddress, output1);
                }
                else {
                    for (int i = 0; i < accessSize; i++) {
                        ptr1 += sprintf(ptr1, "%02x", aim_block[blockoff + i]);
                    }
                    // printf("valload: %d", val_load[blockoff+i]);
                //printf("you detected a conflict miss\n");
                    printf("load 0x%x miss %s\n", currAddress, output1);
                }
                lru_set->valid = 1;
                lru_set->dirty = 0;
                lru_set->tag = ctag;
                lru_set->lru = 0;
                actually_used = lru_set;
            }
            }
        


        //STORE
        else {
            unsigned char data_buffer[64], * pos = data_buffer;
            fscanf(myFile, "%s", pos);
            unsigned char val[accessSize + 1];
            size_t count = 0;
            unsigned int byteval;
            while (sscanf(pos, "%2x", &byteval) == 1) {
                val[count] = byteval;
                count++;
                pos += 2 * sizeof(unsigned char);
            }

            if (hit == 1) { //hit
                is_miss = 0;
                for (int i = 0; i < accessSize; i++) {
                    target_block[blockoff + i] = val[i];
                }
                target_index->dirty = 1;
                printf("store 0x%x hit\n", currAddress);
                target_index->lru = 0;
                target_index->valid = 1;
                target_index->tag = ctag;
                actually_used = target_index;
            }


            //MISS
            else {
                is_miss = 1;

                unsigned char* aim_block = lru_set->data;

                if (lru_set->dirty == 1) {
                    int startAddress1 = currAddress - blockoff;
                    write_to_memory(aim_block, startAddress1, blockSize);
                }

                for (int i = 0; i < accessSize; i++) {
                    *(aim_block + (i + blockoff)) = *(val + i);
                }

                lru_set->valid = 1;
                lru_set->dirty = 0;
                lru_set->tag = ctag;
                lru_set->lru = 0;
                printf("store 0x%x miss\n", currAddress);
                actually_used = lru_set;
            }
        }

        if (associativity > 1) {
            set_node* header = cache[index];
            while (header != NULL) {
                if (header == actually_used) {
                    header = header->more_recent;
                }
                else {
                    header->lru = (header->lru) + 1;
                    header = header->more_recent;
                }

            }
        }
    }
    //free memory
  /*  for (int i = 0; i < index; i++) {
        set_node* head = cache[i];
        set_node* tmp;
        if (associativity > 1) {
            while (head != NULL) {
                int* currblk = head->data;
                free(currblk);
                tmp = head;
                head = head->more_recent;
                free(tmp);
            }
        }
        else {
            int* currblk = head->data;
            free(currblk);
            free(head);
        }
    }*/
    //printf("%s", "really");
    destroy_memory();
    //printf("%s", "really");
    return EXIT_SUCCESS;
}
