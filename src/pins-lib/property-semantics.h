/*
 * property-semantics.c
 *
 *  Created on: Aug 8, 2012
 *      Author: laarman
 */

#ifndef PROPERTY_SEMANTICS_H
#define PROPERTY_SEMANTICS_H

#include <limits.h>

#include <ltsmin-lib/ltsmin-tl.h>
#include <ltsmin-lib/ltsmin-parse-env.h>
#include <ltsmin-lib/ltsmin-standard.h>
#include <pins-lib/pins.h>
#include <pins-lib/pins-util.h>
#include <util-lib/util.h>

/* set visibility in PINS and dependencies in deps (if not NULL) */
extern void set_pins_semantics(model_t model, ltsmin_expr_t e, ltsmin_parse_env_t env, bitvector_t *deps, bitvector_t *sl_deps);

extern long eval_predicate(model_t model, ltsmin_expr_t e, int *state, ltsmin_parse_env_t env);

/**
 * evaluate predicate on state
 * NUM values may be looked up in the chunk tables (for chunk types), hence the
 * conditional.
 */
static inline int
eval_predicate_ltl(model_t model, ltsmin_expr_t e, int *state,
               int N, ltsmin_parse_env_t env)
{
    switch (e->token) {
        case PRED_TRUE:
            return 1;
        case PRED_FALSE:
            return 0;
        case PRED_NUM:
            return e->idx;
        case PRED_SVAR:
            if (e->idx < N) { // state variable
                return state[e->idx];
            } else { // state label
                return GBgetStateLabelLong(model, e->idx - N, state);
            }
        case PRED_CHUNK:
        case PRED_VAR: {
            chunk c;
            c.data = SIgetC(env->values, e->idx, (int*) &c.len);
            return pins_chunk_put(model, ltsmin_expr_type_check(LTSminExprSibling(e), env, GBgetLTStype(model)), c);
        }
        case PRED_NOT:
            return !eval_predicate_ltl(model, e->arg1, state, N, env);
        case PRED_EQ:
            return (eval_predicate_ltl(model, e->arg1, state, N, env) ==
                    eval_predicate_ltl(model, e->arg2, state, N, env));
        case PRED_AND:
            return (eval_predicate_ltl(model, e->arg1, state, N, env) &&
                    eval_predicate_ltl(model, e->arg2, state, N, env));
        case PRED_OR:
            return (eval_predicate_ltl(model, e->arg1, state, N, env) ||
                    eval_predicate_ltl(model, e->arg2, state, N, env));
        case PRED_IMPLY:
            return ((!eval_predicate_ltl(model, e->arg1, state, N, env)) ||
                      eval_predicate_ltl(model, e->arg2, state, N, env));
        case PRED_EQUIV:
            return ((!eval_predicate_ltl(model, e->arg1, state, N, env)) ==
                    (!eval_predicate_ltl(model, e->arg2, state, N, env)) );
        default:
            LTSminLogExpr (error, "Unhandled predicate expression: ", e, env);
            HREabort (LTSMIN_EXIT_FAILURE);
    }
    return 0;
}


#endif // PROPERTY_SEMANTICS_H
