// -*- tab-width:4 ; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <ctype.h>
#include <hre/config.h>

#include <hre/user.h>
#include <lts-io/internal.h>
#include <ltsmin-lib/ltsmin-standard.h>

struct lts_file_s {
	value_table_t labelAction;
	int labelActionIndex;
	value_table_t labelCustomDot;
	int labelCustomDotIndex;
//	long long unsigned int root;
	char*name;
	FILE* file;
	int statesSeen;
};

static int thinkItIsAString(const char* str, int len) {
	const char* p = str;
	while(p<(str+len) && isgraph(*p)) {
		p++;
	}
	//return (p==(str+len) && len>0);
	if(p<str+len) {
		return len>0 && (*p=='\0');
	} else {
		return len>0;
	}
}

static void printChunkType(lts_file_t file, int typeNo, int value) {
	if(lts_type_get_format(lts_file_get_type(file), typeNo)==LTStypeChunk) {
		printf("DOING %i\n", typeNo);
		value_table_t table = lts_file_get_table(file,typeNo);
		chunk c = VTgetChunk(table, value);
		if(thinkItIsAString(c.data, c.len)) {
			char* copy = malloc(c.len*2);
			char* src = c.data;
			char* dst = copy;
			while(src<c.data+c.len) {
				switch(*src) {
					case '|':
					case '{':
					case '}':
					case '"':
						*dst++ = '\\';
						*dst++ = '"';
						break;
					default:
						*dst++ = *src;
						break;
				}
				src++;
			}
			*dst = '\0';
			fprintf(file->file, "%s", copy);
			free(copy);
		} else {
//			fprintf(file->file, "[%i]", name, lts_type_get_type(lts_file_get_type(file), typeNo), value);
			for(size_t j=0; j<c.len; ++j) {
				fprintf(file->file, "%x", c.data[j]);
			}
			fprintf(file->file, "[%i]", c.len);
		}
	} else {
		fprintf(file->file, "%i", value);
	}
}

static void dot_init() {
}

static void dot_file_push(lts_file_t src,lts_file_t dst) {
	(void)src;
	(void)dst;
	Abort("dot_file_push");
}

static void dot_read_close(lts_file_t file) {
	(void)file;
	Abort("dot_file_close");
}

lts_file_t dot_file_open(const char* name) {
	lts_type_t ltstype=single_action_type();
	lts_file_t file=lts_file_bare(name,ltstype,1,NULL,sizeof(struct lts_file_s));
	lts_file_set_push(file,dot_file_push);
	lts_file_set_close(file,dot_read_close);
	lts_file_complete(file);
	Debug("dot_read_begin %s",name);
	//file->root=BCG_OT_INITIAL_STATE (file->graph);
	//file->root = 0;
	return file;
}

static void dot_write_init(lts_file_t file,int seg,void* state) {
	//uint32_t root=*((uint32_t*)state);
	if (seg!=0) Abort("bad initial state %u",seg);
	if(file->name) {
//		file->root=root;
		file->file = fopen(file->name, "wb+");
		RTfree(file->name);
		file->name=NULL;
		
		fprintf(file->file, "digraph LTS {\n");
	fprintf(file->file,"\t\trankdir=\"TB\";\n");
		
//	} else {
//		if (file->root!=root) Abort("bad initial state %u/%u",seg,root);
	}
	int index = 0;
	index = lts_type_get_edge_label_count(lts_file_get_type(file));
	while(index-- && strcmp("action",lts_type_get_edge_label_name(lts_file_get_type(file),index)));
	if(index>=0) {
		file->labelActionIndex = index;
		int type_no=lts_type_get_edge_label_typeno(lts_file_get_type(file),index);
		file->labelAction=lts_file_get_table(file,type_no);
	}
	index = lts_type_get_edge_label_count(lts_file_get_type(file));
	while(index-- && strcmp("custom_dot",lts_type_get_edge_label_name(lts_file_get_type(file),index)));
	if(index>=0) {
		file->labelCustomDotIndex = index;
		int type_no=lts_type_get_edge_label_typeno(lts_file_get_type(file),index);
		file->labelCustomDot=lts_file_get_table(file,type_no);
	}
	fprintf(file->file, "\tsubgraph States {\n");
	fprintf(file->file,"\t\trankdir=\"TB\";\n");
}

