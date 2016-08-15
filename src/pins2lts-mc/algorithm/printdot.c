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


// maximum number of nodes allowed in the DOT
#define MAX_NODES 100


/**
 * local info
 */
struct alg_local_s {
    ref_t   to_arr[MAX_NODES];
    int     max_index;
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
    (void) run; 
}


void
printdot_local_deinit   (run_t *run, wctx_t *ctx)
{
    alg_local_t        *loc = ctx->local;
    RTfree (loc);
    (void) run;
}


static void
printdot_handle (void *arg, state_info_t *successor, transition_info_t *ti,
              int seen)
{
    wctx_t             *ctx       = (wctx_t *) arg;
    alg_local_t        *loc       = ctx->local;
    uint32_t            acc_set   = 0;

    printf("  HANDLE: %zu -> %zu\n", ctx->state->ref, successor->ref);

    // TGBA acceptance
    if (ti->labels != NULL && PINS_BUCHI_TYPE == PINS_BUCHI_TYPE_TGBA) {
        acc_set = ti->labels[pins_get_accepting_set_edge_label_index(ctx->model)];
        if (acc_set != 0) {
            printf("    acceptance set = %d\n", acc_set);
        }
    }

    // print more detailed info :)


    if (loc->max_index < MAX_NODES) {
        loc->to_arr[loc->max_index]   = successor->ref; 
        loc->max_index ++;
    }
    else{
        printf("    ignoring transition, reached maximum!\n");
    }

    (void) ti; (void) seen;
}


static inline size_t
explore_state (wctx_t *ctx)
{
    printf("\nEXPLORE: %zu\n", ctx->state->ref);

    permute_trans (ctx->permute, ctx->state, printdot_handle, ctx);

    printf("\nDONE EXPLORING: %zu\n", ctx->state->ref);
}


static void
printdot_init  (wctx_t *ctx)
{
    transition_info_t   ti     = GB_NO_TRANSITION;

    printf("INIT\n");

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

        for (int i=0; i<10; i++) {
            if (i > loc->max_index) {
                printf("no more nodes left! :(\n");
                break;
            }

            state_info_set (ctx->state, loc->to_arr[i], LM_NULL_LATTICE);
            explore_state (ctx);
        }

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
