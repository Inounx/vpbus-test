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
	
	
	std::cout << "Enter \"W address data\" to write" << std::endl;	
    std::cout << "or \"R address\" to read" << std::endl;
	std::cout << "address must be between 0x00 and 0xFF" << std::endl;
    std::cout << "data is 16bits" << std::endl;
	
    char command = 'R';
	uint8_t address = 0;
	uint16_t data = 0;
	scanf("%c %d %d", command, address, data);
	address = address << 1;
	if(command == 'R' || command == 'r')
	{
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		std::fseek(f, address, std::SEEK_SET);
		std::fread(&data, sizeof(data), 1, f);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		std::cout << "Read at " << std::hex << address << ", value: " << data << std::endl;

		timespec_diff(&start, &stop, &result);
		std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms"
	}
	else
	{
		std::cout << "Writing at " << std::hex << address << ", value: " << data << std::endl;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		std::fseek(f, address, std::SEEK_SET);
		std::fwrite(&data, sizeof(data), 1, f);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
		std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms"
	}

	std::cout << "fermeture du fichier" << std::endl;
	std::fclose(f);
	
	return 0;
}

