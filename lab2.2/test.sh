#!/bin/sh

echo "******* compile *******"
cd multi/
g++ -o ../Lab2.2_test/elf/multi multi_Simulation.cpp Read_Elf.cpp Read_Elf.h Reg_def.h Simulation.h
cd ..

cd pipeline/
g++ -o ../Lab2.2_test/elf/pipeline pipeline_Simulation.cpp Read_Elf.cpp Read_Elf.h Reg_def.h Simulation.h
cd ..

cd super_pipeline/
g++ -o ../Lab2.2_test/elf/super_pipeline pipeline_super_Simulation.cpp Read_Elf.cpp Read_Elf.h Reg_def.h Simulation.h
cd ..

echo "******* test *******"
cd Lab2.2_test/elf
sh multi_test.sh > ../../multi.txt
sh pipeline_test.sh > ../../pipeline.txt
sh superpipeline_test.sh > ../../super_pipeline.txt

echo "******** end *******"
