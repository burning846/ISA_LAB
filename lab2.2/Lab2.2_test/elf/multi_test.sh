#! /bin/sh
echo "**********  test1  **********" 
echo "l test1\nr\nm 70976 4\nm 70952 4\nm 70956 4\nq\n" | ./multi
echo "**********  test2  **********" 
echo "l test2\nr\nm 70976 4\nm 70952 4\nm 70956 4\nq\n" | ./multi
echo "**********  test3  **********" 
echo "l test3\nr\nm 69056 40\nq\n" | ./multi
echo "**********  test4  **********" 
echo "l test4\nr\nm 69104 40\nm 71016 4\nq\n" | ./multi
echo "**********  test5  **********" 
echo "l test5\nr\nm 69072 40\nq\n" | ./multi
echo "**********  test6 **********" 
echo "l test6\nr\nm 70920 4\nm 70944 4\nq\n" | ./multi
echo "**********  test7  **********" 
echo "l test7\nr\nm 69072 40\nm 70968 4\nq\n" | ./multi
echo "**********  test8  **********" 
echo "l test8\nr\nm 69184 40\nm 71080 4\nq\n" | ./multi
echo "**********  test9  **********" 
echo "l test9\nr\nm 71000 4\nm 71004 4\nm 71008 4\nq\n" | ./multi
echo "**********  test10 **********" 
echo "l test10\nr\nm 70936 4\nm 70940 4\nm 70944 4\nq\n" | ./multi
