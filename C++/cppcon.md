## 2014

* make simple tasks simple!
  * being too clever is not clever: 太trick或需要思考的代码，后面维护的时候会导致至少、甚至更多的思绪——nightmare!
    * Design for clarity: 
      * 必要的 + 意外的
      * 尽量干掉后者：避免过度抽象、good interface（隔离好脏逻辑）
* [Leak-Freedom in C++... By Default.](https://www.youtube.com/watch?v=JfmTagWcqoE&t=780s)
  * pimpl使用const unique_ptr
  * Fixed-but-dynamic-size array: const unique_ptr<T[]>
  * 使用unique_ptr表示树时，可能会有析构时的stack overflow：n->child->child->chid->...->~Node()
  * don't own "upwards", 以避免loop。这个有点类似lock，don't pass an owner down to "unkown code" that might store it.
  
* [Data-Oriented Design](https://www.bilibili.com/video/BV12x411P7fU?p=2&vd_source=cd2e888c356719a0724eacfa14acabc8)  [知乎解读](https://zhuanlan.zhihu.com/p/34425262) 。

  * Lies:

    * Software is a platform——hardware is
    * code designed around model of the world——model只是我们对现实的模拟
    * code is more important than data

  * Cache 很重要

    * 耗时

      * L1~ 3cycle
      * L2 ~ 20cycles
      * Ram 200 cycles

    * 使用SOA(structures of array)代替AOS（array of structures）。这样要访问的数据都在一起

    * 注意数据排布。

      * ```c++
        struct {
          bool xxx; // 1
          bool yyy; // 2
          Mat mmm; // 4, 由于前面的数据，一个64cacheline无法一次性读完Mat，导致要获取两次
          bool zzz; // 68
        }
        ```

* [back to the basic](https://www.bilibili.com/video/BV12x411P7fU?p=5&vd_source=cd2e888c356719a0724eacfa14acabc8)

  * 如无理由，不要overthink——be default
  * pass by value and std::move inside: only for constructor 其他情况较少适用

* [Make Simple Tasks Simple - Bjarne Stroustrup](https://github.com/CppCon/CppCon2014/tree/master/Presentations/Make Simple Tasks Simple)

* [The Philosophy of Google’s C++ Style Guide](https://www.bilibili.com/video/BV12x411P7fU?p=19&vd_source=cd2e888c356719a0724eacfa14acabc8)

  * 代码规范是为了reader而非writer

* [Lock Free Programing](https://www.bilibili.com/video/BV12x411P7fU?p=44&vd_source=cd2e888c356719a0724eacfa14acabc8)

  * Double check：如果ptr是原子的，就没问题

  * 24:41有个优化版的代码，减少一次atomic读

  * 32:00左右：解决ABA问题（reader）；解决find（find的结果可能被pop删掉了）

    * ```c++
      template<typename T> class slist {
        struct Node{T t; shared_ptr<Node> next;}; // T* -> shared_ptr to solve ABA
        atomic<shared_patr<Node>> head;
      };
      ```

  * to improve scalability, we need to minimize the contention

    * reduce the size of critical sections
    * reduce sharing by isolating threads to use different parts of the data structure
      * moving cleanup from producer to consumer lets consumers touch only the head, producers touch only  the tail
    * reduce false sharing of different data on the same cache line, but adding alignment padding.
      * Separate variables that should be able to used concurrently by different threads should be far enough apart in memory

## 2016

* [A ＜chrono＞ Tutorial](https://www.youtube.com/watch?v=P32hvk8b13M&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=64)
  * 为啥要弄个库：
    * 10ms明确地表示10毫秒
    * 可以快速筛出代码中的时间，和其他用途的数字区分开。并快速切换单位，如s -> ms。f(seconds)
    * 灵活的时间转换比如 `frame = duration<int32_5, ratio<1,60>>`，输出时按ms进行cout
  * 什么时候需要time_point_cast和duration_cast: 隐式转换会丢信息时，比如1234ms->1s