/*!
 * Copyright © 2018 Virtu'Ailes
 * Marc Laval
 */

#ifndef VPBUS_TEST_H
#define VPBUS_TEST_H

#include <iostream>
#include <cstdio>
#include <cstdlib>

class VpbusTest
{
public:
    enum BusAccessType
    {
        ReadAccess,
        WriteAccess,
        ReadWriteAccess,
        WriteReadAccess,
        CyclicWriteThenReadAccess
    };

    enum DataGeneration
    {
        DataPattern,
        DataIncrement,
        DataDecrement
    };

    VpbusTest();
    ~VpbusTest();

    int run(int argc, const char* argv[]);
    bool parseArgs(int argc, const char* argv[]);
    void doRead();
    void doWrite();
    void doCyclicWriteThenRead();

private:
    BusAccessType m_accessType;
    uint32_t m_address;
    uint32_t m_accessSize;
    uint16_t* m_dataToWrite;
    uint16_t* m_dataToRead;
    FILE* m_device;
};

#endif
