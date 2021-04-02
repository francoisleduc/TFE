#ifndef LOG_H
#define LOG_H

/**
 * \fn void log_fatal(char *message, const char *file, const char *funct, int line)
 * \brief Prints fatal log.
 *
 * \param message The core message.
 * \param file __FILE__ macro.
 * \param funct __func__ macro.
 * \param line __LINE__ macro.
 *
 * \return Void.
 *
 */
void log_fatal(char *message, const char *file, const char *funct, int line);

/**
 * \fn void log_error(char *message, const char *funct, int line)
 * \brief Prints error log.
 *
 * \param message The core message.
 * \param funct __func__ macro.
 * \param line __LINE__ macro.
 *
 * \return Void.
 *
 */
void log_error(char *message, const char *funct, int line);

/**
 * \fn void log_warn(char *message)
 * \brief Prints warning log.
 *
 * \param message The core message.
 *
 * \return Void.
 *
 */
void log_warn(char *message);

/**
 * \fn void log_info(char *message)
 * \brief Prints information log.
 *
 * \param message The core message.
 *
 * \return Void.
 *
 */
void log_info(char *message);

#endif
