/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"
#include "sstring.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char *get_filename(int argc, char *argv[]) {
    // TODO implement get_filename
    // take a look at editor_main.c to see what this is used for
    return argv[1];
}

sstring *handle_create_string() {
    // TODO create empty string
    return cstr_to_sstring("");
}
document *handle_create_document(const char *path_to_file) {
    // TODO create the document
    return document_create_from_file(path_to_file);
}

void handle_cleanup(editor *editor) {
    // TODO destroy the document
    document_destroy(editor->document);
}

void handle_display_command(editor *editor, size_t start_line,
                            ssize_t max_lines, size_t start_col_index,
                            ssize_t max_cols) {
    // TODO implement handle_display_command
    if (document_size(editor->document) == 0 && start_line != 0) {
        print_document_empty_error();
    }

    if (document_size(editor->document) == 0 && start_line == 0) {
        return;
    }

    if (max_lines == -1) {
         max_lines = document_size(editor->document) - start_line + 1;
    }

    if (start_line + max_lines - 1 > document_size(editor->document)) {
        max_lines = document_size(editor->document) - start_line + 1;
    }

    for (size_t i = start_line; i <= (start_line + max_lines - 1); i++) {
        print_line(editor->document, i, start_col_index, max_cols);
    }
}

void handle_insert_command(editor *editor, location loc, const char *line) {
    // TODO implement handle_insert_command
    if (loc.line_no <= document_size(editor->document)) {
       // if (loc.idx < 0) loc.idx = 0;
        if (loc.idx > strlen(document_get_line(editor->document, loc.line_no))) loc.idx = strlen(document_get_line(editor->document, loc.line_no));
    } else {
        loc.idx = 0;
        document_insert_line(editor->document, loc.line_no, line);
        return;
    }

    sstring *s_line = cstr_to_sstring((char *)line);
    sstring *old = cstr_to_sstring((char *)document_get_line(editor->document, loc.line_no));
    char *old1 = sstring_slice(old, 0, loc.idx);
    char *old2 = sstring_slice(old, loc.idx, strlen(document_get_line(editor->document, loc.line_no)));
    sstring *s_old1 = cstr_to_sstring(old1);
    sstring *s_old2 = cstr_to_sstring(old2);
    sstring_append(s_old1, s_line);
    sstring_append(s_old1, s_old2);
    char *new = sstring_to_cstr(s_old1);

    sstring_destroy(s_line);
    sstring_destroy(old);
    sstring_destroy(s_old1);
    sstring_destroy(s_old2);

    free(old1);
    free(old2);

    document_delete_line(editor->document, loc.line_no);
    document_insert_line(editor->document, loc.line_no, new);

    loc.idx += (size_t)strlen(line);

    free(new);
}

void handle_append_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_append_command
    if (line_no > document_size(editor->document)) {
        handle_write_command(editor, line_no, line);
        return;
    }

    char *seeker = (char *)line;
    int count = 0;
    char *p = malloc(1);
    int f = 1;
    if (line_no > document_size(editor->document)) {
        f = 0;
    }
    while (*seeker != '\0') {
       // printf("%c\n", *seeker);
        //sleep(2);
        if (*seeker != '\\' && *seeker != '\n') {
            p[count] = *seeker;
            count++;
            seeker++;
            p = realloc(p, count + 1);
        } else if (*seeker == '\\') {
            if (*(seeker + 1) == '\0') {
                p[count] = '\0';
                if (f) {
                    sstring *s_app = cstr_to_sstring((char *)p);
                    sstring *s_old = cstr_to_sstring((char *)document_get_line(editor->document, line_no));

                    sstring_append(s_old, s_app);
                    char *new = sstring_to_cstr(s_old);

                    sstring_destroy(s_app);
                    sstring_destroy(s_old);
                    document_delete_line(editor->document, line_no);
                    document_insert_line(editor->document, line_no, new);
                    f = 0;
                    free(new);
                } else {
                    document_insert_line(editor->document, line_no, p);
                }
                line_no++;
                free(p);
                p = NULL;
                return;
            } else if (*(seeker + 1) == 'n') {
                p[count] = '\0';
                if (f) {
                    sstring *s_app = cstr_to_sstring((char *)p);
                    sstring *s_old = cstr_to_sstring((char *)document_get_line(editor->document, line_no));

                    sstring_append(s_old, s_app);
                    char *new = sstring_to_cstr(s_old);

                    sstring_destroy(s_app);
                    sstring_destroy(s_old);
                    document_delete_line(editor->document, line_no);
                    document_insert_line(editor->document, line_no, new);
                    f = 0;
                    free(new);
                } else {
                    document_insert_line(editor->document, line_no, p);
                }
                line_no++;
                seeker += 2;
                free(p);
                p = NULL;
                p = malloc(1);
                count = 0;
            } else {
                seeker++;
                p[count] = *seeker;
                count++;
                seeker++;
                p = realloc(p, count + 1);
            }
        }
    }

    p[count] = '\0';
    if (f) {
        sstring *s_app = cstr_to_sstring((char *)p);
        sstring *s_old = cstr_to_sstring((char *)document_get_line(editor->document, line_no));

        sstring_append(s_old, s_app);
        char *new = sstring_to_cstr(s_old);

        sstring_destroy(s_app);
        sstring_destroy(s_old);
        document_delete_line(editor->document, line_no);
        document_insert_line(editor->document, line_no, new);
        free(new);
    } else {
        document_insert_line(editor->document, line_no, p);
    }
    free(p);
    p = NULL;

}

