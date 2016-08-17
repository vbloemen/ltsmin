/**
 *
 */

#include <hre/config.h>

#include <ltsmin-lib/ltsmin-standard.h>
#include <mc-lib/unionfind.h>
#include <pins-lib/pins-util.h>
#include <pins-lib/pins.h>
#include <pins2lts-mc/algorithm/ltl.h>
#include <pins2lts-mc/algorithm/algorithm.h>
#include <pins2lts-mc/algorithm/util.h>
#include <pins2lts-mc/parallel/permute.h>
#include <pins2lts-mc/parallel/state-info.h>
#include <pins2lts-mc/parallel/worker.h>
#include <util-lib/fast_set.h>

#include <ltsmin-lib/etf-util.h>


// maximum number of nodes allowed in the DOT
#define MAX_NODES 100


/**
 * local info
 */
struct alg_local_s {
    ref_t   from_arr[MAX_NODES];
    ref_t   to_arr[MAX_NODES];
    ref_t   visited[MAX_NODES];
    int     max_index;
    int     max_visit;
};


void
printdot_global_init (run_t *run, wctx_t *ctx)
{
    (void) run; (void) ctx;
}


void
printdot_global_deinit (run_t *run, wctx_t *ctx)
{
    (void) run; (void) ctx;
}


void
printdot_local_init (run_t *run, wctx_t *ctx)
{
    ctx->local = RTmallocZero (sizeof (alg_local_t) );
    ctx->local->max_index = 0;
    ctx->local->max_visit = 0;
    (void) run; 
}


void
printdot_local_deinit   (run_t *run, wctx_t *ctx)
{
    alg_local_t        *loc = ctx->local;
    RTfree (loc);
    (void) run;
}

int
have_visited(wctx_t *ctx, ref_t node) 
{
    alg_local_t        *loc       = ctx->local;
    for (int i=0; i<loc->max_visit; i++) {
        if (node == loc->visited[i])
            return 1;
    }
    return 0;
}

static void
printdot_handle (void *arg, state_info_t *successor, transition_info_t *ti,
              int seen)
{
    wctx_t             *ctx       = (wctx_t *) arg;
    alg_local_t        *loc       = ctx->local;
    uint32_t            acc_set   = 0;

    printf("EDGE %zu %zu\n", ctx->state->ref, successor->ref);

    if (loc->max_index < MAX_NODES) {
        loc->from_arr[loc->max_index] = ctx->state->ref; 
        loc->to_arr[loc->max_index]   = successor->ref; 
        loc->max_index ++;
    }

    (void) ti; (void) seen;
}


static inline size_t
explore_state (wctx_t *ctx)
{
    alg_local_t        *loc       = ctx->local;

    loc->visited[loc->max_visit++] = ctx->state->ref;

    permute_trans (ctx->permute, ctx->state, printdot_handle, ctx);

}


static void
printdot_init  (wctx_t *ctx)
{
    transition_info_t   ti     = GB_NO_TRANSITION;

    // handle the initial state
    ctx->state->ref = ctx->initial->ref;
    printdot_handle (ctx, ctx->initial, &ti, 0);
}


void
printdot_run  (run_t *run, wctx_t *ctx)
{
    alg_local_t        *loc       = ctx->local;

    // only run sequentially
    if (ctx->id == 0) {
        printf("\n\nSTART\n");
        printdot_init (ctx);

        for (int i=0; i<100; i++) {
            if (i > loc->max_index) {
                break;
            }

            if (!have_visited(ctx,loc->to_arr[i])) {
                state_info_set (ctx->state, loc->to_arr[i], LM_NULL_LATTICE);
                explore_state (ctx);
            }
        }

        /*// print transitions and acceptance
        printf("###\n");
        printf("---  INIT -> %zu\n", loc->to_arr[0]);
        for (int i=1; i<loc->max_index; i++) {

            state_info_set (ctx->state, loc->from_arr[i], LM_NULL_LATTICE);
            int acc1 = pins_state_is_accepting(ctx->model, state_info_state(ctx->state));

            state_info_set (ctx->state, loc->to_arr[i], LM_NULL_LATTICE);
            int acc2 = pins_state_is_accepting(ctx->model, state_info_state(ctx->state));

            printf("--- %zu (%d) -> %zu (%d)\n", loc->from_arr[i], acc1, loc->to_arr[i], acc2);
        }
        printf("###\n");*/


    }

    (void) run;
}


void
printdot_reduce (run_t *run, wctx_t *ctx)
{
    // we don't need this 
    (void) run; (void) ctx;
}


void
printdot_print_stats   (run_t *run, wctx_t *ctx)
{

    printf("\n\nDONE\n\n");
    fflush(stdout);

    run_report_total (run);

    (void) ctx;
}


void
printdot_shared_init   (run_t *run)
{
    set_alg_local_init    (run->alg, printdot_local_init);
    set_alg_global_init   (run->alg, printdot_global_init);
    set_alg_global_deinit (run->alg, printdot_global_deinit); 
    set_alg_local_deinit  (run->alg, printdot_local_deinit);
    set_alg_print_stats   (run->alg, printdot_print_stats);
    set_alg_run           (run->alg, printdot_run);
    set_alg_reduce        (run->alg, printdot_reduce);
}
