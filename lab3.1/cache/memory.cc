#include "memory.h"
#include <string.h>

void Memory::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &time) {
	hit = 1;
	time = latency_.hit_latency + latency_.bus_latency;
	stats_.access_time += time;

	//printf("11\n");
	//printf("%llx(%lld), %d, %d\n", addr, addr, bytes, read);
	if(addr > MAX)
		addr = 0;
	//printf("%llx(%lld), %d, %d\n", addr, addr, bytes, read);
	if(read)
		memcpy(content, &memory[addr], bytes);
	else
		memcpy(&memory[addr], content, bytes);
	//printf("12\n");

	return;
}

