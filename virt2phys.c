
// TODO: Everything.... thank you for that 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>



int main(int argc, char** argv[])
{
    char* pgtable = *(argv + 1);
    char* vadd = *(argv + 2);
    int addlen = strlen(vadd);
    addlen = addlen * 4;
    int addbits;
    int pgsz;
    int pgoffsz;
    unsigned int bivadd;
    int x;

    //check validity of each individual input?
    if (argc != 3) {
        printf("Wrong number of arguments, expecting 2\n");
        return 0;
    }
    FILE* f;
    f = fopen(pgtable, "r");
    if (f == NULL) {
        return 0;
    }
    fscanf(f, "%d", &addbits);
    fscanf(f, "%d", &pgsz);
    sscanf(vadd, "%x", &bivadd);

    //log
    int n = pgsz;
    int r = 0;
    while (n >>= 1) r++;
    pgoffsz = r;

    //bit mask VA 
    unsigned int tot_mask = 4294967295;
    int shift_mask = tot_mask << (pgoffsz);
    //get upper bits
    unsigned int vpn = bivadd & shift_mask;
    //get lower bits
    int oset = bivadd - vpn;
    //printf("offset: %x\n", oset);
    vpn = vpn >> pgoffsz;
    //printf("vpn: %d\n", vpn);
    
    int i = 0;
    char s1[24];
    while (!fscanf(f, "%s\n", s1) != EOF)
    {
        if (i == vpn) {
            break;
        }
        i++;
    }

    fclose(f);
    //int help = addbits - pgoffsz;
    int ppn = atoi(s1);
    //printf("ppn: %d\n", ppn);
    //printf("%x\n", ppn);
    if (ppn == -1) {
        printf("%s", "PAGEFAULT\n");
        return 0;
    }
    // scanf(s1, "%X\n", &ppn);
     //concatenate physical address
     //printf("%s\n", ppn);
    ppn = ppn << pgoffsz;
    int pa = ppn | oset;
    printf("%x\n", pa);

    return 0;
}
