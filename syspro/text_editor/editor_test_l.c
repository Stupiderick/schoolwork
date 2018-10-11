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

// print all content
void print_all(document* doc) {
    size_t size = 1;
    for(; size <= document_size(doc); size++){
        printf("line %d: %s\n", (int)size, document_get_line(doc, size));
    }
}

/**
 * You can programatically test your text editor.
*/
int main() {
    // Setting up a docment based on the file named 'filename'.
    char *filename = "test.txt";
    document *doc = document_create_from_file(filename);
    printf("size: %d\n", (int)document_size(doc));
    document_insert_line(doc, 3, "lalalalaalaala");
    printf("size: %d\n", (int)document_size(doc));
    print_all(doc);
    printf("finished test insert line\n\n");
    document_set_line(doc, 2, "yess");
    print_all(doc);
    printf("finished test set line\n\n");
    document_delete_line(doc, 3);
    print_all(doc);
    document_destroy(doc);
    printf("finished test document\n\n");

    //test editor
    editor* edit = malloc(sizeof(editor));
    printf("The ptr size: %d, the obj size: %d\n", (int)sizeof(editor*), (int)sizeof(editor));
    edit->document = handle_create_document(filename);
    edit->filename = filename;
    location loc;
    loc.line_no = 5;
    loc.idx = 0;
    handle_insert_command(edit, loc, "ye");
    handle_display_command(edit, 1, -1, 0, -1);
    char *test_app = "123\\\\n 3\\a5\\n6\\";
    printf("str: %s\n", test_app);
    handle_append_command(edit, 5, test_app);
    handle_display_command(edit, 1, -1, 0, -1);
    char *test_wr = "\\nWrite this \\a, \\nfd";
    handle_write_command(edit, 7, test_wr);
    handle_display_command(edit, 1, -1, 0, -1);
    loc.line_no = 5;
    loc.idx = 4;
    char* test_insert = "Mo\\nha ";
    handle_insert_command(edit, loc, test_insert);
    handle_display_command(edit, 1, -1, 0, -1);
    puts("Insert, write, append module test complete!\n");

    int i = 1;
    for(; i < 10; i++){
        char *test_str = "Moha +1";
        handle_write_command(edit, (size_t)i, test_str);
    }
    handle_append_command(edit, document_size(edit->document), "Really MO?\\n "); //should it insert new line?
    handle_append_command(edit, 4, "Brutly MO is\\n not suggested!\\n ");
    handle_display_command(edit, 1, -1, 0, -1);
    loc.line_no = 4;
    loc.idx = 2;
    handle_delete_command(edit, loc, 4);
    handle_display_command(edit, 1, -1, 0, -1);
    //handle_delete_line(edit, 5);
    //handle_display_command(edit, 1, -1, 0, -1);
    loc.line_no = 4;
    loc.idx = 3;
    char *find = "not suggested";
    location result = handle_search_command(edit, loc, find);
    found_search(find, result.line_no, result.idx);
    handle_append_command(edit, 6, "moha Moha moha ohma MMoha");
    handle_display_command(edit, 1, -1, 0, -1);
    char *test_find = "Moha";
    location loc1;
    location loc2;
    location loc3;
    loc1.line_no = 6;
    loc1.idx = 0;
    loc2.line_no = 6;
    loc2.idx = 8;
    loc3.line_no = 6;
    loc3.idx = 13;
    loc1 = handle_search_command(edit, loc1, test_find);
    loc2 = handle_search_command(edit, loc2, test_find);
    loc3= handle_search_command(edit, loc3, test_find);
    found_search(test_find, loc1.line_no, loc1.idx);
    found_search(test_find, loc2.line_no, loc2.idx);
    found_search(test_find, loc3.line_no, loc3.idx);
    handle_display_command(edit, 1, -1, 0, -1);
    loc.line_no = 7;
    loc.idx = 2;
    handle_split_line(edit, loc);
    handle_display_command(edit, 1, -1, 0, -1);
    puts(edit->filename);
    handle_save_command(edit);
    edit->string = cstr_to_sstring("");
    handle_cleanup(edit);

    // test create from file
    edit->document = handle_create_document("test.txt");
    handle_display_command(edit, 1, -1, 0, -1);
    edit->string = cstr_to_sstring("");
    handle_cleanup(edit);
    free(edit);

/*
    append_command_test
    document_load_test
    find_test_multiple_exist_test
    inserting_test_1
    loading_saving_test
    mutiple_inserts_same_line_test
    printing_test
    split_lines_test
    write_command_test
    write_white_spaces_test
*/
    editor* testing = malloc(sizeof(editor));
    testing->document = handle_create_document("grader_test.txt");
    testing->filename = "grader_test.txt";
    //append_command_test
    // append single line
    char *test_str = "Moha";
    handle_append_command(testing, 2, test_str);
    handle_append_command(testing, 2, "+1");
    testing->string = cstr_to_sstring("");
    handle_save_command(testing);
    handle_cleanup(testing);
    free(testing);
}
