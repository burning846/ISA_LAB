#include "cache.h"
#include "def.h"
#include <string.h>

#define LRU
//#define LFU
//#define FIFO

void Cache::HandleRequest(uint64_t addr, int bytes, int read,
    char *content, int &hit, int &time) {
    hit = 0;
    time = 0;

    ++stats_.access_counter;

    int s = getbitnum(config_.set_num);
    int b = getbitnum(block_size);
    set = getbit(addr, s + b - 1, b);
    block = getbit(addr, b - 1, 0);
    tag = addr >> (s + b);
    uint64_t cache_addr = 0, write_addr = 0;
    int lower_hit, lower_time;
    stats_.access_time += latency_.bus_latency + latency_.hit_latency;

    //printf("s: %d, b: %d, set: %d, block: %d, tag: %llx\n", s,b,set,block,tag);

    // Bypass?
    if (!BypassDecision()) {
        PartitionAlgorithm();
        // Miss
        if (ReplaceDecision(set, tag)) {
            // Choose victim
            ReplaceAlgorithm(set, tag);
            hit = 0;
            ++stats_.miss_num;
            cache_addr = (tag << (s + b)) | (set << b);
            write_addr = (blocks[set][line].tag << (s + b)) | (set << b);
            //time += latency_.bus_latency + lower_time;
            time += latency_.bus_latency;

            // Fetch from lower layer
            char* tmp_content = new char[block_size];
            
            //printf("1\n");

            if(read)
            {
                //printf("1.1\n");
                lower_->HandleRequest(cache_addr, block_size, read, tmp_content,
                    lower_hit, lower_time);
                ++stats_.fetch_num;
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
                        //time += lower_time;
                        ++stats_.fetch_num;
                    }
                }
                //printf("3\n");
                memcpy(blocks[set][line].bits, tmp_content, block_size);
                int tmp_valid = blocks[set][line].valid;
                //printf("4\n");
                for(int i = 0; i < config_.associativity; ++i)
                {
                    if(blocks[set][i].valid > tmp_valid)
                        blocks[set][i].valid -= 1;
                    if(i != line)
                        blocks[set][i].uses = 0;
                }
                //printf("5\n");
                blocks[set][line].valid = config_.associativity;
                history_tags[set] = blocks[set][line].tag;
                blocks[set][line].tag = tag;
                blocks[set][line].uses = 1;

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
                            //time += lower_time;
                            ++stats_.fetch_num;
                        }
                    }
                //printf("4\n");
                //write allocation
                    lower_->HandleRequest(cache_addr, block_size, 1, blocks[set][line].bits, lower_hit, lower_time);
                    ++stats_.fetch_num;
                    time += lower_time;
                    memcpy(&blocks[set][line].bits[block], content, bytes);
                    int tmp_valid = blocks[set][line].valid;
                    for(int i = 0; i < config_.associativity; ++i)
                    {
                        if(blocks[set][i].valid > tmp_valid)
                            blocks[set][i].valid -= 1;
                        if(i != line)
                            blocks[set][i].uses = 0;
                    }
                    blocks[set][line].valid = config_.associativity;
                    history_tags[set] = blocks[set][line].tag;
                    blocks[set][line].tag = tag;
                    blocks[set][line].uses = 1;
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
                        ++stats_.fetch_num;
                    }
                }
                else
                {
                //printf("6\n");
                //write noallocation
                    lower_->HandleRequest(addr, bytes, 0, content, lower_hit, lower_time);
                    time += lower_time;
                    ++stats_.fetch_num;
                }
            }

            if(PrefetchDecision())
            {
                PrefetchAlgorithm();
            }

        //printf("7\n");
            delete tmp_content;
        } 
        else //hit 
        {
            //printf("hit\n");
            // return hit & time
            hit = 1;
            time += latency_.bus_latency + latency_.hit_latency;
            if(read)
            {
                blocks[set][line].uses++;
                memcpy(content, &blocks[set][line].bits[block], bytes);
            }
            else
            {
                memcpy(&blocks[set][line].bits[block], content, bytes);
                blocks[set][line].uses++;
                if(config_.write_through == 0)
                {
                    //write back
                    blocks[set][line].write = 1;
                }
                else
                {
                    //write through
                    lower_->HandleRequest(addr, bytes, 0, content,
                        lower_hit, lower_time);
                    //time += lower_time;
                    ++stats_.fetch_num;
                }
            }
        }
        return;
    }

    //printf("bypass\n");
    hit = 0;
    ++stats_.miss_num;
    ++stats_.bypass_num;
    lower_->HandleRequest(addr, bytes, read, content,
                    lower_hit, lower_time);
    time += lower_time + latency_.bus_latency;
    ++stats_.fetch_num;

      // Prefetch?
    //if (PrefetchDecision()) {
    //    PrefetchAlgorithm();
    //} 
    // else {
        // hit = 0;
        // cache_addr = (tag << (s + b)) | (set << b);
        // write_addr = (blocks[set][line].tag << (s + b)) | (set << b);
        // //time += latency_.bus_latency + lower_time;
        // time += latency_.bus_latency;
        // stats_.access_time += latency_.bus_latency;

        // // Fetch from lower layer
        // char* tmp_content = new char[block_size];
        // int lower_hit, lower_time;
        // //printf("1\n");

        // if(read)
        // {
        //     //printf("1.1\n");
        //     lower_->HandleRequest(cache_addr, block_size, read, tmp_content,
        //                     lower_hit, lower_time);
        //     //printf("1.3\n");
        //     time += lower_time;

        //     memcpy(content, tmp_content + block, bytes);
        //     //printf("2\n");

        //     if(config_.write_through == 0)
        //     {
        //         //write back
        //         if(blocks[set][line].write == 1)
        //         {
        //             lower_->HandleRequest(write_addr, block_size, 0, 
        //                                 blocks[set][line].bits, lower_hit, lower_time);
        //             time += lower_time;
        //         }
        //     }
        //     //printf("3\n");
        //     memcpy(blocks[set][line].bits, tmp_content, block_size);
        //     int tmp_valid = blocks[set][line].valid;
        //     for(int i = 0; i < config_.associativity; ++i)
        //     {
        //         if(blocks[set][i].valid > tmp_valid)
        //             blocks[set][i].valid -= 1;
        //     }
        //     blocks[set][line].valid = config_.associativity;
        //     history_tags[set] = blocks[set][line].tag;
        //     blocks[set][line].tag = tag;
        // }
        // else
        // {
        //     //printf("1.2\n");
        //     if(config_.write_allocate == 1)
        //     {
        //         if(config_.write_through == 0)
        //         {
        //             //write back
        //             if(blocks[set][line].write == 1)
        //             {
        //                 lower_->HandleRequest(write_addr, block_size, 0, 
        //                                         blocks[set][line].bits, lower_hit, lower_time);
        //                 time += lower_time;
        //             }
        //         }
        //         //printf("4\n");
        //         //write allocation
        //         lower_->HandleRequest(cache_addr, block_size, 1, blocks[set][line].bits, lower_hit, lower_time);
        //         time += lower_time;
        //         memcpy(&blocks[set][line].bits[block], content, bytes);
        //         int tmp_valid = blocks[set][line].valid;
        //         for(int i = 0; i < config_.associativity; ++i)
        //         {
        //             if(blocks[set][i].valid > tmp_valid)
        //                 blocks[set][i].valid -= 1;
        //         }
        //         blocks[set][line].valid = config_.associativity;
        //         history_tags[set] = blocks[set][line].tag;
        //         blocks[set][line].tag = tag;
        //         //printf("5\n");
        //         if(config_.write_through == 0)
        //         {
        //             //write back
        //             blocks[set][line].write = 1;
        //         }
        //         else
        //         {
        //             //write through
        //             lower_->HandleRequest(addr, bytes, 0, blocks[set][line].bits, lower_hit, lower_time);
        //             time += lower_time;
        //         }
        //     }
        //     else
        //     {
        //         //printf("6\n");
        //         //write noallocation
        //         lower_->HandleRequest(addr, bytes, 0, content, lower_hit, lower_time);
        //         time += lower_time;
        //     }
        // }

        // //printf("7\n");
        // delete tmp_content;
    // }
}

