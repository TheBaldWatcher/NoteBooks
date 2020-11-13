# Linux 多线程服务端编程：使用 muduo C++ 网络库

  * 线程安全的对象生命期管理
    * 构造：不要泄露this指针：不注册回调，不外传this指针，即便是最后一行
      * 如果要注册回调，考虑二段式构造：ctor+init。这样构造函数可以变为`noexcept`。
    * 构造：mutex无法保护析构函数——析构函数会把mutex销毁掉，此时线程不安全。需要一个在析构后还存在的东西
      * observer模式下，`observable`和`observer`互相注册回调时，不知道对方是否析构，需要用智能指针，但有不希望影响对方生命周期（循环引用），所以用`weak_ptr`
      * observor模式下，`obj->update`需要保证不会有线程在析构。使用`weak_ptr.lock`来保证。
      * `shared_ptr`读是线程安全的，写不是（析构算作写）。有读/写操作时，需要加锁。
    * 加锁后的`shared_ptr`在临界区中，尽量用`swap`避免触发析构。
    
* 多线程服务器的适用场合和常用编程模型
  * I/O: event-loop: one loop per thread + thread pool
  * 适用多线程（相较于多进程）场景：任务轻量，有可变的共享数据，任务有优先级、异步操作、CPU或I/O未达到瓶颈。

* c++多线程系统编程精要
  * 线程的创建与销毁守则：
    * 创建：
      * 要明确告知，以便上层规划资源；
      * main函数前不应启动线程，不然如果线程访问了其他全局变量，会产生初始化顺序依赖，极难维护；
      * 最好是一开始就创建好（线程池）
    
    * 析构：
      * 自然死亡、非自然死亡（throw），自杀，他杀。后面几个都不太好
      * 自杀（exit）会析构全局对象。可能导致死锁（lock - exit - ~T() - lock），或者其他正在访问全局变量的线程崩溃。
      * 由于全局变量的变量在多线程的析构可能有问题，所以一个退出办法（非正常退出）是，拒绝后续的服务，然后杀掉
    
  * RAII不一定能fork后正常工作。thread也是类似。
  
  * 多线程是不要使用signal，#4.10
  
* 6-9章略过

* 反思c++面向对象与虚函数

  * 二进制兼容性：如果以静态库/源码发布，则无下列问题
  * 以虚函数作为借口，会导致兼容问题：虚函数调用是依靠offset（non-virtual是按函数名），虚函数变动可能导致offset变更，导致新旧不对应。
  * 可以考虑pImpl，对外暴露non-virtual接口，在.cc文件中再调virtual函数。
  * 更本质的看法上，避免使用virtual，避免使用继承（除非接口完全固定，后续不会有变动）。使用function来完成相应的任务
    * rust没有继承，而是tag，或者trait。（或者可以理解为细粒度时的base。）

* c++经验谈

  * 不要重载`::operator new`：在library中重载会影响上层调用；在上层调用中重载，不会影响library，甚至遇到inline function会导致UB。——可以考虑替换malloc
  * 为单独的的class重载`new`，不建议。虽然影响小了很多，只影响base、derive，但与"最小惊讶原则"相悖。
  * 单元测试中mock系统调用<font color=red>未细看，后续再翻阅</font>。
  * 如何学习网络编程：①TCPv1 + UNPv1(可中文) + Effective Tcp/Ip Programming(参阅TCPv2 + Pattern-Oriented Software Architecture Volume 2)；②熟悉参数设置和优化；③自己实现各TCP/IP协议。
    * 比较好的例子：echo、chat、proxy。
    * 可以借助Ipython：4个窗口，log+strace+通信对方+tcpdump
  * 附录中有一些TCP的思考题