#include <stdio.h>

#include "log.h"

#define LOG_FATAL "[FATAL]" /*!< Fatal header message. */
#define LOG_ERROR "[ERROR]" /*!< Error header message. */
#define LOG_WARN "[WARNING]" /*!< Warning header message. */
#define LOG_INFO "[INFO]" /*!< Info header message. */

void log_fatal(char *message, const char *file, const char *funct, int line)
{
    printf("%s at %s <%s+%d>: %s\n", LOG_ERROR, file, funct, line, message);
}

void log_error(char *message, const char *funct, int line)
{
    printf("%s <%s+%d>: %s\n", LOG_ERROR, funct, line, message);
}

void log_warn(char *message)
{
    printf("%s %s\n", LOG_WARN, message);
}

void log_info(char *message)
{
    printf("%s %s", LOG_INFO, message);
}
