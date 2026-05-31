#pragma once

#include <cstdio>

#define LOG_INFO(...)   do { fprintf(stdout, "[INFO] " __VA_ARGS__); fputc('\n', stdout); } while(0)
#define LOG_WARN(...)   do { fprintf(stderr, "[WARN] " __VA_ARGS__); fputc('\n', stderr); } while(0)
#define LOG_ERROR(...)  do { fprintf(stderr, "[ERR]  " __VA_ARGS__); fputc('\n', stderr); } while(0)