int Cache::BypassDecision() {
    if(config_.bypass == 0)
        return false;
    if(tag == history_tags[set])
        return true;
    return false;
}

void Cache::PartitionAlgorithm() {
}

int Cache::ReplaceDecision(int set, uint64_t tag) {
    for(line = 0; line < config_.associativity; ++line)
    {
        if(blocks[set][line].valid != 0 && blocks[set][line].tag == tag)
            return false;
    }
    return true;
}

void Cache::ReplaceAlgorithm(int& set, uint64_t& tag){
    ++stats_.replace_num;
    int victim = -1;

#ifdef LRU
    for(line = 0; line < config_.associativity; ++line)
    {
        if(blocks[set][line].valid == 0)
            return;
        if(blocks[set][line].valid == 1)
            victim = line;
    }    
#endif

#ifdef LFU
    int use = 1 << 30;
    for(line = 0; line < config_.associativity; ++line)
    {
        if(blocks[set][line].valid == 0)
            return;
        if(blocks[set][line].valid != 0)
        {
            if(use > blocks[set][line].uses)
            {
                use = blocks[set][line].uses;
                victim = line;
            }
        }
    }
#endif

#ifdef FIFO
    victim = nowp[set] = (nowp[set] + 1 ) % config_.associativity;
#endif

    line = victim;
}

