
#include <hre/config.h>
#include <mc-lib/mclog.h>

#include <stdio.h>
#include <time.h>
#include <limits.h>

struct mclog_entry_s
{
    int thread_id;
    long long time; // needs to be a 64 bit number
    section_id sec_id;
    size_t state_a;
    size_t state_b;
};

const size_t DUMMY_REF = -1;
int *mclog_counters; // sizes of the logs, until now
mclog_entry_t **mclog;
int mclog_max_size;
int mclog_threads;

void mclog_init (int size, int threads)
{
    printf("Starting log\n");

    mclog_counters = (int *) RTmallocZero (sizeof(int) * threads);
    mclog          = (mclog_entry_t **)
        RTmalloc (sizeof(mclog_entry_t *) * threads);
    for (int i=0; i<threads; i++)
        mclog[i] = RTmalloc (sizeof(mclog_entry_t) * size);
    mclog_max_size = size;
    mclog_threads = threads;
}


static void print_mclog_entry (const mclog_entry_t entry)
{
    printf("time: %zu  ID: %d  section: %s  A: %zu  B: %zu\n",
            entry.time, entry.thread_id, section_id_str(entry.sec_id),
            entry.state_a, entry.state_b);
}


void mclog_print_stats ()
{
    for (int i=0; i<mclog_threads; i++) {
        printf("thread %d: %d\n", i, mclog_counters[i]);
    }
}


void mclog_print ()
{
    for (int i=0; i<mclog_threads; i++) {
        printf("thread %d:\n", i);
        for (int j=1; j<mclog_counters[i]; j++) {
            print_mclog_entry(mclog[i][j]);
        }
    }
}

long state_print(size_t state){
    if (state == 18446744073709551615) return -1;
    return state;
}

void mclog_print_file (const char *file)
{
    FILE *f = fopen(file, "w");
    if (f == NULL)
        Abort("Error opening file %s", file);

    // sort the results as well
    int *cur_cntrs = RTmalloc(sizeof(int) * mclog_threads);
    for (int i=0; i<mclog_threads; i++) cur_cntrs[i] = 1;

    int min_thread_id = 0;
    long long min_time;
    while (min_thread_id != -1) {
        min_thread_id = -1;
        min_time = LLONG_MAX;
        for (int i=0; i<mclog_threads; i++) {
            if (cur_cntrs[i] >= mclog_counters[i]) continue;

            mclog_entry_t entry = mclog[i][cur_cntrs[i]];
            if (entry.time < min_time) {
                min_time = entry.time;
                min_thread_id = i;
            }
        }

        if (min_thread_id != -1) {
            mclog_entry_t entry = mclog[min_thread_id][cur_cntrs[min_thread_id]];
            fprintf(f, "%lld,%d,%s,%ld,%ld\n",
                entry.time, entry.thread_id, section_id_str(entry.sec_id),
                state_print(entry.state_a), state_print(entry.state_b));
            cur_cntrs[min_thread_id] ++;
        }

    }

    RTfree(cur_cntrs);
    fclose(f);
}

void mclog_add (int thread_id, section_id id, size_t state_a, size_t state_b)
{
    struct timespec time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
    if (mclog_counters[thread_id] > mclog_max_size + 1) {
        Abort("Log exceeded size limit");
    }
    int c_id = mclog_counters[thread_id]++;

    if (c_id == 0) {
        // add extra initial entry (for timing basis)
        mclog[thread_id][0].thread_id = thread_id;
        mclog[thread_id][0].time      = (long long)time.tv_nsec + ((long long)time.tv_sec * 1000000000);
        mclog[thread_id][0].sec_id    = MCLOG_INIT;
        mclog[thread_id][0].state_a   = state_a;
        mclog[thread_id][0].state_b   = state_b;
        c_id = mclog_counters[thread_id]++;
    }

    mclog[thread_id][c_id].thread_id = thread_id;
    mclog[thread_id][c_id].time      = (long long)time.tv_nsec  + ((long long)time.tv_sec * 1000000000) - mclog[thread_id][0].time;
    mclog[thread_id][c_id].sec_id    = id;
    mclog[thread_id][c_id].state_a   = state_a;
    mclog[thread_id][c_id].state_b   = state_b;
}
