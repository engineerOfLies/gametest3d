#ifndef __SIMPLE_LOGGER__
#define __SIMPLE_LOGGER__

/**
  @brief initializes the simple logger.  Will automatically cleanup at program exit.

  @param log_file_path the file to log to
*/
void init_logger(const char *log_file_path);

/**
  @brief logs a message to stdout and to the configured log file
  @param msg a string with tokens
  @param ... variables to be put into the tokens.
*/
#define slog(...) _slog(__FILE__,__LINE__,__VA_ARGS__)
void _slog(char *f,int l,char *msg,...);


#endif

