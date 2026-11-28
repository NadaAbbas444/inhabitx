#pragma once
// The exported header uses C99 restrict and has no C++ linkage guards.
extern "C" {
#define restrict __restrict__
#include "src/model/model.h"
int ClassroomModel_allocate(void);
#undef restrict
}
static_assert(IMAI_DATA_IN_COUNT == 1, "Expected streaming mono PCM input");
static_assert(IMAI_DATA_OUT_COUNT == 3, "Check model class order before replacing model");
