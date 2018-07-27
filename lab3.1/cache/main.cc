#include "stdio.h"
#include "cache.h"
#include "memory.h"
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
    if(argc != 2)
    {
        printf("Invalid cmd!\n");
        return 0;
    }

    Memory m;
    Cache l1;
    l1.SetLower(&m);

    StorageStats s;
    s.access_time = 0;
    m.SetStats(s);
    l1.SetStats(s);

    StorageLatency ml;
    ml.bus_latency = 6;
    ml.hit_latency = 100;
    m.SetLatency(ml);

    StorageLatency ll;
    ll.bus_latency = 3;
    ll.hit_latency = 10;
    l1.SetLatency(ll);

#if 1
    for(int size = 32 * 1024; size <= 32 * 1024 * 1024 ; size <<= 1) //
    {
        for(int associativity = 1; associativity <= 32; associativity <<= 1)
        {
            for(int set_num = 1; set_num <= 512; set_num <<= 1)
            {
                CacheConfig cc;
                cc.size = size;
                cc.associativity = associativity;
                cc.set_num = set_num;
                cc.write_through = 1;
                cc.write_allocate = 0;
                l1.SetConfig(cc);
                //printf("size: %dKB, set: %d, associativity: %d, block_size: %d\n", size/1024, set_num, associativity, size / set_num / associativity);
                printf("%d,%d,%d,%d,", size/1024, set_num, associativity, size / set_num / associativity);

                int hit, time;
                char content[64] = {0};

                fstream fin;
                fin.open(argv[1]);
                char op;
                uint64_t addr;
                int total_hit = 0, total_miss = 0, total_time = 0, total_require = 0;
                int read = 0;
                while(fin >> op)
                {
                    if(op == 'r')
                        read = 1;
                    else
                        read = 0;
                    //printf("1\n");
                    fin >> addr;
                    hit = 0;
                    time = 0;
                    //printf("%c %llx ...... ", op, addr);
                    l1.HandleRequest(addr, 1, read, content, hit, time);
                    if(hit)
                    {
                        total_hit++;
                        //printf("hit!\n");
                    }
                    else
                    {
                        //printf("miss!\n");
                    }
                    total_require++;
                    total_time += time;
                    //printf("Request access time: %dns\n", time);
                }

                fin.close();
                total_miss = total_require - total_hit;

                //l1.HandleRequest(0, 0, 1, content, hit, time);
                //printf("Request access time: %dns\n", time);
                //l1.HandleRequest(1024, 0, 1, content, hit, time);
                //printf("Request access time: %dns\n", time);

                l1.GetStats(s);
                //printf("Total L1 access time: %dns\n", s.access_time);
                printf("%d,", s.access_time);
                m.GetStats(s);
                //printf("Total Memory access time: %dns\n", s.access_time);
                //printf("Total Require Time: %dns\n", total_time);
                //printf("Total Require: %dtimes, hit: %dtimes, miss: %dtimes\n", total_require, total_hit, total_miss);
                //printf("Miss rate: %f%%\n\n", (double)total_miss * 100.0 / total_require);
                printf("%d,", s.access_time);
                printf("%d,", total_time);
                printf("%d,%d,", total_require, total_hit, total_miss);
                printf("%f%%\n", (double)total_miss * 100.0 / total_require);
            }
        }
    }
#endif

#if 0
    CacheConfig cc;
    cc.size = 32 * 1024 * 512;
    cc.associativity = 1;
    cc.set_num = 1;
    cc.write_through = 0;
    cc.write_allocate = 1;
    l1.SetConfig(cc);
    

    int hit, time;
    char content[64] = {0};

    fstream fin;
    fin.open(argv[1]);
    char op;
    uint64_t addr;
    int total_hit = 0, total_miss = 0, total_time = 0, total_require = 0;
    int read = 0;

    while(fin >> op)
    {
        if(op == 'r')
            read = 1;
        else
            read = 0;
        fin >> addr;
        hit = 0;
        time = 0;
        printf("%c %llx(%lld) ...... ", op, addr, addr);
        l1.HandleRequest(addr, 1, read, content, hit, time);
        if(hit)
        {
            total_hit++;
            printf("hit!\n");
        }
        else
        {
            printf("miss!\n");
        }
        total_require++;
        total_time += time;
        printf("Request access time: %dns\n", time);
    }

    fin.close();
    total_miss = total_require - total_hit;

    //l1.HandleRequest(0, 0, 1, content, hit, time);
    //printf("Request access time: %dns\n", time);
    //l1.HandleRequest(1024, 0, 1, content, hit, time);
    //printf("Request access time: %dns\n", time);

    l1.GetStats(s);
    printf("Total L1 access time: %dns\n", s.access_time);
    m.GetStats(s);
    printf("Total Memory access time: %dns\n", s.access_time);
    printf("Total Require Time: %dns\n", total_time);
    printf("Total Require: %dtimes, hit: %dtimes, miss: %dtimes\n", total_require, total_hit, total_miss);
    printf("Miss rate: %f\n", (double)total_miss / total_require);
#endif

    return 0;
}
