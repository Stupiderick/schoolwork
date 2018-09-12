/**
* Vector Lab
* CS 241 - Fall 2018
*/


#include <string.h>
#include "sstring.h"

int main() {
    // TODO create some tests
    char *test1 = "helloworld";
    char *test2 = "okayfine";
    sstring *ptr1 = cstr_to_sstring(test1);
    sstring *ptr2 = cstr_to_sstring(test2);

    char *t1 = sstring_to_cstr(ptr1);
    char *t2 = sstring_to_cstr(ptr2);

    printf("init: %s\n%s\n", t1, t2);
    //int count = sstring_append(ptr1, ptr2);
    char *t3 = sstring_to_cstr(ptr1);
    char *t4 = sstring_to_cstr(ptr2);
    //printf("after append: %s\n%s\nlength of ptr1: %d\n", t3, t4, count);

    vector *vtr = sstring_split(ptr1, 'l');
    printf("after split helloworld by l:\n");
    for (int i = 0; i < 4; i++) {
        printf("%s\n", vector_get(vtr, i));
    }
    vector_destroy(vtr);

    vector *vtr2 = sstring_split(ptr1, 'd');
     printf("after split helloworld by d:\n");
      for (int i = 0; i < 2; i++) {
          printf("%s\n", vector_get(vtr2, i));
      }
      vector_destroy(vtr2);
    
    char *p = "";
    p  = sstring_slice(ptr2, 2, 5);
    printf("%s\n", p);

    free(p);

    free(t1);
    free(t2);
    free(t3);
    free(t4);
    sstring_destroy(ptr1);
    sstring_destroy(ptr2);

    sstring *ptr3 = cstr_to_sstring(test1);
    char* tar = "ell";
    char* sub = "tt";
    sstring_substitute(ptr3, 0, tar, sub);
    
    sstring_destroy(ptr3);


//    return 0;
//}




/**
* Vector Lab
* CS 241 - Fall 2018
*/

//#include "sstring.h"
//#include <string.h>

// int main() {
    // TODO create some tests
    //test cstr_to_sstring, sstring_to_cstr
    char *str1 = "abcedf";
    sstring *test = cstr_to_sstring(str1);
    char *back = sstring_to_cstr(test);
    printf("char: %s, size: %d, cstring: %s, size: %d\n", str1, (int)strlen(str1), back, (int)strlen(back));
    printf("cstr transfrom finished\n\n");
    free(back);

    //test append
    char *str2 = "ni hao!";
    char *str3 = "Hello";
    sstring *ss2 = cstr_to_sstring(str2);
    sstring *ss3 = cstr_to_sstring(str3);
    int result = sstring_append(ss2, ss3);
    printf("NULL? %d\n", (ss2 == NULL));
    char *out2 = sstring_to_cstr(ss2);
    char *out3 = sstring_to_cstr(ss3);
    printf("char: %s, size: %d, cstring: %s, size: %d, result: %d\n", out2, (int)strlen(out2), out3, (int)strlen(out3), result);
    free(out2);
    free(out3);
    printf("append test finished\n\n");

    //split
    char *str4 = "aopqb ceopqd e abde";
    sstring *ss4 = cstr_to_sstring(str4);
    vector *sp = sstring_split(ss4, ',');
    int size = (int)vector_size(sp);
    while(size){
        size --;
        char* printval = vector_get(sp, size);
        printf("size %d, %s\n", size, printval);
    }
    vector_destroy(sp);
    printf("split finished\n\n");

    //subs
    char *str5 = str4;
    sstring *ss5 = cstr_to_sstring(str5);
    char *target = "aopqb ceopqid e abde";
    char *subs = "tttttttt";
    printf("before: %s\n", str5);
    result = sstring_substitute(ss5, 0, target, subs);
    char *out5 = sstring_to_cstr(ss5);
    printf("after: %s, result: %d\n", out5, result);
    free(out5);

    //slice
    char *str6 = "0123456";
    sstring *ss6 = cstr_to_sstring(str6);
    char *out6 = sstring_slice(ss6, 7, 8);
    printf("before: %s, after %s\n", str6, out6);
    free(out6);

    //test destroy
    sstring_destroy(test);
    sstring_destroy(ss2);
    sstring_destroy(ss3);
    sstring_destroy(ss4);
    sstring_destroy(ss6);
    sstring_destroy(ss5);
    //free(back);
    return 0;
}

