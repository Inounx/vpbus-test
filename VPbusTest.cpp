#include "VPbusTest.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <array>
#include "time.h"
#include <string.h>
#include <stropts.h>
#include <sys/select.h>
#include <limits>

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

VpbusTest::VpbusTest() :
    m_address(0),
    m_accessSize(0),
    m_dataToWrite(0),
    m_dataToRead(0),
    m_device(0)
{
}

VpbusTest::~VpbusTest()
{
    if(m_dataToWrite != 0)
    {
        delete[] m_dataToWrite;
    }

    if(m_dataToRead != 0)
    {
        delete[] m_dataToRead;
    }
}

int VpbusTest::run(int argc, const char* argv[])
{
    if(parseArgs(argc, argv))
    {
        std::cout << "Opening /dev/vpbus..." << std::endl;

        m_device = std::fopen("/dev/vpbus", "r+b");
        if(!m_device)
        {
            std::cout << "Unable to open /dev/vpbus !" << std::endl;
            return 0;
        }
        else
        {
            std::cout << "VPBUS opened !" << std::endl;
        }

        setvbuf(m_device, NULL, _IONBF, 0);

        if(m_accessType == ReadAccess ||
           m_accessType == ReadWriteAccess)
        {
            doRead();
        }

        if(m_accessType == WriteAccess ||
           m_accessType == ReadWriteAccess ||
           m_accessType == WriteReadAccess)
        {
            doWrite();
        }

        if(m_accessType == WriteReadAccess)
        {
            doRead();
        }

        if(m_accessType == CyclicWriteThenReadAccess)
        {
            doCyclicWriteThenRead();
        }

        std::cout << "Closing device" << std::endl;
        std::fclose(m_device);
    }
    else
    {
        std::cout << "Invalid arguments." << std::endl;
        std::cout << "Use vpbus_test <accessType> <wordAdress> <size> <dataType> <data>" << std::endl;
        std::cout << "[AccessType]:"<<std::endl;
        std::cout << "  -\"r\": Read only access" << std::endl;
        std::cout << "  -\"w\": Write only access" << std::endl;
        std::cout << "  -\"rw\": Read then write access" << std::endl;
        std::cout << "  -\"wr\": Write then read access" << std::endl;
        std::cout << "  -\"cwrc\": Cyclic Write then Read with Check" << std::endl;
        std::cout << "[wordAdress]: Start address of access (for both read and write) in word." << std::endl;
        std::cout << "[size]: Size of the acces in words (for both read and write)." << std::endl;
        std::cout << "[dataType]: Needed for write." << std::endl;
        std::cout << "  -\"p\": Alterned pattern of 0x5555 and 0xAAAA" << std::endl;
        std::cout << "  -\"i\": Increment data start at <data>. If no data starts at 0" << std::endl;
        std::cout << "  -\"d\": Decrement data start at <data>. If no data starts at 0xFFFF" << std::endl;
        std::cout << "[data]: When dataType is \"i\" or \"d\", start value of data to write." << std::endl;
        std::cout << std::endl;
        std::cout << "Note: in cyclic write then read, the output is less verbose to be able to run the test over long period of time." << std::endl;
        std::cout << "Only the access errors will be outputed, and when the test will be terminated, the count of error over total access will be displayed" << std::endl;
    }

    return 0;
}