void handle_write_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_write_command
    char *seeker = (char *)line;
    int count = 0;
    char *p = malloc(1);
    int f = 1;
    //printf("%zu\n", line_no);
    if (line_no > document_size(editor->document)) {
        f = 0;
    }
    while (*seeker != '\0') {
    //    printf("%c\n", *seeker);
        if (*seeker != '\\' && *seeker != '\n') {
            p[count] = *seeker;
            count++;
            seeker++;
            p = realloc(p, count + 1);
        } else if (*seeker == '\\') {
            if (*(seeker + 1) == '\0') {
                p[count] = '\0';
                if (f) {
                    //printf("rrrr\n");
                    //printf("%zu\n", line_no);
                    document_delete_line(editor->document, line_no);
                    f = 0;
                }
                document_insert_line(editor->document, line_no, p);
                line_no++;
                free(p);
                p = NULL;
                return;
            } else if (*(seeker + 1) == 'n') {
                p[count] = '\0';
                if (f) {
                    document_delete_line(editor->document, line_no);
                    f = 0;
                }
                document_insert_line(editor->document, line_no, p);
                line_no++;
                seeker += 2;
                free(p);
                p = NULL;
                p = malloc(1);
                count = 0;
            } else {
                seeker++;
                p[count] = *seeker;
                count++;
                seeker++;
                p = realloc(p, count + 1);
            }
        }
    }

    p[count] = '\0';
    if (f) {
       // printf("%zu\n", line_no);
        document_delete_line(editor->document, line_no);
    }
    document_insert_line(editor->document, line_no, p);
    free(p);
    p = NULL;

}

void handle_delete_command(editor *editor, location loc, size_t num_chars) {
    // TODO implement handle_delete_command
    if (loc.line_no <= document_size(editor->document)) {
       // if (loc.idx < 0) loc.idx = 0;
        if (loc.idx > strlen(document_get_line(editor->document, loc.line_no))) loc.idx = strlen(document_get_line(editor->document, loc.line_no));
    } else {
        loc.idx = 0;
        //document_insert_line(editor->document, loc.line_no, line);
        return;
    }

    if (loc.idx + num_chars >= strlen(document_get_line(editor->document, loc.line_no))) {
        num_chars = strlen(document_get_line(editor->document, loc.line_no)) - loc.idx;
    }

    sstring *s_old = cstr_to_sstring((char *)document_get_line(editor->document, loc.line_no));
    char *t1 = sstring_slice(s_old, 0, loc.idx);
    sstring *part1 = cstr_to_sstring(t1);
    char *t2 = sstring_slice(s_old, loc.idx + num_chars, strlen(document_get_line(editor->document, loc.line_no)));
    sstring *part2 = cstr_to_sstring(t2);
    sstring_append(part1, part2);

    char *new = sstring_to_cstr(part1);

    sstring_destroy(s_old);
    sstring_destroy(part1);
    sstring_destroy(part2);
    free(t1);
    free(t2);

    document_delete_line(editor->document, loc.line_no);
    document_insert_line(editor->document, loc.line_no, new);

    loc.idx -= num_chars;
    free(new);
}