int Cache::PrefetchDecision() {
    if(config_.prefetch == 0)
        return false;
    return true;
}

void Cache::PrefetchAlgorithm() {
    ++stats_.prefetch_num;
    int s = getbitnum(config_.set_num);
    int b = getbitnum(block_size);
    uint64_t addr = (tag << (s + b)) | (set << b);
    for(int i = 0; i < 3; i++)
    {
        addr += 1ull << b;
        int tmp_set = getbit(addr, s + b - 1, b);
        uint64_t tmp_tag = addr >> (s + b);
        if(ReplaceDecision(tmp_set, tmp_tag))
        {
            ReplaceAlgorithm(tmp_set, tmp_tag);
            int victim = line;
            // for(line = 0; line < config_.associativity; ++line)
            // {
            //     if(blocks[tmp_set][line].valid == 0)
            //     {
            //         victim = line;
            //         break;
            //     }
            // }
            // if(victim == -1)
            //     continue;
            // else
            // {
                int lower_hit, lower_time;
                if(config_.write_through == 0)
                {
                //write back
                    if(blocks[set][line].write == 1)
                    {
                        lower_->HandleRequest((blocks[set][line].tag << (s + b)) | (set << b), block_size, 0, 
                            blocks[set][line].bits, lower_hit, lower_time);
                        ++stats_.fetch_num;
                    }
                }


                lower_->HandleRequest(addr, block_size, 1, blocks[tmp_set][victim].bits,
                    lower_hit, lower_time);
                ++stats_.replace_num;

                int tmp_valid = blocks[set][line].valid;
                //printf("4\n");
                for(int i = 0; i < config_.associativity; ++i)
                {
                    if(blocks[set][i].valid > tmp_valid)
                        blocks[set][i].valid -= 1;
                    if(i != line)
                        blocks[set][i].uses = 0;
                }
                //printf("5\n");
                blocks[set][line].valid = config_.associativity;
                history_tags[set] = blocks[set][line].tag;
                blocks[set][line].tag = tag;
                blocks[set][line].uses = 1;
            // }
        }
    }
}


void Cache::SetConfig(const CacheConfig &cc){
    //config
    config_ = cc;
    int size = cc.size;
    int associativity = cc.associativity;
    int set_num = cc.set_num;
    block_size = size / associativity / set_num;

    blocks = new CacheBlock*[set_num];
    nowp = new int[set_num];
    memset(nowp, 0, sizeof(nowp));
    history_tags = new uint64_t[set_num];
    memset(history_tags, 0, sizeof(history_tags));
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