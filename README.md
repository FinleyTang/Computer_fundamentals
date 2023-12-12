
## 堆内存

### sbrk and brk

brk 通过增加程序中断位置(brk)从内核中获取内存（初始化非0）。一开始堆段的起始(start_brk)和结束(brk)都指向同一位置。

当 ASLR 关闭时，start_brk和brk将指向数据段 (end_data)的末尾
当ASLR打开时，start_brk和brk指向的位置即为数据段(end_data)的末尾地址加上随机brk偏移地址所指向的位置

sbrk() 系统调用在类Unix操作系统中用于增加或减少进程堆段的内存分配。当以参数 0 调用时，sbrk() 返回当前程序断点位置，即堆段结束的地址。


```c++
void *curr_brk, *tmp_brk = NULL;

printf("Welcome to sbrk example:%d\n", getpid());

/* sbrk(0) gives current program break location */
tmp_brk = curr_brk = sbrk(0);
printf("Program Break Location1:%p\n", curr_brk);
```
![img.png](img.png)

上述程序中的 sbrk(0) 调用返回指向当前程序断点位置的指针，然后使用 printf() 打印出来。注意，%p 格式说明符用于打印指针的值。

这在需要在堆上分配内存并跟踪已分配内存量的情况下非常有用。通过在每次分配之前和之后调用 sbrk(0)，程序可以通过两个值相减来确定实际分配了多少内存。


在内存管理的背景下，“break location”指的是进程数据段（堆）的边界或结束点。它代表动态内存分配的当前限制。

当程序运行时，需要内存来存储变量、数据结构和动态分配的内存（例如在C/C++中使用malloc()或new）。断点位置标记了已分配的内存空间的末端，并确定程序可以分配额外内存的位置。

通过使用诸如brk()或sbrk()之类的系统调用，程序可以调整断点位置，扩展或收缩可用于分配的内存量。当断点位置移动时，程序会相应地获得或释放内存。

因此，断点位置表示程序数据段的当前内存位置结束点，以及在需要时可以分配额外内存的位置。

### 分别使用sbrk和brk函数

首先是sbrk函数的例子：

```c
#include <unistd.h>
#include <stdio.h>

int main() {
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

    return 0;
}
```
现在让我们来看看brk函数的例子：

````c
#include <unistd.h>
#include <stdio.h>

int main() {
void* ptr1 = sbrk(0);  // 获取当前堆区的末尾地址
printf("初始堆指针位置：%p\n", ptr1);

    brk(ptr1 + sizeof(int));  // 将堆区的末尾地址扩展sizeof(int)个字节

    int* int_ptr = (int*)ptr1;  // 使用分配的内存块
    printf("分配的内存地址：%p\n", int_ptr);

    void* ptr2 = sbrk(0);  // 再次获取堆区的末尾地址
    printf("分配后堆指针位置：%p\n", ptr2);

    return 0;
}
````
## 地址随机化

我测试的时候为什么每次初始堆指针地址都是一样的，没有地址随机化吗

> 这是因为现代的操作系统通常会对堆地址进行一定的随机化，以增加程序的安全性。但是，这种随机化不是默认启用的，需要在编译选项中显式指定才能生效。
如果你想在测试中观察到堆指针地址的随机化效果，可以在编译时通过添加 -fPIE 或者 -pie 的选项来开启地址随机化。例如：
gcc -o test test.c -fPIE 这样就可以开启地址随机化，并在每次运行时观察到不同的堆指针地址了。

加上之后还是一样：
> 如果你在编译时加上了 -fPIE 或 -pie 选项，但是运行程序时仍然没有看到地址随机化的效果，那可能是因为你的操作系统默认不开启地址随机化。这种情况下，你可以尝试手动开启地址随机化。

以 Ubuntu Linux 为例，你可以通过以下命令来开启地址随机化：

sudo sysctl -w kernel.randomize_va_space=2
其中 kernel.randomize_va_space 参数表示地址随机化的级别，2 表示开启最高级别的地址随机化。开启后再运行程序，就应该可以看到堆指针地址的随机化效果了。

需要注意的是，开启地址随机化可能会对程序的调试和性能分析造成影响，因此在实际开发过程中需要权衡利弊。


如果你想在 Ubuntu Linux 上关闭地址随机化，可以使用以下命令：

sudo sysctl -w kernel.randomize_va_space=0

### 堆的管理

