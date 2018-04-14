#ifndef __ERROR_H__
#define __ERROR_H__

#define xstr(s) str(s)
#define str(s) #s

#define MSG(s) \
    puts(__FILE__ ", L" xstr(__LINE__) ": " s "\n");

#define MSG_WITH_ARGS(s, ...) \
  printf(__FILE__ ", L" xstr(__LINE__) ": " s "\n", __VA_ARGS__);

#define ERROR(s) \
    MSG(s); \
    raise(3); \
    exit(1);

#define ERROR_WITH_ARGS(s, ...) \
    MSG_WITH_ARGS(s, __VA_ARGS__); \
    raise(3); \
    exit(1);

#define ASSERT(c) \
    if (!(c)) { \
        ERROR("failed: ASSERT(" xstr(c) ")") \
    }

#define CHECK_SDL_RET(call) \
    ({ \
        int ret = call; \
        if (ret < 0) { \
            ERROR_WITH_ARGS("SDL error \"%s\"", SDL_GetError()); \
        } \
        ret; \
    })

#define ASSERT_SDL(c) \
    if (!(c)) { \
        ERROR_WITH_ARGS("failed: ASSERT_SDL(" xstr(c) "), SDL_GetError()=%s", SDL_GetError()) \
    }

#endif
