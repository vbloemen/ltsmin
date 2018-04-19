
/**
 * C wrapper for the C++ file hoa2aut.cpp
 */
#ifndef HOA2AUT_H
#define HOA2AUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ltsmin-lib/ltsmin-buchi.h>
#include <pins-lib/pins2pins-ltl.h>

ltsmin_buchi_t* ltsmin_create_hoa(const char *hoa_file, ltsmin_parse_env_t env, lts_type_t ltstype);

#ifdef __cplusplus
}
#endif

#endif // HOA2AUT_H
