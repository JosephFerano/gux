#include <stdio.h>
#include <dlfcn.h>

int main(void) {

    typedef int (*add_f)(int, int);

    void *handle = dlopen("./libmylib.so", RTLD_NOW);

    add_f f;
    *(void **) (&f) = dlsym(handle, "add");

    int test = f(5, 5);
    printf("%i\n", test);
}
