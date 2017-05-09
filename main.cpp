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
    uint16_t data;
    uint32_t address;

	std::cout << "Opening /dev/vpbus..." << std::endl;	
	
	FILE* f = std::fopen("/dev/vpbus", "wb");
	if(!f)
	{
		std::cout << "Unable to open /dev/vpbus !" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "VPBUS opened !" << std::endl;
	}
	
    std::cout << "----------------------" << std::endl;
    std::cout << "Doing read sequence ..." << std::endl;

    address = 10;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(f, address, SEEK_SET);
    std::fread(&data, sizeof(data), 1, f);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    std::cout << "Read at " << std::hex << address << ", value: " << data << std::endl;

    timespec_diff(&start, &stop, &result);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms";

    std::cout << std::endl << "----------------------" << std::endl;
    std::cout << "Doing write sequence..." << std::endl;
	
    data = 0xABCD;

    std::cout << "Writing at " << std::hex << address << ", value: " << data << std::endl;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(f, address, SEEK_SET);
    std::fwrite(&data, sizeof(data), 1, f);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms";


    std::cout << "Closing device" << std::endl;
	std::fclose(f);
	
	return 0;
}

