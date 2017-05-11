#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <array>
#include "time.h"

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}

int main(int argc, const char* argv[])
{
    struct timespec start, stop, result;
    static const uint16_t dataSize = 2048;
    uint16_t data[dataSize];
    uint32_t address;

    std::cout << "Opening /dev/vpbus..." << std::endl;	
	
    FILE* f = std::fopen("/dev/vpbus", "r+b");
    if(!f)
    {
        std::cout << "Unable to open /dev/vpbus !" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "VPBUS opened !" << std::endl;
    }
	
    setvbuf(f, NULL, _IONBF, 0);
    std::cout << "----------------------" << std::endl;
    std::cout << "Doing read sequence ..." << std::endl;

    address = 10;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(f, address, SEEK_SET);
    std::fread(data, sizeof(data[0]), dataSize, f);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    std::cout << "Read " << dataSize << " blocks of " << sizeof(data[0]) << " at " << std::hex << address << std::endl;

    timespec_diff(&start, &stop, &result);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms";

    std::cout << std::endl << "----------------------" << std::endl;
    std::cout << "Doing write sequence..." << std::endl;
	

    std::cout << "Writing " << dataSize << "blocks of " << sizeof(data[0]) << " at " << std::hex << address << std::endl;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(f, address, SEEK_SET);
    std::fwrite(&data, sizeof(data[0]), dataSize, f);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms";


    std::cout << "Closing device" << std::endl;
	std::fclose(f);
	
	return 0;
}