bool VpbusTest::parseArgs(int argc, const char* argv[])
{
    if(argc >= 4)
    {
        //AccessType
        if(strcmp(argv[1], "r") == 0)
        {
            m_accessType = ReadAccess;
        }
        else if (strcmp(argv[1], "w") == 0)
        {
            m_accessType = WriteAccess;
        }
        else if (strcmp(argv[1], "rw") == 0)
        {
            m_accessType = ReadWriteAccess;
        }
        else if (strcmp(argv[1], "wr") == 0)
        {
            m_accessType = WriteReadAccess;
        }
        else if (strcmp(argv[1], "cwrc") == 0)
        {
            m_accessType = CyclicWriteThenReadAccess;
        }
        else
        {
            return false;
        }

        //Address
        m_address = strtol(argv[2], NULL, 0);

        //Size
        m_accessSize = strtol(argv[3], NULL, 0);

        if(m_accessType != ReadAccess)
        {
            if(argc >= 5)
            {
                uint16_t startValue = 0;
                DataGeneration dataType;
                if(strcmp(argv[4], "p") == 0)
                {
                    dataType = DataPattern;
                    m_dataToWrite = new uint16_t[m_accessSize];
                    for(int i=0; i<m_accessSize; i++)
                    {
                        if(i & 0x01)
                        {
                            m_dataToWrite[i] = 0x5555;
                        }
                        else
                        {
                            m_dataToWrite[i] = 0xAAAA;
                        }
                    }
                }
                else if (strcmp(argv[4], "i") == 0)
                {
                    dataType = DataIncrement;
                }
                else if (strcmp(argv[4], "d") == 0)
                {
                    dataType = DataDecrement;
                }
                else
                {
                    return false;
                }

                if(dataType != DataPattern)
                {
                    if(argc >= 6)
                    {
                        startValue = strtol(argv[5], NULL, 0);
                    }
                    else if(dataType == DataIncrement)
                    {
                        startValue = 0;
                    }
                    else if(dataType == DataDecrement)
                    {
                        startValue = 0xFFFF;
                    }

                    if(dataType == DataIncrement)
                    {
                        m_dataToWrite = new uint16_t[m_accessSize];
                        for(int i=0; i<m_accessSize; i++)
                        {
                            m_dataToWrite[i] = startValue;
                            startValue++;
                        }
                    }
                    else
                    {
                        m_dataToWrite = new uint16_t[m_accessSize];
                        for(int i=0; i<m_accessSize; i++)
                        {
                            m_dataToWrite[i] = startValue;
                            startValue--;
                        }
                    }
                }
            }
            else
            {
                return false;
            }
        }

        if(m_accessType != WriteAccess)
        {
            m_dataToRead = new uint16_t[m_accessSize];
        }
    }
    else
    {
            return false;
    }

    return true;
}

void VpbusTest::doRead()
{
    struct timespec start, stop, result;
    std::cout << "----------------------" << std::endl;
    std::cout << "Reading " << m_accessSize << " blocks of 2o at word address 0x" << std::hex << m_address << std::dec << std::endl;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(m_device, m_address*2, SEEK_SET);
    std::fread(m_dataToRead, 2, m_accessSize, m_device);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    timespec_diff(&start, &stop, &result);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms" << std::endl;

    for(int i = 0; i < m_accessSize; i++)
    {
        std::cout << std::hex << "0x" << m_address+i << ": 0x" << m_dataToRead[i] << std::dec << std::endl;
    }
}

void VpbusTest::doWrite()
{
    struct timespec start, stop, result;
    std::cout << std::endl << "----------------------" << std::endl;
    std::cout << "Writing " << m_accessSize << " blocks of 2o at word address 0x" << std::hex << m_address << std::dec << std::endl;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    std::fseek(m_device, m_address*2, SEEK_SET);
    std::fwrite(m_dataToWrite, 2, m_accessSize, m_device);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    timespec_diff(&start, &stop, &result);
    std::cout << "Operation took " << (double)result.tv_sec*1000 + (double)result.tv_nsec / 1000000.0 << " ms" << std::endl;

    for(int i = 0; i < m_accessSize; i++)
    {
        std::cout << std::hex << "0x" << m_address+i << ": 0x" << m_dataToWrite[i] << std::dec << std::endl;
    }
}

void VpbusTest::doCyclicWriteThenRead()
{
    uint32_t accessCount = 0;
    uint32_t accessErrorCount = 0;

    std::cout << "Press any key to stop cyclic run." << std::endl;
    std::cout << "Running..." << std::endl;

    fd_set fds;
    int maxfd;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    maxfd = 0;

    while(1)
    {
        //Write
        std::fseek(m_device, m_address*2, SEEK_SET);
        std::fwrite(m_dataToWrite, 2, m_accessSize, m_device);

        //Read
        std::fseek(m_device, m_address*2, SEEK_SET);
        std::fread(m_dataToRead, 2, m_accessSize, m_device);

        if(memcmp(m_dataToWrite, m_dataToRead, m_accessSize*2) != 0)
        {
            accessErrorCount++;
            std::cout << "Access error !" << std::endl;
        }

        accessCount++;

        FD_ZERO(&fds);
        FD_SET(0, &fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        select(maxfd+1, &fds, NULL, NULL, &timeout);


        if(FD_ISSET(0, &fds))
        {
            std::cout << "Key pressed! Stopping." << std::endl;
            // Ignore to the end of line
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }

    std::cout << "Access stats : " << accessErrorCount << " errors over " << accessCount << " access total" << std::endl;
}



