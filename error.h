#ifndef __ERROR_H__
#define __ERROR_H__

#define xstr(s) str(s)
#define str(s) #s

#define MSG(s) \
    puts(__FILE__ ", L" xstr(__LINE__) ": " s "\n");

#define ERROR(s) \
    MSG(s); \
    raise(3); \
    exit(1);

#define ASSERT(c) \
    if (!(c)) { \
        ERROR("failed: ASSERT(" xstr(c) ")") \
    }

#endif
