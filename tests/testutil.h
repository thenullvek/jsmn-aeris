#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include "../jsmn.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

static int vtokeq(const char *s, jsmntok_t *t, unsigned long numtok,
                  va_list ap) {
  if (numtok > 0) {
    unsigned long i;
    size_t start;
    size_t size;
    jsmntype_t type;
    char *value;

    size = -1;
    value = NULL;
    for (i = 0; i < numtok; i++) {
      type = va_arg(ap, jsmntype_t);
      if (type == JSMN_STRING) {
        value = va_arg(ap, char *);
        size = va_arg(ap, int);
        start = -1;
      } else if (type == JSMN_PRIMITIVE) {
        value = va_arg(ap, char *);
        start = -1;
        size = t[i].size;
      } else {
        start = va_arg(ap, int);
        size = va_arg(ap, int);
        value = NULL;
      }
      if (t[i].type != type) {
        printf("token %lu type is %d, not %d\n", i, t[i].type, type);
        return 0;
      }
      if (start != -1) {
        if (t[i].start != start) {
          printf("token %lu start is %zu, not %zu\n", i, t[i].start, start);
          return 0;
        }
      }
      if (size != -1 && t[i].size != size) {
        printf("token %lu size is %d, not %zu\n", i, t[i].size, size);
        return 0;
      }

      if (s != NULL && value != NULL) {
        const char *p = s + t[i].start;
        if (strlen(value) != size || strncmp(p, value, t[i].size) != 0) {
          printf("token %lu value is %.*s, not %s\n", i, t[i].size,
                 s + t[i].start, value);
          return 0;
        }
      }
    }
  }
  return 1;
}

static int tokeq(const char *s, jsmntok_t *tokens, unsigned long numtok, ...) {
  int ok;
  va_list args;
  va_start(args, numtok);
  ok = vtokeq(s, tokens, numtok, args);
  va_end(args);
  return ok;
}

static int parse(const char *s, enum jsmnerr err, unsigned long numtok, ...) {
  enum jsmnerr r;
  int ok = 1;
  va_list args;
  jsmn_parser p;
  jsmntok_t *t = malloc(numtok * sizeof(jsmntok_t));

  jsmn_init(&p);
  r = jsmn_parse(&p, s, strlen(s), t, numtok);
  if (r != err) {
    printf("return code is %d, not %d\n", r, err);
    return 0;
  }
  if (r == JSMN_SUCCESS) {
    if (p.toknext != numtok) {
      printf("token count is %d, not %lu\n", p.toknext, numtok);
    }
    va_start(args, numtok);
    ok = vtokeq(s, t, numtok, args);
    va_end(args);
  }
  free(t);
  return ok;
}

#endif /* __TEST_UTIL_H__ */
