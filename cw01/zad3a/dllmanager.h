#ifndef DLLMANAGER_H
#define DLLMANAGER_H

#ifdef USE_DLL

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void *handle = NULL;

void load_dll(const char* filepath) {
    handle = dlopen(filepath, RTLD_LAZY);

    if (handle == NULL) {
        fprintf(stderr, "Failed to load symbols\n");
        return;
    }

    *(void **) (&init) = dlsym(handle,"init");
    *(void **) (&count) = dlsym(handle,"count");
    *(void **) (&block) = dlsym(handle,"block");
    *(void **) (&delete) = dlsym(handle,"delete");
    *(void **) (&destroy) = dlsym(handle,"destroy");
}

void close_dll(void){
        dlclose(handle);
}

#else
void load_dll(const char* filepath){}
void close_dll(void){}
#endif
#endif
