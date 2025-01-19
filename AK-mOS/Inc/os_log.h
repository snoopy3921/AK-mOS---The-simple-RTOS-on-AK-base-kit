#ifndef OS_LOG_H
#define OS_LOG_H

#include <stdio.h>
#include "os_cfg.h"

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define LOG_DBG_EN      (1u)
#define LOG_WARN_EN     (1u)
#define LOG_PRINT_EN    (1u)
#define LOG_ERROR_EN    (1u)
#define LOG_ASSERT_EN   (1u)
#define LOG_SIG_EN      (1u)

#if ( LOG_DBG_EN & OS_CFG_USE_LOG ) == 1
#define LOG_DBG(fmt, ...) USER_PRINT(KBLU "[DEBUG: %s:%d] " KYEL fmt KNRM "\r\n", (uint8_t *)__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...) ((void*)0)
#endif

#if ( LOG_WARN_EN & OS_CFG_USE_LOG ) == 1
#define LOG_WARN(fmt, ...) USER_PRINT(KYEL "[WARN: %s:%d] " KNRM fmt KNRM "\r\n", (uint8_t *)__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...) ((void*)0)
#endif

#if ( LOG_PRINT_EN & OS_CFG_USE_LOG ) == 1
#define LOG_PRINT(fmt, ...) USER_PRINT("[PRINTLN] " fmt "\r\n", ##__VA_ARGS__)
#else
#define LOG_PRINT(fmt, ...) ((void*)0)
#endif

#if ( LOG_ERROR_EN & OS_CFG_USE_LOG ) == 1
#define LOG_ERROR(fmt, ...) USER_PRINT(KRED "[ERROR: %s:%d] " fmt KNRM "\r\n", (uint8_t *)__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void*)0)
#endif

#if ( LOG_ASSERT_EN & OS_CFG_USE_LOG ) == 1
#define LOG_ASSERT(fmt, ...) USER_PRINT(KRED "[ASSERT FAILED: %s:%d] " fmt KNRM "\r\n", (uint8_t *)__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_ASSERT(fmt, ...) ((void*)0)
#endif

#if ( LOG_SIG_EN & OS_CFG_USE_LOG ) == 1
#define LOG_SIG(fmt, ...) USER_PRINT(KGRN "[SIGNAL: %s:%d] " KMAG fmt KNRM "\r\n", (uint8_t *)__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_SIG(fmt, ...) ((void*)0)
#endif

#endif /* OS_LOG_H */