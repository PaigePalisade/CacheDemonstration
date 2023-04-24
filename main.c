#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    // Stores memory address to make things easier in place of tag on a real cache
    unsigned int addr;
    // Stores when the block was last accessed or -1 if it has not been accessed
    // unused for direct mapped memory or random replacement
    int lastUsed;
} cacheBlock;

// MUST BE MULTIPLE OF 4
#define CACHE_SIZE 32

int main() {
    srand(time(0));

    FILE* fp = fopen("traces.txt", "r");

    // stores direct-mapped cache
    cacheBlock directMapped[CACHE_SIZE];
    for (int i = 0; i < CACHE_SIZE; i++) {
        directMapped[i] = (cacheBlock){0, -1};
    }
    int directHits = 0;

    // stores 2-way associative chache with random replacement
    cacheBlock random2Way[CACHE_SIZE/2][2];
    for (int i = 0; i < CACHE_SIZE / 2; i++) {
        for (int j = 0; j < 2; j++) {
            random2Way[i][j] = (cacheBlock){0, -1};
        }
    }
    int random2WayHits = 0;

    // stores 2-way associative chache with least-recently used replacement
    cacheBlock lru2Way[CACHE_SIZE/2][2];
    for (int i = 0; i < CACHE_SIZE / 2; i++) {
        for (int j = 0; j < 2; j++) {
            lru2Way[i][j] = (cacheBlock){0, -1};
        }
    }
    int lru2WayHits = 0;

    // stores 4-way associative chache with random replacement
    cacheBlock random4Way[CACHE_SIZE/4][4];
    for (int i = 0; i < CACHE_SIZE / 4; i++) {
        for (int j = 0; j < 4; j++) {
            random4Way[i][j] = (cacheBlock){0, -1};
        }
    }
    int random4WayHits = 0;

    // stores 4-way associative chache with least-recently replacement
    cacheBlock lru4Way[CACHE_SIZE/4][4];
    for (int i = 0; i < CACHE_SIZE / 4; i++) {
        for (int j = 0; j < 4; j++) {
            lru4Way[i][j] = (cacheBlock){0, -1};
        }
    }
    int lru4WayHits = 0;

    // stores fully associative cache with random replacement
    cacheBlock randomFully[CACHE_SIZE];
    for (int i = 0; i < CACHE_SIZE; i++) {
        randomFully[i] = (cacheBlock){0, -1};
    }
    int randomFullyHits = 0;

    // stores fully associative cache with least-recently used replacement
    cacheBlock lruFully[CACHE_SIZE];
    for (int i = 0; i < CACHE_SIZE; i++) {
        lruFully[i] = (cacheBlock){0, -1};
    }
    int lruFullyHits = 0;

    // number of memory operations done
    int numMemOps = 0;

    while (!feof(fp)) {
        unsigned int addr;
        fscanf(fp, "%x ", &addr);

        // attempt to access direct mapped cache
        int directIndex = addr % CACHE_SIZE;
        // check for hit
        // we assume no memory address is zero
        if (directMapped[directIndex].addr == addr) {
            directHits ++;
        }
        else {
            directMapped[directIndex].addr = addr;
        }

        // attempt to access 2-way associative cache with random replacement
        int random2WayIndex = addr % (CACHE_SIZE/2);
        // check for hit
        if (random2Way[random2WayIndex][0].addr == addr ||
            random2Way[random2WayIndex][1].addr == addr) {
                random2WayHits ++;
        // Check both memory blocks to see if they haven't been used
        } else if (random2Way[random2WayIndex][0].addr == 0){
            random2Way[random2WayIndex][0].addr = addr;
        } else if (random2Way[random2WayIndex][1].addr == 0){
            random2Way[random2WayIndex][1].addr = addr;
        } else {
            // do random replacement
            int i = rand() % 2;
            random2Way[random2WayIndex][i].addr = addr;
        }

        // attempt to access 2-way associative cache with least-recently used replacement
        int lru2WayIndex = addr % (CACHE_SIZE/2);
        // check for hit
        if (lru2Way[lru2WayIndex][0].addr == addr) {
            lru2WayHits ++;
            lru2Way[lru2WayIndex][0].lastUsed = numMemOps;
        } else if (lru2Way[lru2WayIndex][1].addr == addr){
            lru2WayHits ++;
            lru2Way[lru2WayIndex][1].lastUsed = numMemOps;
        } else if (lru2Way[lru2WayIndex][0].lastUsed < lru2Way[lru2WayIndex][1].lastUsed) {
            lru2Way[lru2WayIndex][0].addr = addr;
            lru2Way[lru2WayIndex][0].lastUsed = numMemOps;
        } else {
            lru2Way[lru2WayIndex][1].addr = addr;
            lru2Way[lru2WayIndex][1].lastUsed = numMemOps;
        }

        // attempt to access 4-way associative cache with random replacement
        int random4WayIndex = addr % (CACHE_SIZE/4);
        // check for hit
        int hasHit = 0;
        for (int i = 0; i < 4; i++) {
            if (random4Way[random4WayIndex][i].addr == addr) {
                hasHit = 1;
                random4WayHits ++;
                break;
            }
        }
        if (!hasHit) {
            int unused = 0;
            for (int i = 0; i < 4; i++) {
                // check for any unused blocks
                if (random4Way[random4WayIndex][i].addr == 0) {
                    random4Way[random4WayIndex][i].addr = addr;
                    unused = 1;
                    break;
                }
            }
            if (!unused) {
                // do random replacement
                int i = rand() % 4;
                random4Way[random4WayIndex][i].addr = addr;
            }
        }

        // attempt to access 4-way associative cache with least-recently used replacement
        int lru4WayIndex = addr % (CACHE_SIZE/4);
        // check for hit
        hasHit = 0;
        for (int i = 0; i < 4; i++) {
            if (lru4Way[lru4WayIndex][i].addr == addr) {
                hasHit = 1;
                lru4WayHits ++;
                lru4Way[lru4WayIndex][i].lastUsed = numMemOps;
                break;
            }
        }
        if (!hasHit) {
            int unused = 0;
            for (int i = 0; i < 4; i++) {
                // check for any unused blocks
                if (lru4Way[lru4WayIndex][i].addr == 0) {
                    lru4Way[lru4WayIndex][i].addr = addr;
                    lru4Way[lru4WayIndex][i].lastUsed = numMemOps;
                    unused = 1;
                    break;
                }
            }
            if (!unused) {
                int leastUsed = 0;
                for (int i = 1; i < 4; i++) {
                    if (lru4Way[lru4WayIndex][i].lastUsed < lru4Way[lru4WayIndex][leastUsed].lastUsed) {
                        leastUsed = i;
                    }
                }
                lru4Way[lru4WayIndex][leastUsed].addr = addr;
                lru4Way[lru4WayIndex][leastUsed].lastUsed = numMemOps;
            }
        }

        // attempt to access fully associative cache with random replacement
        // check for hit
        hasHit = 0;
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (randomFully[i].addr == addr) {
                randomFullyHits ++;
                hasHit = 1;
                break;
            }
        }
        if (!hasHit) {
            // check for any unused blocks
            int unused = 0;
            for (int i = 0; i < CACHE_SIZE; i++) {
                if (randomFully[i].addr == 0) {
                    randomFully[i].addr = addr;
                    unused = 1;
                    break;
                } 
            }
            if (!unused) {
                int i = rand() % CACHE_SIZE;
                randomFully[i].addr = addr;
            }
        }

        // attempt to access fully associative cache with least-recently used replacement
        // check for hit
        hasHit = 0;
        for (int i = 0; i < CACHE_SIZE; i++) {
            if (lruFully[i].addr == addr) {
                lruFully[i].lastUsed = numMemOps;
                lruFullyHits ++;
                hasHit = 1;
                break;
            }
        }
        if (!hasHit) {
            // check for any unused blocks
            int unused = 0;
            for (int i = 0; i < CACHE_SIZE; i++) {
                if (lruFully[i].addr == 0) {
                    lruFully[i].addr = addr;
                    lruFully[i].lastUsed = numMemOps;
                    unused = 1;
                    break;
                } 
            }
            if (!unused) {
                int leastUsed = 0;
                for (int i = 1; i < CACHE_SIZE; i++) {
                    if (lruFully[i].lastUsed < lruFully[leastUsed].lastUsed) {
                        leastUsed = i;
                    }
                }
                lruFully[leastUsed].addr = addr;
                lruFully[leastUsed].lastUsed = numMemOps;
            }
        }
        // increment total count
        numMemOps ++;
    }

    printf("number of memory operations: %d\n", numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from direct mapped cache: %d\n", directHits);
    printf("percent of hits from direct mapped cache: %.1lf%%\n", (100.0 * directHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from 2-way associative cache with random replacement: %d\n", random2WayHits);
    printf("percent of hits from 2-way associative cache with random replacement: %.1lf%%\n", (100.0 * random2WayHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from 2-way associative cache with least-recently ued replacement: %d\n", lru2WayHits);
    printf("percent of hits from 2-way associative cache with least-recently used replacement: %.1lf%%\n", (100.0 * lru2WayHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from 4-way associative cache with random replacement: %d\n", random4WayHits);
    printf("percent of hits from 4-way associative cache with random replacement: %.1lf%%\n", (100.0 * random4WayHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from 4-way associative cache with least-recently ued replacement: %d\n", lru4WayHits);
    printf("percent of hits from 4-way associative cache with least-recently used replacement: %.1lf%%\n", (100.0 * lru4WayHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from fully associative cache with random replacement: %d\n", randomFullyHits);
    printf("percent of hits from fully associative cache with random replacement: %.1lf%%\n", (100.0 * randomFullyHits)/numMemOps);
    printf("---------------------------------------\n");
    printf("number of hits from fully associative cache with least-recently used replacement: %d\n", lruFullyHits);
    printf("percent of hits from fully associative cache with least-recently used replacement: %.1lf%%\n", (100.0 * lruFullyHits)/numMemOps);
    printf("---------------------------------------\n");
    
}