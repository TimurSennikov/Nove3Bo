#define LOGLEVEL 4

#define LOG_ERROR_LVL 1
#define LOG_WARNING_LVL 2
#define LOG_LOGS_LVL 3
#define LOG_FPS_LVL 4

#define LOG_ERROR LOG_ERROR_LVL <= LOGLEVEL
#define LOG_WARNING LOG_WARNING_LVL <= LOGLEVEL
#define LOG_LOGS LOG_LOGS_LVL <= LOGLEVEL
#define LOG_FPS LOG_LOGS_LVL <= LOGLEVEL

bool freeToLog(int level){return level <= LOGLEVEL;}
