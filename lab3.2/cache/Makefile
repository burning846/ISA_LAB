CC=g++

all: sim pipeline

sim: main.o cache.o memory.o
	$(CC) -o test/$@ $^

pipeline: pipeline_super_Simulation.o Read_Elf.o cache.o memory.o 
	$(CC) -o test/$@ $^

main.o: cache.h

cache.o: cache.h def.h

memory.o: memory.h

Read_Elf.o: Read_Elf.h

pipeline_super_Simulation.o: Simulation.h Reg_def.h cache.h memory.h

.PHONY: clean

clean:
	rm -rf sim pipeline *.o
