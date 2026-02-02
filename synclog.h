#pragma once
#ifndef SYNCLOG_H
#define SYNCLOG_H

#include <stdio.h>

#define LOG_DBG(fmt, ...)                                                      \
    fprintf(stderr, "[DBG %s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
