#include "TestManager.h"

#include <containers/DArray.h>
#include <core/Logger.h>
#include <core/CString.h>
#include <core/Clock.h>

typedef struct TestEntry
{
    PFN_test func;
    char* desc;
} TestEntry;

static TestEntry* tests;

void TestManagerInit() 
{
    tests = DArrayCreate(TestEntry);
}

void TestManagerRegisterTest(u8 (*PFN_test)(), char* _desc) 
{
    TestEntry e;
    e.func = PFN_test;
    e.desc = _desc;
    DArrayPush(tests, e);
}

void TestManagerRunTests() 
{
    u32 passed = 0;
    u32 failed = 0;
    u32 skipped = 0;

    u32 count = DArrayLength(tests);

    Clock totalTime;
    ClockStart(&totalTime);

    for(u32 i = 0; i < count; ++i)
    {
        Clock testTime;
        ClockStart(&testTime);
        u8 result = tests[i].func();
        ClockUpdate(&testTime);

        if(result == true)
            ++passed;
        else if(result == BYPASS)
        {
            LOG_WARN("[SKIPPED]: %s", tests[i].desc);
            ++skipped;
        }
        else
        {
            LOG_WARN("[FAILED]: %s", tests[i].desc);
            ++failed;
        }

        char status[20];
        StringFormat(status, failed ? "*** %d FAILED ***" : "SUCCESS", failed);
        ClockUpdate(&totalTime);
        LOG_INFO("Executed %d of %d (skipped %d) %s (%.6f sec / %.6f sec total)", i + 1, count, skipped, status, testTime.elapsed, totalTime.elapsed);
    }

    ClockStop(&totalTime);

    LOG_INFO("Results: %d passed, %d failed, %d skipped.", passed, failed, skipped);
}