static void dot_write_edge(lts_file_t file,int src_seg,void* src_state,
                           int dst_seg,void*dst_state,void* labels) {
	(void)src_seg;
	(void)dst_seg;
	if(file->name) {
		//BCG_IO_WRITE_BCG_BEGIN(file->name,0,1,"LTSmin",0);
		RTfree(file->name);
		file->name=NULL;
	}
	uint32_t src=*((uint32_t*)src_state);
	uint32_t dst=*((uint32_t*)dst_state);
//	if (file->root) {
//		if (src==file->root) {
//			src=0;
//		} else if (src==0) {
//			src=file->root;
//		}
//		if (dst==file->root) {
//			dst=0;
//		} else if (dst==0) {
//			dst=file->root;
//		}
//	}
	fprintf(file->file, "\t\t %u -> %u [ shape=rectangle", src, dst);
	if (file->labelAction && labels) {
		uint32_t* lbl=((uint32_t*)labels);
//		chunk label_c;
//		char label_s[label_c.len*2+6];
//		if(file->labels==NULL) {
//			sprintf(label_s,"%u",lbl);
		chunk label_c = VTgetChunk(file->labelAction,lbl[file->labelActionIndex]);
//			chunk2string(label_c,sizeof label_s,label_s);
//		}
		fprintf(file->file, ", label = \"%s\"", label_c.data);
	}
	if (file->labelCustomDot && labels) {
		uint32_t* lbl=((uint32_t*)labels);
		chunk label_c = VTgetChunk(file->labelCustomDot,lbl[file->labelCustomDotIndex]);
		fprintf(file->file, ", %s", label_c.data);
	}
	fprintf(file->file, " ];\n");
//    char *bcg_label;
//    if (strcmp(label_s,LTSMIN_EDGE_VALUE_TAU) && strcmp(label_s,"\"" LTSMIN_EDGE_VALUE_TAU "\"")){
//        bcg_label=label_s;
//    } else {
//        bcg_label="i";
//    }
//    BCG_IO_WRITE_BCG_EDGE(src,bcg_label,dst);
//	fprintf(file->file, "\t");
//	for(int i=0; i<lts_type_get_state_length(lts_file_get_type(file)); ++i) {
//		fprintf(file->file, "%u", ((uint32_t*)src_state)[i]);
//	}
//	fprintf(file->file, " -> ");
//	for(int i=0; i<lts_type_get_state_length(lts_file_get_type(file)); ++i) {
//		fprintf(file->file, "%u", ((uint32_t*)dst_state)[i]);
//	}
//	fprintf(file->file, "\n");
}

static void dot_write_state(lts_file_t file,int seg,__uint32_t *state,__uint32_t* labels) {

#if 0
	if(lts_type_get_state_length(lts_file_get_type(file))) {
		fprintf(file->file, "\t %u [ label=\"%u[", file->statesSeen, file->statesSeen);
		fprintf(file->file, "%u", state[0]);
		for(int i=1; i<lts_type_get_state_length(lts_file_get_type(file)); ++i) {
			fprintf(file->file, "-%u", state[i]);
		}
		fprintf(file->file,"]");
		if(lts_type_get_state_label_count(lts_file_get_type(file))) {
			for(int i=0; i<lts_type_get_state_label_count(lts_file_get_type(file)); ++i) {
				int typeNo = lts_type_get_state_label_typeno(lts_file_get_type(file), i);
				if(lts_type_get_format(lts_file_get_type(file), typeNo)==LTStypeChunk) {
					value_table_t table = lts_file_get_table(file,typeNo);
					fprintf(file->file, "\\n+%s: %s", lts_type_get_state_label_name(lts_file_get_type(file), i), VTgetChunk(table, labels[i]).data);
				} else {
					fprintf(file->file, "\\n-%s: %i", lts_type_get_state_label_name(lts_file_get_type(file), i), labels[i]);
				}
			}
		}
	} else {
		fprintf(file->file, "\t %u [ label=\"%u[", *state, *state);
		if(lts_type_get_state_label_count(lts_file_get_type(file))) {
			fprintf(file->file, "-%u", labels[0]);
			for(int i=1; i<lts_type_get_state_label_count(lts_file_get_type(file)); ++i) {
				fprintf(file->file, "-%u", labels[i]);
			}
		}
	}
#else
	if(lts_type_get_state_length(lts_file_get_type(file))) {
		fprintf(file->file, "\t\t %u [ label=\"{%u|{{state|{", file->statesSeen, file->statesSeen);
		for(int i=0, first=1; i<lts_type_get_state_length(lts_file_get_type(file)); ++i) {
			if(first) { first = 0; } else { fprintf(file->file, "|"); }
			const char* name = lts_type_get_state_name(lts_file_get_type(file), i);
			fprintf(file->file, "{%.3s|%u}", name?name:"??????" , state[i]);
			//int typeNo = lts_type_get_state_typeno(lts_file_get_type(file), i);
			//printChunkType(file, lts_type_get_state_name(lts_file_get_type(file), i), typeNo, state[i]);
		}
		fprintf(file->file,"}}");
		if(lts_type_get_state_label_count(lts_file_get_type(file))) {
			fprintf(file->file, "|{labels|{");
			for(int i=0, first=1; i<lts_type_get_state_label_count(lts_file_get_type(file)); ++i) {
				if(first) { first = 0; } else { fprintf(file->file, "|"); }
				const char* name = lts_type_get_state_label_name(lts_file_get_type(file), i);
				fprintf(file->file, "{%.3s|%u}", name?name:"??????", labels[i]);
//				int typeNo = lts_type_get_state_label_typeno(lts_file_get_type(file), i);
//				printChunkType(file, lts_type_get_state_label_name(lts_file_get_type(file), i), typeNo, labels[i]);
//				if(lts_type_get_format(lts_file_get_type(file), typeNo)==LTStypeChunk) {
//					value_table_t table = lts_file_get_table(file,typeNo);
//					chunk c = VTgetChunk(table, labels[i]);
//					if(thinkItIsAString(c.data, c.len)) {
//						fprintf(file->file, "|{%s|%s}", lts_type_get_state_label_name(lts_file_get_type(file), i), c.data);
//					} else {
//						fprintf(file->file, "|{%s|%i}", lts_type_get_state_label_name(lts_file_get_type(file), i), labels[i]);
//					}
//				} else {
//					fprintf(file->file, "|{-%s|%i}", lts_type_get_state_label_name(lts_file_get_type(file), i), labels[i]);
//				}
			}
			fprintf(file->file, "}}");
		}
		fprintf(file->file,"}}");
	} else {
		fprintf(file->file, "\t\t %u [ label=\"%u[", *state, *state);
		if(lts_type_get_state_label_count(lts_file_get_type(file))) {
			fprintf(file->file, "-%u", labels[0]);
			for(int i=1; i<lts_type_get_state_label_count(lts_file_get_type(file)); ++i) {
				fprintf(file->file, "-%u", labels[i]);
			}
		}
	}
#endif
	fprintf(file->file,"\", shape=record ];\n");
	++file->statesSeen;
}

