#pragma once
#include <stdint.h>

typedef enum { TT_REF=0, TT_VAL=1, TT_IFACE=2, TT_ENUM=3 } TypeKind;

typedef struct {
  const char* name;
  TypeKind kind;
  uint32_t parent;
  uint32_t size;
  uint32_t method_start, method_count;
} TypeEntry;

typedef struct {
  TypeEntry* rows;
  uint32_t count, cap;
} TypeTable;
