// strutil.cpp
#include "strutil.h"
#include <iostream>
namespace pr {

size_t length(const char* s) {
    size_t y = 0;
    for(;*s;++s){
        ++y;
    }
    return y;
}

char* newcopy(const char* s) {
    size_t len = length(s);
    char* nc = new char[len+1];
    for(size_t i=0;i<=len;++i){
        nc[i] = s[i];
    }
    return nc;
}

int compare(const char* a, const char* b) {
    for(;*a == *b && *a;++a,++b){}
    return *a - *b;
}

char* newcat(const char* a, const char* b) {
    size_t len_a = length(a);
    size_t len_b = length(b);
    char* concat = new char[len_a + len_b + 1];
    
    for (size_t i = 0; i < len_a; ++i) {
        concat[i] = a[i];
    }
    for (size_t i = 0; i < len_b; ++i) {
        concat[len_a + i] = b[i];
    }

    concat[len_a + len_b] = '\0';
    return concat;
}
}
