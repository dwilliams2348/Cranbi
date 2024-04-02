#include<core/Logger.h>
#include<core/Asserts.h>

//TODO: test
#include <platform/Platform.h>

int main(void)
{
    LOG_FATAL("A fatal test message: %f", 3.14f);
    LOG_ERROR("An error test message: %d", 42);
    LOG_WARN("A warn test message: %s", "suck ween");
    LOG_INFO("An info test message: %f", 6.9f);
    LOG_DEBUG("A debug test message: %f", 42.0f);
    LOG_TRACE("A trace test message");

    //CORE_ASSERT(1 == 0);

    PlatformState state;
    if(PlatformStartup(&state, "Cranbi Engine Sandbox", 100, 100, 1280, 720))
    {
        while(TRUE)
        {
            PlatformPumpMessages(&state);
        }
    }

    PlatformShutdown(&state);

    return 0;
}