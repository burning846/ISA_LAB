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
    Cache l1, l2;
    l1.SetLower(&l2);
    l2.SetLower(&m);

    StorageStats s;
    memset(&s, 0, sizeof(s));
    m.SetStats(s);
    l1.SetStats(s);
    l2.SetStats(s);

    StorageLatency ml;
    ml.bus_latency = 0;
    ml.hit_latency = 100;
    m.SetLatency(ml);

    StorageLatency ll;
    ll.bus_latency = 0;
    ll.hit_latency = 3;
    l1.SetLatency(ll);

    ll.bus_latency = 6;
    ll.hit_latency = 4;
    l2.SetLatency(ll);

#if 0
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

#if 1
    CacheConfig cc;
    cc.size = 32 * 1024;
    cc.associativity = 8;
    cc.set_num = 64;
    cc.write_through = 0;
    cc.write_allocate = 1;
    cc.prefetch = 0;
    cc.bypass = 0;
    l1.SetConfig(cc);

    cc.size = 256 * 1024;
    cc.associativity = 8;
    cc.set_num = 512;
    cc.write_through = 0;
    cc.write_allocate = 1;
    cc.prefetch = 0;
    cc.bypass = 0;
    l2.SetConfig(cc);
    

    int hit, time;
    char content[64] = {0};
    long long miss_sum = 0;
    long long access_sum = 0;

    for(int i = 0; i < 5; i++)
    {
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
            fin >> hex >> addr;
            //fin >>  addr;
            hit = 0;
            time = 0;
            //printf("%c %llx(%lld) ...... ", op, addr, addr);
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
        printf("*****************************************************\n");
        printf("Total L1 access time:     %dcycles\n", s.access_time);
        printf("Total L1 access number:   %d\n", s.access_counter);
        printf("Total L1 miss number:     %d\n", s.miss_num);
        printf("Total L1 miss rate:       %lf%%\n", (double)s.miss_num * 100.0 / s.access_counter);
        printf("Total L1 replace number:  %d\n", s.replace_num);
        printf("Total L1 fetch number:    %d\n", s.fetch_num);
        printf("Total L1 bypass number:   %d\n", s.bypass_num);
        printf("Total L1 prefetch number: %d\n\n", s.prefetch_num);
        l2.GetStats(s);
        printf("Total L2 access time:     %dcycles\n", s.access_time);
        printf("Total L2 access number:   %d\n", s.access_counter);
        printf("Total L2 miss number:     %d\n", s.miss_num);
        printf("Total L2 miss rate:       %lf%%\n", (double)s.miss_num * 100.0 / s.access_counter);
        printf("Total L2 replace number:  %d\n", s.replace_num);
        printf("Total L2 fetch number:    %d\n", s.fetch_num);
        printf("Total L2 bypass number:   %d\n", s.bypass_num);
        printf("Total L2 prefetch number: %d\n\n", s.prefetch_num);
        m.GetStats(s);
        printf("Total Memory access time: %dns\n", s.access_time);

        printf("Total Require Time: %dns\n", total_time);
        printf("Total Require: %dtimes, hit: %dtimes, miss: %dtimes\n", total_require, total_hit, total_miss);
        printf("Miss rate: %lf%%\n", (double)total_miss * 100.0 / total_require);
        printf("Average memory access time: %lf\n", (double)total_time / total_require);
        printf("****************************************************\n\n");

        miss_sum += total_miss;
        access_sum += total_require;
    }

    printf("Total miss:         %d\n", miss_sum);
    printf("Total access:       %d\n", access_sum);
    printf("Average miss rate:  %lf%%\n", (double)miss_sum * 100.0 / access_sum);
#endif

    return 0;
}
