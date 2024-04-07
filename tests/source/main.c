#include "TestManager.h"

#include "memory/LinearAllocatorTests.h"

#include "core/Logger.h"

int main()
{
    //allways intialize test manager
    TestManagerInit();

    //TODO: add registrations here
    LinearAllocatorRegisterTests();

    LOG_DEBUG("Starting tests...");

    //execute tests
    TestManagerRunTests();

    return 0;
}