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
	int flag = 0;
	if (flag == 1) {
		return 1000;
	}
	while (b) {
		a %= b;
		int tmp = a;
		a = b;
		b = tmp;
	}
	return a;
}
*/

const int len = 71;
void* addr;
unsigned char code[] = {0x55, 0x48, 0x89, 0xe5, 0x89, 0x7d, 0xec, 0x89, 0x75, 0xe8, 0xc7, 0x45, 0xf8, /*start*/0x00, 0x00, 0x00, 0x00/*finish*/, 0x83, 0x7d, 0xf8, 0x01, 0x75, 0x07, 0xb8, 0xe8, 0x03, 0x00, 0x00, 0xeb, 0x27, 0x83, 0x7d, 0xe8, 0x00, 0x74, 0x1e, 0x8b, 0x45, 0xec, 0x99, 0xf7, 0x7d, 0xe8, 0x89, 0x55, 0xec, 0x8b, 0x45, 0xec, 0x89, 0x45, 0xfc, 0x8b, 0x45, 0xe8, 0x89, 0x45, 0xec, 0x8b, 0x45, 0xfc, 0x89, 0x45, 0xe8, 0xeb, 0xdc, 0x8b, 0x45, 0xec, 0x5d, 0xc3};
int gcd (int a, int b) {
	int flag = 0;
	if (flag == 1) {
		return 1000;
	}
	while (b) {
		a %= b;
		int tmp = a;
		a = b;
		b = tmp;
	}
	return a;
}

int map() {
    addr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED) {
        std::cerr << "Failed mmap. Error is " << strerror(errno) << std::endl;
        return -1;
    }
    memcpy(addr, code, len);
	return 0;
}

int patch(int val) {
	if (mprotect(addr, 4096, PROT_WRITE | PROT_READ) == -1) {
		std::cerr << "Couldn't change a rights. Error is " << strerror(errno) << std::endl;
		return -1;
	}
	for(int i = 0; i < 4; i++) {
		code[13 + i] = static_cast<unsigned char>(val % 256);
		val /= 256;
	}
	memcpy(addr, code, len);
	return 0;
}

int exec(int a, int b) {
	if (mprotect(addr, 4096, PROT_READ | PROT_EXEC) == -1) {
        std::cerr << "Couldn't change rights. Error is " << strerror(errno) << std::endl;
        return -1;
    }
    std::cout << "Good ans: " << gcd(a, b) << std::endl;
    std::cout << reinterpret_cast<int(*)(int, int)>(addr)(a, b) << std::endl;
	return 0;
}

int clear() {
    if (munmap(addr, 4096) == -1) {
        std::cerr << "Couldn't clear a memory. Error is " << strerror(errno) << std::endl;
		return -1;
    }
	return 0;
}

int main(int argc, char** argv, char** env) {
	if (map() == -1) {
		exit(EXIT_FAILURE);
	}
	while (true) {
		std::string command;
		std::cin >> command;
		if (command == "exec") {
			int num1, num2;
			std::cin >> num1 >> num2;
			if (exec(num1, num2) == -1) {
				clear();
				exit(EXIT_FAILURE);
			}
		} else if (command == "patch") {
			int val;
			std::cin >> val;
			if(patch(val) == -1) {
				clear();
				exit(EXIT_FAILURE);
			}
		} else  if (command == "exit") {
			clear();
			exit(EXIT_SUCCESS);
		} else {
			std::cerr << "Incorrect command!" << std::endl;
		}
	}
	clear();
    exit(EXIT_SUCCESS);
}