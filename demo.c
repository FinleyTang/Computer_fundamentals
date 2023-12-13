
/* sbrk and brk example */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void test_brk();
void test_sbrk();
void * naive_malloc(size_t size);
void first_fit();
void basic_function_test();
void vul_test();
void uaf_test();

void test_brk(){
    printf("=====================test_brk============================\n");
    void* ptr1 = sbrk(0);  // 获取当前堆区的末尾地址
    printf("初始堆指针位置：%p\n", ptr1);

    brk(ptr1 + sizeof(int));  // 将堆区的末尾地址扩展sizeof(int)个字节

    int* int_ptr = (int*)ptr1;  // 使用分配的内存块
    printf("分配的内存地址：%p\n", int_ptr);
    *int_ptr = 4;
    printf("%d", *int_ptr);

    void* ptr2 = sbrk(0);  // 再次获取堆区的末尾地址
    printf("分配后堆指针位置：%p\n", ptr2);
}


void test_sbrk(){
    printf("=====================test_sbrk============================\n");
    void* ptr1 = sbrk(0);  // 获取当前堆区的末尾地址
    printf("初始堆指针位置：%p\n", ptr1);

    int* int_ptr = (int*)sbrk(sizeof(int));  // 分配一个int大小的内存块
    if (int_ptr == (void*)-1) {
        perror("sbrk");
        return 1;
    }

    printf("分配的内存地址：%p\n", int_ptr);

    void* ptr2 = sbrk(0);  // 再次获取堆区的末尾地址
    printf("分配后堆指针位置：%p\n", ptr2);

}



void test_sbrk_and_brk(){
    void *curr_brk, *tmp_brk = NULL;

    printf("Welcome to sbrk example:%d\n", getpid());

    /* sbrk(0) gives current program break location */
    tmp_brk = curr_brk = sbrk(0);
    printf("Program Break Location1:%p\n", curr_brk);
    getchar();

    /* brk(addr) increments/decrements program break location */
    brk(curr_brk+4096);

    curr_brk = sbrk(0);
    printf("Program break Location2:%p\n", curr_brk);
    getchar();

    brk(tmp_brk);

    curr_brk = sbrk(0);
    printf("Program Break Location3:%p\n", curr_brk);
    getchar();
}

void test_malloc(){
    void *tret;
    char *pmem;

    tret = sbrk(0);
    if (tret != (void *)-1)
        printf("heap start: %p\n", tret);

    pmem = (char *)malloc(1);  // 分配内存
    if (pmem == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    printf("pmem: %p\n", pmem);

    tret = sbrk(0);
    if (tret != (void *)-1)
        printf("heap size on each load: %p\n", (long)tret - (long)pmem);

    free(pmem);
}

void * naive_malloc(size_t size){
    void* result = sbrk(size);
    if(result == (void *) -1) return NULL;
    return  result;
}

void first_fit(){
    fprintf(stderr, "This file doesn't demonstrate an attack, but shows the nature of glibc's allocator.\n");
    fprintf(stderr, "glibc uses a first-fit algorithm to select a free chunk.\n");
    fprintf(stderr, "If a chunk is free and large enough, malloc will select this chunk.\n");
    fprintf(stderr, "This can be exploited in a use-after-free situation.\n");

    fprintf(stderr, "Allocating 2 buffers. They can be large, don't have to be fastbin.\n");
    char* a = malloc(0x512);
    char* b = malloc(0x256);
    char* c;

    fprintf(stderr, "1st malloc(0x512): %p\n", a);
    fprintf(stderr, "2nd malloc(0x256): %p\n", b);
    fprintf(stderr, "we could continue mallocing here...\n");
    fprintf(stderr, "now let's put a string at a that we can read later \"this is A!\"\n");
    strcpy(a, "this is A!");
    fprintf(stderr, "first allocation %p points to %s\n", a, a);

    fprintf(stderr, "Freeing the first one...\n");
    free(a);
    fprintf(stderr, "first allocation %p points to %s\n", a, a);
    fprintf(stderr, "We don't need to free anything again. As long as we allocate smaller than 0x512, it will end up at %p\n", a);

    fprintf(stderr, "So, let's allocate 0x500 bytes\n");
    c = malloc(0x500);
    fprintf(stderr, "3rd malloc(0x500): %p\n", c);
    fprintf(stderr, "And put a different string here, \"this is C!\"\n");
    strcpy(c, "this is C!");
    fprintf(stderr, "3rd allocation %p points to %s\n", c, c);
    fprintf(stderr, "first allocation %p points to %s\n", a, a);
    fprintf(stderr, "If we reuse the first allocation, it now holds the data from the third allocation.\n");
}

void uaf_test(){
    char *buf1 = malloc(16);
    printf("buf1 addr: %p", buf1);
    char *buf2 = malloc(16);
    free(buf1);

    strcpy(buf1, "Hello"); // Use-after-free vulnerability
    printf("buf1: %s\n", buf1);
    char *buf3 = malloc(16);
    printf("buf3 addr: %p", buf3);

    free(buf2);
    free(buf3);
}

void basic_function_test(){
    //        test_sbrk_and_brk();
//        test_brk();
//        test_sbrk();
//        test_brk();
//        test_sbrk();
//        test_malloc();
//    void *p = naive_malloc(4);
//    printf("naive address: %p\n", p);
    first_fit();
}



void vul_test(){
    uaf_test();
}

int main()
{
    basic_function_test();
    vul_test();
    return 0;

}
