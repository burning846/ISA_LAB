#include "cache.h"
#include "def.h"
#include <string.h>

void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                            char *content, int &hit, int &time) {
    hit = 0;
    time = 0;

    int s = getbitnum(config_.set_num);
    int b = getbitnum(block_size);
    set = getbit(addr, s + b - 1, b);
    block = getbit(addr, b - 1, 0);
    tag = addr >> (s + b);
    uint64_t cache_addr = 0, write_addr = 0;

    //printf("s: %d, b: %d, set: %d, block: %d, tag: %llx\n", s,b,set,block,tag);

    // Bypass?
    if (!BypassDecision()) {
        PartitionAlgorithm();
        // Miss?
        if (ReplaceDecision()) {
            // Choose victim
            ReplaceAlgorithm();
        } 
        else {
            //printf("hit\n");
            // return hit & time
            hit = 1;
            time += latency_.bus_latency + latency_.hit_latency;
            stats_.access_time += time;
            if(read)
            {
                memcpy(content, &blocks[set][line].bits[block], bytes);
            }
            else
            {
                memcpy(&blocks[set][line].bits[block], content, bytes);
                if(config_.write_through == 0)
                {
                    //write back
                    blocks[set][line].write = 1;
                }
                else
                {
                    //write through
                    int lower_hit, lower_time;
                    lower_->HandleRequest(addr, bytes, 0, content,
                                lower_hit, lower_time);
                    time += lower_time;
                }

               
            }
            
            return;
        }
    }

    //printf("miss\n");
      // Prefetch?
    if (PrefetchDecision()) {
        PrefetchAlgorithm();
    } 
    else {
        hit = 0;
        cache_addr = (tag << (s + b)) | (set << b);
        write_addr = (blocks[set][line].tag << (s + b)) | (set << b);
        //time += latency_.bus_latency + lower_time;
        time += latency_.bus_latency;
        stats_.access_time += latency_.bus_latency;

        // Fetch from lower layer
        char* tmp_content = new char[block_size];
        int lower_hit, lower_time;
        //printf("1\n");

        if(read)
        {
            //printf("1.1\n");
            lower_->HandleRequest(cache_addr, block_size, read, tmp_content,
                            lower_hit, lower_time);
            //printf("1.3\n");
            time += lower_time;

            memcpy(content, tmp_content + block, bytes);
            //printf("2\n");

            if(config_.write_through == 0)
            {
                //write back
                if(blocks[set][line].write == 1)
                {
                    lower_->HandleRequest(write_addr, block_size, 0, 
                                        blocks[set][line].bits, lower_hit, lower_time);
                    time += lower_time;
                }
            }
            //printf("3\n");
            memcpy(blocks[set][line].bits, tmp_content, block_size);
            int tmp_valid = blocks[set][line].valid;
            for(int i = 0; i < config_.associativity; ++i)
            {
                if(blocks[set][i].valid > tmp_valid)
                    blocks[set][i].valid -= 1;
            }
            blocks[set][line].valid = config_.associativity;
            blocks[set][line].tag = tag;
        }
        else
        {
            //printf("1.2\n");
            if(config_.write_allocate == 1)
            {
                if(config_.write_through == 0)
                {
                    //write back
                    if(blocks[set][line].write == 1)
                    {
                        lower_->HandleRequest(write_addr, block_size, 0, 
                                                blocks[set][line].bits, lower_hit, lower_time);
                        time += lower_time;
                    }
                }
                //printf("4\n");
                //write allocation
                lower_->HandleRequest(cache_addr, block_size, 1, blocks[set][line].bits, lower_hit, lower_time);
                time += lower_time;
                memcpy(&blocks[set][line].bits[block], content, bytes);
                int tmp_valid = blocks[set][line].valid;
                for(int i = 0; i < config_.associativity; ++i)
                {
                    if(blocks[set][i].valid > tmp_valid)
                        blocks[set][i].valid -= 1;
                }
                blocks[set][line].valid = config_.associativity;
                blocks[set][line].tag = tag;
                //printf("5\n");
                if(config_.write_through == 0)
                {
                    //write back
                    blocks[set][line].write = 1;
                }
                else
                {
                    //write through
                    lower_->HandleRequest(addr, bytes, 0, blocks[set][line].bits, lower_hit, lower_time);
                    time += lower_time;
                }
            }
            else
            {
                //printf("6\n");
                //write noallocation
                lower_->HandleRequest(addr, bytes, 0, content, lower_hit, lower_time);
                time += lower_time;
            }
        }

        //printf("7\n");
        delete tmp_content;
    }
}

int Cache::BypassDecision() {
    return FALSE;
}

void Cache::PartitionAlgorithm() {
}

int Cache::ReplaceDecision() {
    for(line = 0; line < config_.associativity; ++line)
    {
        if(blocks[set][line].valid != 0 && blocks[set][line].tag == tag)
            return FALSE;
    }
    return TRUE;
}

void Cache::ReplaceAlgorithm(){
    int victim = -1;
    for(line = 0; line < config_.associativity; ++line)
    {
        if(blocks[set][line].valid == 0)
            return;
        if(blocks[set][line].valid == 1)
            victim = line;
    }
    line = victim;
}

int Cache::PrefetchDecision() {
    return FALSE;
}

void Cache::PrefetchAlgorithm() {
}


void Cache::SetConfig(const CacheConfig &cc){
    //config
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

void Cache::GetConfig(CacheConfig &cc){
    cc = config_;
}