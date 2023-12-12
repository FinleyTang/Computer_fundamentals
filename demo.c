
/* sbrk and brk example */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void test_brk();
void test_sbrk();


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

    pmem = (char *)malloc(64);  // 分配内存
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

int main()
{
        test_sbrk_and_brk();
        test_brk();
        test_sbrk();
        test_brk();
        test_sbrk();
//            test_malloc();
        return 0;
}
