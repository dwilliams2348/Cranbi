#include "Platform.h"

#if CPLATFORM_LINUX

#include "core/Logger.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h> //sudo apt-get install libx11-dev
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h> //sudo apt-get install libxkbcommon-x11-dev
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#   include <time.h> //nanosleep
#else
#   include <unistd.h> //usleep
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct InternalState
{
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wmProtocols;
    xcb_atom_t wmDeleteWin;
}InternalState;

b8 PlatformStartup(PlatformState* _state, const char* _appName, i32 _x, i32 _y, i32 _width, i32 _height)
{
    //create internal state
    _state->InternalState = malloc(sizeof(InternalState));
    InternalState* state = (InternalState*)_state->InternalState;

    //connect to x
    state->display = XOpenDisplay(NULL);

    //turn off key repeats
    XAutoRepeatOff(state->display);

    //retrieve conection from display
    state->connection = XGetXCBConnection(state->display);

    if(xcb_connection_has_error(state->connection))
    {
        LOG_FATAL("Failed to connect to the X server via XCB");
        return FALSE;
    }

    //get data from x server
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    //loop through screens using iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screenP = 0;
    for(i32 s = screenP; s > 0; s--)
    {
        xcb_screen_next(&it);
    }

    //after screens have been looped through assign it
    state->screen = it.data;

    //allocate XID for the window to be created
    state->window = xcb_generate_id(state->connection);

    //register event types
    //XCB_CW_BACK_PIXEL = filling window bg with a single color
    //XCB_CW_EVENT_MASK is required
    u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    //listen for keyboard and mouse events
    u32 eventValues =   XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE       |
                        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION     |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    u32 valueList[] = { state->screen->black_pixel, eventValues };

    //create the window
    xcb_void_cookie_t cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,           //depth
        state->window,
        state->screen->root,            //parent
        _x,                             //x
        _y,                             //y
        _width,                         //width
        _height,                        //height
        0,                              //no border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,  //class
        state->screen->root_visual,
        eventMask,
        valueList);

    //change title of window
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8, //data should be viewed 8 bits at a time
        strlen(_appName),
        _appName);

    //tells the server to notify when the window manager tries to destroy window
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wmDeleteReply = xcb_intern_atom_reply(
        state->connection,
        wmDeleteCookie,
        NULL);
    xcb_intern_atom_reply_t* wmProtocolsReply = xcb_intern_atom_reply(
        state->connection,
        wmProtocolsCookie,
        NULL);

    state->wmDeleteWin = wmDeleteReply->atom;
    state->wmProtocols = wmProtocolsReply->atom;

    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wmProtocolsReply->atom,
        4,
        32,
        1,
        &wmDeleteReply->atom);

    //map window to screen
    xcb_map_window(state->connection, state->window);

    //flush the stream
    i32 streamRes = xcb_flush(state->connection);
    if(streamRes <= 0)
    {
        LOG_FATAL("An error occured when flushing the stream: %d", streamRes);
        return FALSE;
    }

    return TRUE;
}

void PlatformShutdown(PlatformState* _state)
{
    //simply cast internal state
    InternalState* state = (InternalState*)_state->InternalState;

    //turn key repeats back on since its global on the OS
    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection, state->window);
}

b8 PlatformPumpMessages(PlatformState* _state)
{
    //simply cast to state
    InternalState* state = (InternalState*)_state->InternalState;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;

    b8 quitFlagged = FALSE;

    //poll for events until null is returned
    while(event != 0)
    {
        event = xcb_poll_for_event(state->connection);
        if(event == 0)
            break;

        switch(event->response_type & ~0x80)
        {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            {
                //TODO: key presses and releases
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
            {
                //TODO: mouse button presses and releases
            } break;
            case XCB_MOTION_NOTIFY:
            {
                //TODO: mouse movement
            } break;
            case XCB_CONFIGURE_NOTIFY:
            {
                //TODO: resizing
            } break;
            case XCB_CLIENT_MESSAGE:
            {
                cm = (xcb_client_message_event_t*)event;

                //window close
                if(cm->data.data32[0] == state->wmDeleteWin)
                {
                    quitFlagged = TRUE;
                }
            } break;
            default:
                //something else
                break;
        }

        free(event);
    }

    return !quitFlagged;
}

void* PlatformAllocate(u64 _size, b8 _aligned)
{
    return malloc(_size);
}

void PlatformFree(void* _block, b8 _aligned)
{
    free(_block);
}

void* PlatformZeroMem(void* _block, u64 _size)
{
    return memset(_block, 0, _size);
}

void* PlatformCopyMem(void* _dest, const void* _src, u64 _size)
{
    return memcpy(_dest, _src, _size);
}

void* PlatformSetMem(void* _dest, i32 _value, u64 _size)
{
    return memset(_dest, _value, _size);
}

void PlatformConsoleWrite(const char* _msg, u8 _color)
{
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colorStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
    printf("\033[%sm%s\033[0m", colorStrings[_color], _msg);
}

void PlatformConsoleWriteError(const char* _msg, u8 _color)
{
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colorStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
    printf("\033[%sm%s\033[0m", colorStrings[_color], _msg);
}

f64 PlatformGetAbsoluteTime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void PlatformSleep(u64 _ms)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = _ms / 1000;
    ts.tv_nsec = (_ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if(_ms >= 1000)
    {
        sleep(_ms / 1000);
    }

    usleep((_ms % 1000) * 1000);
#endif
}

#endif