void handle_delete_line(editor *editor, size_t line_no) {
    // TODO implement handle_delete_line
    if (line_no > document_size(editor->document)) {
        return;
    }

    document_delete_line(editor->document, line_no);
}

location handle_search_command(editor *editor, location loc,
                               const char *search_str) {
    // TODO implement handle_search_command
    loc.idx++;
    int line_no = (int)loc.line_no;
    int idx = (int)loc.idx;

    if (strlen(search_str) == 0) {
        loc.line_no = 0;
        return loc;
    }

    if (line_no > (int)document_size(editor->document)) {
        line_no = 1;
        idx = 0;
    }

    if (idx >= (int)strlen(document_get_line(editor->document, line_no))) {
        line_no++;
        idx = 0;
        if (line_no > (int)document_size(editor->document)) {
            line_no = 1;
            idx = 0;
        }
    }

    char *find;
    char *first = (char *)document_get_line(editor->document, line_no);
    first += idx;
    find = strstr(first, search_str);

    if (find) {
        char *t = first;
        int co = 0;
        while (t != find) {
            t++;
            co++;
        }
        idx += co;
        found_search((char *)search_str, (size_t)line_no, (size_t)idx);
        loc.line_no = (size_t)line_no;
        loc.idx = (size_t)idx;
        return loc;
    } else {
        line_no++;
        idx = 0;
    }

    for (int i = line_no; i <= (int)document_size(editor->document); i++) {
        char *li = (char *)document_get_line(editor->document, i);
        find = strstr(li, search_str);
        if (find) {
            idx = (find - li) * sizeof(*find);
            found_search((char *)search_str, (size_t)i, (size_t)idx);
            loc.line_no = (size_t)i;
            loc.idx = (size_t)idx;
            return loc;
        } else {
            line_no++;
            idx = 0;
        }
    }

    for (int i = 1; i <= (int)loc.line_no; i++) {
        char *li = (char *)document_get_line(editor->document, i);
        find = strstr(li, search_str);
        if (find) {
            idx = (find - li) * sizeof(*find);
            found_search((char *)search_str, (size_t)i, (size_t)idx);
            loc.line_no = (size_t)i;
            loc.idx = (size_t)idx;
            return loc;
        } else {
            line_no++;
            idx = 0;
        }
    }

    not_found((char *)search_str);
    loc.line_no = 0;
    //loc.idx = 0;
    return loc;
}

void handle_merge_line(editor *editor, size_t line_no) {
    // TODO implement handle_merge_line
    sstring *line1 = cstr_to_sstring((char *)document_get_line(editor->document, line_no));
    sstring *line2 = cstr_to_sstring((char *)document_get_line(editor->document, line_no + 1));
    sstring_append(line1, line2);
    char *new = sstring_to_cstr(line1);

    document_delete_line(editor->document, line_no);
    document_insert_line(editor->document, line_no, new);
    document_delete_line(editor->document, line_no + 1);

    sstring_destroy(line1);
    sstring_destroy(line2);
    free(new);
}

void handle_split_line(editor *editor, location loc) {
    // TODO implement handle_split_line
    if (loc.line_no <= document_size(editor->document)) {
       // if (loc.idx < 0) loc.idx = 0;
        if (loc.idx > strlen(document_get_line(editor->document, loc.line_no))) loc.idx = strlen(document_get_line(editor->document, loc.line_no));
    } else {
        loc.idx = 0;
        //document_insert_line(editor->document, loc.line_no, line);
        return;
    }
    
    sstring *line = cstr_to_sstring((char *)document_get_line(editor->document, loc.line_no));
    char *part1 = sstring_slice(line, 0, loc.idx);
    char *part2 = sstring_slice(line, loc.idx, strlen(document_get_line(editor->document, loc.line_no)));

    document_delete_line(editor->document, loc.line_no);
    document_insert_line(editor->document, loc.line_no, part1);
    document_insert_line(editor->document, loc.line_no + 1, part2);

    sstring_destroy(line);
    free(part1);
    free(part2);
}

void handle_save_command(editor *editor) {
    // TODO implement handle_save_command
    document_write_to_file(editor->document, editor->filename);
}
