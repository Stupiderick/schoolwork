// Author: Erick
// Key_codes:h
// F_name:erick_ext
// Description:"Copy and paste by using curser, line and idx"
//---
#include <string.h>
void erick_ext(editor *editor, display *display, char **buffer, char k) {
    (void)buffer;
    (void)k;

    char *str;
    char *input = NULL;
    char *input2 = NULL;
    char *input3 = NULL;

    display_interact(display, "How many bytes you want to copy starting from the curser (included) (-1: to the end) ? ", &input);
    // make sure there are something in the input...
    if (strlen(input) == 0) {
        fprintf(stderr, "ERROR: No input!\n");
        return;
    }
    // make sure the input is legit as number only...
    char *ptr;
    int num = (int)strtol(input, &ptr, 10);
    if (strlen(ptr) != 0) {
        fprintf(stderr, "ERROR: Numbers ONLY!\n");
        return;
    }


    display_interact(display, "Paste to which line (1-indexed)? ", &input2);
    if (strlen(input2) == 0) {
        free(input);
        fprintf(stderr, "ERROR: No input!\n");
        return;
    }
    char *ptr2;
    int num2 = (int)strtol(input2, &ptr2, 10);
    if (strlen(ptr2) != 0) {
        free(input);
        free(input2);
        fprintf(stderr, "ERROR: Numbers ONLY!\n");
        return;
    }


    display_interact(display, "Paste from which column (0-indexed)? ", &input3);
    if (strlen(input3) == 0) {
        free(input);
        free(input2);
        fprintf(stderr, "ERROR: No input!\n");
        return;
    }
    char *ptr3;
    int num3 = (int)strtol(input3, &ptr3, 10);
    if (strlen(ptr3) != 0) {
        free(input);
        free(input2);
        free(input3);
        fprintf(stderr, "ERROR: Numbers ONLY!\n");
        return;
    }


    location loc = display_get_loc(display);
    char *got = (char *)document_get_line(editor->document, loc.line_no);

    // make sure the line is in the range...
    if (num == -1 || num + (int)loc.idx - 1 > (int)strlen(got)) {
        num = (int)strlen(got) - loc.idx + 1;
    }

    str = malloc(num + 1);
    for (int i = 0; i < num; i++) {
        str[i] = got[loc.idx + i];
    }
    str[num] = '\0';

    if (num2 > (int)document_size(editor->document)) {
        handle_append_command(editor, num2, str);
    } else {
        if (num3 >= (int)strlen((char *)document_get_line(editor->document, num2))) {
            num3 = (int)strlen((char *)document_get_line(editor->document, num2)) - 1;
        }
        loc.line_no = num2;
        loc.idx = num3;
        handle_insert_command(editor, loc, str);
    }
    free(str);
}
