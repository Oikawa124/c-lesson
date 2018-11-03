#include "clesson.h"

//#define CHARACTER_SIZE 100
//
//char words[CHARACTER_SIZE];
//int word_pos = 0;
//
//
//static void add_char(int ch) {
//    words[word_pos] = ch;
//    word_pos++;
//    if (ch == '\0') {
//        word_pos = 0;
//    }
//}
//
//char *get_readline(FILE *f_p) {
//    int ch;
//    char *ret;
//
//    while ((ch = getc(f_p)) != EOF) {
//        if (ch == '\n') {
//            add_char('\0');
//            break;
//        }
//        add_char(ch);
//    }
//    if (ch == EOF) {
//        add_char(EOF);
//    }
//
//    ret = malloc(sizeof(char) * CHARACTER_SIZE);
//    strcpy(ret, words);
//
//    return ret;
//}


#define ALLOC_SIZE 256

static char *st_line_buffer = NULL;

static int st_current_buffer_size = 0;

static int st_current_used_size = 0;

static void add_char(int ch){
    assert(st_current_buffer_size >= st_current_used_size);

    if (st_current_buffer_size == st_current_used_size) {
        st_line_buffer = realloc(st_line_buffer,
                                          (st_current_buffer_size + ALLOC_SIZE)
                                          * sizeof(char));
        st_current_buffer_size += ALLOC_SIZE;
    }

    st_line_buffer[st_current_used_size] = ch;
    st_current_used_size++;
}

static char *read_line(FILE *fp){
    int ch;
    char *ret;
    st_current_used_size = 0;

    while ((ch = getc(fp)) != EOF) {
        if (ch == '\n') {
            add_char('\0');
            break;
        }
        add_char(ch);
    }

    if (ch == EOF) {
        if (st_current_used_size > 0) {
            add_char('\0');
        } else {
            return NULL;
        }
    }
    ret = malloc(sizeof(char) * st_current_used_size);
    strcpy(ret, st_line_buffer);

    return ret;
}

static void free_buffer(){
    free(st_line_buffer);
    st_line_buffer = NULL;
    st_current_used_size = 0;
    st_current_buffer_size = 0;
}

void repl(){
    char *line;

    printf("input words here\n<");
    while ((line = read_line(stdin)) != NULL) {
        cl_getc_set_src(line);

        eval();

        stack_print_all();
        printf("<");

        if (streq(line, "exit")) { break;}
    }
    printf("\n#exit REPL#\n");
    free(line);
    free_buffer();
}

