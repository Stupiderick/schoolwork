/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * You can programatically test your text editor.
*/
int main() {
    // Setting up a docment based on the file named 'filename'.
    char *filename = "test1.txt";
    document *doc = document_create_from_file(filename);
    //(void)doc;
    printf("document size: %zu\n", document_size(doc));

    location *l = malloc(sizeof(location));
    l->line_no = 2;
    l->idx = 4;
    document_destroy(doc);

    editor *p = (editor *)malloc(sizeof(editor));
    p->document = handle_create_document(filename);
    p->filename = filename;
    handle_display_command(p, 1, -1, 0, -1);
    handle_insert_command(p, *l, "hello");
    handle_display_command(p, 1, -1, 0, -1);
    handle_append_command(p, 7, "\\\\n \\a\\");
    handle_display_command(p, 1, -1, 0, -1);
    handle_write_command(p, 2, "hello\\nyes\\nno\\nkkfhjvbsfsbvhbjvsbvhsvbjshvbsh");
    handle_display_command(p, 1, -1, 0, -1);

    l->line_no = 6;
    l->idx = 5;
    handle_delete_command(p, *l, 100);
    handle_display_command(p, 1, -1, 0, -1);
    handle_delete_line(p, 4);
    handle_display_command(p, 1, -1, 0, -1);
    l->line_no = 4;
    l->idx = 500;
    *l = handle_search_command(p, *l, "yes");
    *l = handle_search_command(p, *l, "yes");
    *l = handle_search_command(p, *l, "yes");
    *l = handle_search_command(p, *l, "Yjjjjes");


    handle_merge_line(p, 7);
    handle_display_command(p, 1, -1, 0, -1);

    l->line_no = 4;
    l->idx = 5;
    handle_split_line(p, *l);
    handle_display_command(p, 1, -1, 0, -1);
    handle_save_command(p);
    document_destroy(p->document);


    free(l);
    free(p);
}
