
#ifndef MCLOG_H
#define MCLOG_H

#include <mc-lib/atomics.h>
#include <util-lib/util.h>

typedef enum section_id_s
{
    UNDEF,
    MCLOG_INIT,
    INIT_START, INIT_END,
    SUCCESSOR_START, SUCCESSOR_END,
    BACKTRACK_START, BACKTRACK_END,
    HANDLE_SUC_START, HANDLE_SUC_END,
    PICK_FROM_LIST_START, PICK_FROM_LIST_END,
    PICK_FROM_LIST_END_ALIVE, PICK_FROM_LIST_END_BLIVE, PICK_FROM_LIST_END_DEAD,
    REMOVE_FROM_LIST_START, REMOVE_FROM_LIST_END,
    REMOVE_FROM_LIST_END_LIVE, REMOVE_FROM_LIST_END_DEAD,
    MAKE_CLAIM_START, MAKE_CLAIM_END,
    MAKE_CLAIM_END_DEAD, MAKE_CLAIM_END_VIS, MAKE_CLAIM_END_ADD,
    FIND_START, FIND_END,
    SAMESET_START, SAMESET_END,
    SAMESET_END_TRUE, SAMESET_END_FALSE, SAMESET_END_RETRY,
    UNION_START, UNION_END, UNION_END_EQ, UNION_END_LOCKFAIL, UNION_END_SUCCESS,
    MARK_DEAD_START, MARK_DEAD_END,
    LOCK_UF_START, LOCK_UF_END, LOCK_UF_END_SUCCESS, LOCK_UF_END_FAIL,
    LOCK_LIST_START, LOCK_LIST_END, LOCK_LIST_END_SUCCESS, LOCK_LIST_END_FAIL,
} section_id;

inline char *section_id_str (enum section_id_s id)
{
    switch (id) {
        case MCLOG_INIT: return "MCLOG_INIT";
        case INIT_START: return "INIT_START";
        case INIT_END: return "INIT_END";
        case SUCCESSOR_START: return "SUCCESSOR_START";
        case SUCCESSOR_END: return "SUCCESSOR_END";
        case BACKTRACK_START: return "BACKTRACK_START";
        case BACKTRACK_END: return "BACKTRACK_END";
        case HANDLE_SUC_START: return "HANDLE_SUC_START";
        case HANDLE_SUC_END: return "HANDLE_SUC_END";
        case PICK_FROM_LIST_START: return "PICK_FROM_LIST_START";
        case PICK_FROM_LIST_END: return "PICK_FROM_LIST_END";
        case PICK_FROM_LIST_END_ALIVE: return "PICK_FROM_LIST_END_ALIVE";
        case PICK_FROM_LIST_END_BLIVE: return "PICK_FROM_LIST_END_BLIVE";
        case PICK_FROM_LIST_END_DEAD: return "PICK_FROM_LIST_END_DEAD";
        case REMOVE_FROM_LIST_START: return "REMOVE_FROM_LIST_START";
        case REMOVE_FROM_LIST_END: return "REMOVE_FROM_LIST_END";
        case REMOVE_FROM_LIST_END_LIVE: return "REMOVE_FROM_LIST_END_LIVE";
        case REMOVE_FROM_LIST_END_DEAD: return "REMOVE_FROM_LIST_END_DEAD";
        case MAKE_CLAIM_START: return "MAKE_CLAIM_START";
        case MAKE_CLAIM_END: return "MAKE_CLAIM_END";
        case MAKE_CLAIM_END_DEAD: return "MAKE_CLAIM_END_DEAD";
        case MAKE_CLAIM_END_VIS: return "MAKE_CLAIM_END_VIS";
        case MAKE_CLAIM_END_ADD: return "MAKE_CLAIM_END_ADD";
        case FIND_START: return "FIND_START";
        case FIND_END: return "FIND_END";
        case SAMESET_START: return "SAMESET_START";
        case SAMESET_END: return "SAMESET_END";
        case SAMESET_END_TRUE: return "SAMESET_END_TRUE";
        case SAMESET_END_FALSE: return "SAMESET_END_FALSE";
        case SAMESET_END_RETRY: return "SAMESET_END_RETRY";
        case UNION_START: return "UNION_START";
        case UNION_END: return "UNION_END";
        case UNION_END_EQ: return "UNION_END_EQ";
        case UNION_END_LOCKFAIL: return "UNION_END_LOCKFAIL";
        case UNION_END_SUCCESS: return "UNION_END_SUCCESS";
        case MARK_DEAD_START: return "MARK_DEAD_START";
        case MARK_DEAD_END: return "MARK_DEAD_END";
        case LOCK_UF_START: return "LOCK_UF_START";
        case LOCK_UF_END: return "LOCK_UF_END";
        case LOCK_UF_END_SUCCESS: return "LOCK_UF_END_SUCCESS";
        case LOCK_UF_END_FAIL: return "LOCK_UF_END_FAIL";
        case LOCK_LIST_START: return "LOCK_LIST_START";
        case LOCK_LIST_END: return "LOCK_LIST_END";
        case LOCK_LIST_END_SUCCESS: return "LOCK_LIST_END_SUCCESS";
        case LOCK_LIST_END_FAIL: return "LOCK_LIST_END_FAIL";
        default: return "UNDEF";
    }
}


extern const size_t DUMMY_REF;

typedef struct mclog_entry_s mclog_entry_t;

extern void mclog_init (int size, int threads);
extern void mclog_print ();
extern void mclog_print_stats ();
extern void mclog_print_file (const char *file);
extern void mclog_add (int thread_id, section_id sec_id, size_t state_a, size_t state_b);

#endif // MCLOG_H