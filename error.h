#ifndef __ERROR_H__
#define __ERROR_H__

#include <signal.h>
#include <stdlib.h>

#define XSTR(S) STR(S)
#define STR(S) #S

#define MSG(S) \
    puts(__FILE__ ", L" XSTR(__LINE__) ": " S "\n");

#define MSG_WITH_ARGS(S, ...) \
    printf(__FILE__ ", L" XSTR(__LINE__) ": " S "\n", __VA_ARGS__);

#define ERROR(S) \
    MSG(S); \
    raise(3); \
    exit(1);

#define ERROR_WITH_ARGS(S, ...) \
    MSG_WITH_ARGS(S, __VA_ARGS__); \
    raise(3); \
    exit(1);

#define ASSERT(COND) \
    if (!(COND)) { \
        ERROR("failed: ASSERT(" XSTR(COND) ")") \
    }

#define CHECK_SDL_RET(CALL) \
    ({ \
        int ret = CALL; \
        if (ret < 0) { \
            ERROR_WITH_ARGS("SDL error \"%s\"", SDL_GetError()); \
        } \
        ret; \
    })

#define ASSERT_SDL(COND) \
    if (!(COND)) { \
        ERROR_WITH_ARGS("failed: ASSERT_SDL(" XSTR(COND) "), SDL_GetError()=%s", SDL_GetError()) \
    }

#endif
