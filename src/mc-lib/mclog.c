
#include <hre/config.h>
#include <mc-lib/mclog.h>

#include <stdio.h>
#include <time.h>

struct mclog_entry_s
{
    int thread_id;
    long time;
    section_id sec_id;
};

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
    printf("time: %zu  ID: %d  section: %s\n",
            entry.time, entry.thread_id, section_id_str(entry.sec_id));
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

void mclog_print_file (const char *file)
{
    FILE *f = fopen(file, "w");
    if (f == NULL)
        Abort("Error opening file %s", file);

    // sort the results as well
    int *cur_cntrs = RTmalloc(sizeof(int) * mclog_threads);
    for (int i=0; i<mclog_threads; i++) cur_cntrs[i] = 1;
    long cur_time  = 0;

    int min_thread_id = 0;
    long min_time;
    while (min_thread_id != -1) {
        min_thread_id = -1;
        min_time = 2147483647;
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
            fprintf(f, "%zu,%d,%s\n",
                entry.time, entry.thread_id, section_id_str(entry.sec_id));
            cur_cntrs[min_thread_id] ++;
        }

    }

    RTfree(cur_cntrs);
    fclose(f);
}

void mclog_add (int thread_id, section_id id)
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
        mclog[thread_id][0].time      = time.tv_nsec;
        mclog[thread_id][0].sec_id    = MCLOG_INIT;
        c_id = mclog_counters[thread_id]++;
    }

    mclog[thread_id][c_id].thread_id = thread_id;
    mclog[thread_id][c_id].time      = time.tv_nsec - mclog[thread_id][0].time;
    mclog[thread_id][c_id].sec_id    = id;
}
