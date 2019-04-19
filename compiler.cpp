
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <error.h>
#include <string.h>

/*
int gcd (int a, int b) {
	return b ?  gcd( b, a % b): a;
}
*/

const int len = 51;
unsigned char code[] ={0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xec, 0x89, 0x75, 0xe8, 0x83, 0x7d, 0xe8, 0x00, 0x74, 0x1e, 0x8b, 0x45, 0xec, 0x99, 0xf7, 0x7d, 0xe8, 0x89, 0x55, 0xec, 0x8b, 0x45, 0xec, 0x89, 0x45, 0xfc, 0x8b, 0x45, 0xe8, 0x89, 0x45, 0xec, 0x8b, 0x45, 0xfc, 0x89, 0x45, 0xe8, 0xeb, 0xdc, 0x8b, 0x45, 0xec, 0x5d, 0xc3};
int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}

int main(int argc, char** argv, char** env) {
    //mmap
    void* addr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED) {
        std::cerr << "Failed mmap. Error is " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    memcpy(addr, code, len);
    //change protect
    if (mprotect(addr, 4096, PROT_READ | PROT_EXEC) == -1) {
        std::cerr << "Couldn't change rights. Error is " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE); 
    }
    //execute
    std::cout << "Good ans: " << gcd(16, 24) << std::endl;
    std::cout << reinterpret_cast<int(*)(int, int)>(addr)(24, 16) << std::endl;
    //munmap
    if (munmap(addr, 4096) == -1) {
        std::cerr << "Couldn't clear a memory. Error is " << strerror(errno) << std::endl;
    }
    exit(EXIT_SUCCESS);
}