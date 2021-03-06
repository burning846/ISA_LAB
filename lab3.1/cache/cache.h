#ifndef CACHE_CACHE_H_
#define CACHE_CACHE_H_

#include <stdint.h>
#include <string.h>
#include "storage.h"

typedef struct CacheConfig_ {
    int size;
    int associativity;
    int set_num; // Number of cache sets
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
} CacheConfig;

class CacheBlock
{
public:
    char valid;
    char write;
    uint64_t tag;
    char* bits;
    CacheBlock()
    {
        write = 0;
        valid = 0;
        tag = 0;
        bits = NULL;
    }
    void Setsize(int block_size)
    {
        if(bits)
            delete bits;
        bits = new char[block_size];
        memset(bits, 0, sizeof(bits));
    }
    ~CacheBlock()
    {
        if(bits != NULL)
            delete bits;
    }
};

class Cache: public Storage {
public:
    Cache() {
        blocks = NULL;
    }
    Cache(const CacheConfig_ &cc){
        config_ = cc;
        int size = cc.size;
        int associativity = cc.associativity;
        int set_num = cc.set_num;
        block_size = size / associativity / set_num;

        blocks = new CacheBlock*[set_num];
        for(int i = 0; i < set_num; ++i)
        {
            blocks[i] = new CacheBlock[associativity];
            for(int j = 0; j < associativity; ++j)
                blocks[i][j].Setsize(block_size);
        }
    }
    ~Cache() {
        if(blocks == NULL)
            return;
        delete blocks;
    }

    // Sets & Gets
    void SetConfig(const CacheConfig &cc);
    void GetConfig(CacheConfig &cc);
    void SetLower(Storage *ll) { lower_ = ll; }
    // Main access process
    void HandleRequest(uint64_t addr, int bytes, int read,
                       char *content, int &hit, int &time);

private:
    CacheBlock** blocks;
    int block_size;
    int set, line, block;
    uint64_t tag;

    // Bypassing
    int BypassDecision();
    // Partitioning
    void PartitionAlgorithm();
    // Replacement
    int ReplaceDecision();
    void ReplaceAlgorithm();
    // Prefetching
    int PrefetchDecision();
    void PrefetchAlgorithm();

    CacheConfig config_;
    Storage *lower_;
    DISALLOW_COPY_AND_ASSIGN(Cache);
};

#endif //CACHE_CACHE_H_ 
