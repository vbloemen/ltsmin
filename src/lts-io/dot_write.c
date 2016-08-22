// -*- tab-width:4 ; indent-tabs-mode:nil -*-
#include <hre/config.h>

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include <hre/user.h>
#include <lts-io/internal.h>
#include <ltsmin-lib/ltsmin-standard.h>
#include <util-lib/chunk_support.h>
#include <util-lib/tables.h>

struct lts_file_s{
    FILE* f;
};

/*
The DOT parser cannot deal with " so it's changed to '
*/
static void fix_double_quote (char *str){
    int i = -1;
    while (str[++i] != '\0') {
        if (str[i] == '"')
            str[i] = '\'';
    }
}

static void dot_write_init (lts_file_t file, int seg, void *state){

    fprintf(file->f, "digraph g {\n");

    (void) file;
    (void) seg;
    (void) state;
}

static void dot_write_state (lts_file_t file, int seg, void *state, void*labels){


    // write in the form "  STATE [label="STATE,BUCHI", shape="SHAPE"];"
    lts_type_t lts_type =  lts_file_get_type(file);
    int        NS       =  lts_type_get_state_label_count(lts_type);
    uint32_t   src      = *((uint32_t*) state);
    uint32_t  *lbl      =  ((uint32_t*) labels);

    fprintf(file->f, "  %d [label=\"%d", src, src);

    bool buchi_accept = false;

    for (int i=0; i<NS; i++) {
        int type_no = lts_type_get_state_label_typeno(lts_type, i);
            switch(lts_type_get_format(lts_type, type_no)) {
            case LTStypeEnum: 
            case LTStypeChunk: 
            {
                value_table_t table = lts_file_get_table(file, type_no);
                HREassert(table != NULL, "Could not find lts table!");
                chunk label_c = VTgetChunk(table, lbl[i]);
                char label_s[label_c.len * 2 + 6]; // magic numbers from aut_io.c
                chunk2string(label_c, sizeof label_s, label_s);
                fix_double_quote (label_s);
                char *type_name = lts_type_get_state_label_name(lts_type, i);
                if (strcmp(type_name, LTSMIN_STATE_LABEL_ACCEPTING) == 0) {
                    if (strcmp(label_s, "'true'") == 0)
                        buchi_accept = true;
                } else if (strcmp(type_name, LTSMIN_STATE_LABEL_WEAK_LTL_PROGRESS) != 0) {
                    fprintf(file->f, ",%s", label_s);
                }
            }
        }
    }
    fprintf(file->f, "\", shape=%s];\n", buchi_accept?"doublecircle":"circle");

    (void) seg;
}

static void dot_write_edge (lts_file_t file, int src_seg, void *src_state,
                           int dst_seg, void *dst_state, void *labels){

    // write in the form "  SRC -> DST [label="EDGE_LABELS"];"
    lts_type_t lts_type =  lts_file_get_type(file);
    int        NE       =  lts_type_get_edge_label_count(lts_type);
    uint32_t   src      = *((uint32_t*) src_state);
    uint32_t   dst      = *((uint32_t*) dst_state);
    uint32_t  *lbl      =  ((uint32_t*) labels);
    
    fprintf(file->f, "  %llu -> %llu  [label=\"",
            (long long unsigned int) src,
            (long long unsigned int) dst);

    for (int i=0; i<NE; i++) {
        if (i != 0)
            fprintf(file->f, "\\n");
        int type_no = lts_type_get_edge_label_typeno(lts_type, i);
        switch(lts_type_get_format(lts_type, type_no)) {
            case LTStypeEnum: 
            case LTStypeChunk: 
            {
                value_table_t table = lts_file_get_table(file, type_no);
                HREassert(table != NULL, "Could not find lts table!");
                chunk label_c = VTgetChunk(table, lbl[i]);
                char label_s[label_c.len * 2 + 6]; // magic numbers from aut_io.c
                chunk2string(label_c, sizeof label_s, label_s);
                fix_double_quote (label_s);
                char *type_name = lts_type_get_edge_label_name(lts_type, i);
                //fprintf(file->f, "%s:%s", type_name, label_s);
                fprintf(file->f, "%s", label_s);
                break;
            }
            default:
                fprintf(file->f, "%d", lbl[i]); // just directly print the label
        }

    }
    fprintf(file->f, "\"];\n");

    (void) src_seg;
    (void) dst_seg;
}

static void dot_write_close (lts_file_t file){
    fprintf(file->f, "}\n");
    if (fclose(file->f)){
        AbortCall("while closing %s",lts_file_get_name(file));
    }
}

lts_file_t dot_file_create (const char *name, lts_type_t ltstype, int segments, lts_file_t settings)
{
    lts_file_t file=lts_file_bare(name,ltstype,segments,settings,sizeof(struct lts_file_s));
    file->f=fopen(name,"w");
    if(file->f==NULL){
        AbortCall("while opening %s",name);
    }

    lts_file_set_write_init(file,dot_write_init);
    lts_file_set_write_state(file,dot_write_state);
    lts_file_set_write_edge(file,dot_write_edge);
    lts_file_set_close(file,dot_write_close);
    lts_file_complete(file);
    return file;
}