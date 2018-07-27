#ifndef CACHE_STORAGE_H_
#define CACHE_STORAGE_H_

#include <stdint.h>
#include <stdio.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

// Storage access stats
typedef struct StorageStats_ {
    int access_counter;
    int miss_num;
    int access_time; // In nanoseconds
    int replace_num; // Evict old lines
    int fetch_num; // Fetch lower layer
    int prefetch_num; // Prefetch
    int bypass_num;
} StorageStats;

// Storage basic config
typedef struct StorageLatency_ {
    int hit_latency; // In nanoseconds
    int bus_latency; // Added to each request
} StorageLatency;

class Storage {
public:
    Storage() {}
    ~Storage() {}

    // Sets & Gets
    void SetStats(StorageStats ss) { stats_ = ss; }
    void GetStats(StorageStats &ss) { ss = stats_; }
    void SetLatency(StorageLatency sl) { latency_ = sl; }
    void GetLatency(StorageLatency &sl) { sl = latency_; }
    uint64_t getbit(uint64_t addr, int s, int e){
        return (addr << (63 - s)) >> (63 - s + e); 
    }
    int getbitnum(int a){
        if(a == 1)
            return 0;
        int result = 0;
        a--;
        while(a != 0)
        {
            a >>= 1;
            result++;
        }
        return result;
    }

    // Main access process
    // [in]  addr: access address
    // [in]  bytes: target number of bytes
    // [in]  read: 0|1 for write|read
    // [i|o] content: in|out data
    // [out] hit: 0|1 for miss|hit
    // [out] time: total access time
    virtual void HandleRequest(uint64_t addr, int bytes, int read,
                             char *content, int &hit, int &time) = 0;

protected:
    StorageStats stats_;
    StorageLatency latency_;
};

#endif //CACHE_STORAGE_H_ 
