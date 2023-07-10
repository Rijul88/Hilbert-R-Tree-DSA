rm *.o
rm exe
gcc -c functions.c
gcc -c main.c
gcc -o exe main.o functions.o
./exe
rm *.o
rm exe
