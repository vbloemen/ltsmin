/**
 * C wrapper for the C++ file ltl2spot.cpp
 */
#ifndef LTL2SPOT_H
#define LTL2SPOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ltsmin-lib/ltsmin-buchi.h>
#include <pins-lib/pins2pins-ltl.h>

void ltsmin_ltl2spot(ltsmin_expr_t e, ltsmin_parse_env_t env);
ltsmin_buchi_t* ltsmin_hoa_buchi(ltsmin_parse_env_t env);
void ltsmin_hoa_destroy();
ltsmin_buchi_t* ltsmin_create_hoa(const char *hoa_file, ltsmin_parse_env_t env, lts_type_t ltstype);
char* ltl_to_store (ltsmin_expr_t e, ltsmin_parse_env_t env);

#ifdef __cplusplus
}
#endif

#endif // LTL2SPOT_H
