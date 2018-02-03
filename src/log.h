#ifndef _H_LOG
#define _H_LOG

enum {
    LOG_T_VERB = 0,
    LOG_T_DEBUG,
    LOG_T_INFO,
    LOG_T_WARN,
    LOG_T_ERROR,
    LOG_T_FATAL,
};

#define LOG(...) LOG_T(LOG_T_VERB,   __VA_ARGS__)
#define LOGV(...) LOG_T(LOG_T_VERB,  __VA_ARGS__)
#define LOGD(...) LOG_T(LOG_T_DEBUG, __VA_ARGS__)
#define LOGI(...) LOG_T(LOG_T_INFO,  __VA_ARGS__)
#define LOGW(...) LOG_T(LOG_T_WARN,  __VA_ARGS__)
#define LOGE(...) LOG_T(LOG_T_ERROR, __VA_ARGS__)
#define LOGF(...) LOG_T(LOG_T_FATAL, __VA_ARGS__)
void LOG_T( unsigned int type, const char *fmt, ... );

#endif
