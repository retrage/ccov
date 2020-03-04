/* SPDX-License-Identifier: MIT */

#ifndef CCOV_CCOV_H_
#define CCOV_CCOV_H_

#include <stdio.h>

#define SIG "#CCOV"

enum ccov_attribute {
  ccov_entry = 0x01,
  ccov_ret = 0x02,
};

void __log_coverage(const char *file, const char *func, const int line, const int attr)
{
  printf("%s:%s:%s:%d", SIG, file, func, line);
  if (attr & ccov_entry)
    printf(":entry");
  if (attr & ccov_ret)
    printf(":ret");
  printf("\n");
}

#endif /* CCOV_CCOV_H_ */