static void dot_write_close(lts_file_t file) {
	fprintf(file->file, "\t}\n");

	int maxChunks = 0;
	
	int* types = malloc(lts_type_get_type_count(lts_file_get_type(file))*sizeof(int));
	int types_n = 0;
	
	for(int typeNo=0; typeNo<lts_type_get_type_count(lts_file_get_type(file)); ++typeNo) {
		if(lts_type_get_format(lts_file_get_type(file), typeNo) != LTStypeChunk) continue;
		if(!strcmp("action",lts_type_get_type(lts_file_get_type(file), typeNo))) continue;
		if(!strcmp("custom_dot",lts_type_get_type(lts_file_get_type(file), typeNo))) continue;
		value_table_t table = lts_file_get_table(file,typeNo);
		maxChunks = VTgetCount(table)>maxChunks ? VTgetCount(table) : maxChunks;
		types[types_n] = typeNo;
		types_n++;
	}
	maxChunks++;
	if(types_n>0) {
		fprintf(file->file, "\tsubgraph Types {\n");
		fprintf(file->file,"\t\trankdir=\"TB\";\n");
		fprintf(file->file, "\t\ttypes [ label = \"{Types|{");
		for(int n=0, first = 1; n<types_n; ++n) {
			int typeNo = types[n];
			value_table_t table = lts_file_get_table(file,typeNo);
			if(first) { first = 0; } else { fprintf(file->file, "|"); }
			fprintf(file->file, "{%s", lts_type_get_type(lts_file_get_type(file), typeNo));
			int chunkID = 0;
			for(;chunkID<VTgetCount(table); ++chunkID) {
				//chunk c = VTgetChunk(table, chunkID);
				fprintf(file->file, "|{%i|", chunkID);
				printChunkType(file, typeNo, chunkID);
				fprintf(file->file, "}");
			}
			for(;chunkID<maxChunks; ++chunkID) {
				fprintf(file->file, "|{|}");
			}
			fprintf(file->file, "}");
		}
		fprintf(file->file, "}}\", shape=record, pos=\"-10,-10\" ];\n");
		fprintf(file->file, "\ttypes -> 0\n");
		fprintf(file->file, "\t}\n");
	}
	fprintf(file->file, "}\n");
	if(file->file) fclose(file->file);
	file->file = NULL;
	free(types);
}

lts_file_t dot_file_create(const char* name,lts_type_t ltstype,int segments,lts_file_t settings) {
//	if (lts_type_get_state_length(ltstype)) {
//		Abort("cannot write state to DOT file");
//	}
	if (segments!=1) Abort("DOT files contain precisely 1 segment");
	lts_file_t file=lts_file_bare(name,ltstype,1,settings,sizeof(struct lts_file_s));
	file->name=HREstrdup(name);
	lts_file_set_write_state(file,(lts_write_state_m)dot_write_state);
	lts_file_set_write_init(file,dot_write_init);
	lts_file_set_write_edge(file,dot_write_edge);
	lts_file_set_close(file,dot_write_close);
	lts_file_complete(file);
	return file;
}
