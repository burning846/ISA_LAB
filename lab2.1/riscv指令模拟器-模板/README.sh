
# Lab 2.1 
# A naive Simulator

# @author Burning(Boning) Song 1500012846

# Support:
# RISC-V greencard ISA except ecall & mulh
# Run by step
 
# compile command:
# g++ -o naive naive_Simulation.cpp Simulation.h Reg_def.h Read_Elf.h Read_Elf.cpp

# Naive_simulator command usage
# q: quit
# r: run the program
# d: change step mode
# m $start $num: check the memory of last running program, $start = the beginning of what you want to see, $num = the number of bytes to show
# l $file_name : read elf file into simulator

# Running debug command usage:
# r: show register
# m $start $num: show memory, $start = the beginning of what you want to see, $num = the number of bytes to show
# c: run one step
# g: run till the end
# k: stop the program

# test program: qosrt
# the address of array is memory[69648(0x11010)] and size is 40
# use "m 69648 40" to see the result in step mode
# use "c" to run by step

#run this shell to check
echo "d\nr\nm 69648 40\ng\nm 69648 40\nq\n" | ./naive qsort