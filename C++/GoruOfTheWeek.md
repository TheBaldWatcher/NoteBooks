

Gotw

* [#94 AAA](https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/)——AlmostAlwaysAuto多用auto
  * 使用`auto str1 = "xxx"s`而非`string str2 = "xxx"`来赋值。一个是一致性，另外就是`"xxx\0\0yy"`用后一种，str2是`"xxx"`。
* [#2 temprory objects](https://herbsutter.com/2013/05/13/gotw-2-solution-temporary-objects/)
  * SingleEntry/SingleExit只有前半句是对的，后半句甚至会有损性能。因此没有必要` string ret; … ret = i->addr; break; … return ret;`，直接` return i->addr; break; … return {};.`即可
* [#4 Class Mechanics](https://herbsutter.com/2013/05/20/gotw-4-class-mechanics/)： 一个书写良好的类的示例
* [#5 overriding virtual functions](https://herbsutter.com/2013/05/22/gotw-5-solution-overriding-virtual-functions/)
  * NonVirtualInterface: virtual 和 public二选一. non-virtual public Or virtual protect(not private)
* [# 6const correctness](https://herbsutter.com/2013/05/24/gotw-6a-const-correctness-part-1-3/)
  * Mutable: writable but logically const
    * 多线程读安全、多线程写安全
    * M&M rule: mutable and mutex(atomic) go together
  * *Prefer to make variables, including locals,* **const** *if they should not change.*
* [#7 minimizing compile time dependencies](https://herbsutter.com/2013/08/19/gotw-7a-solution-minimizing-compile-time-dependencies-part-1/)
  * 不要redeclare std中的东西，主要是implementation导致的
    * because library implementations are allowed to do things like add their own extra template parameters beyond those required by the standard that of course your code wouldn’t know about—which is one of the **primary** reasons for the rule that programmers aren’t allowed to write their own declarations for things in namespace std.
  * 使用`#incldue <iosfwd>`代替`#include <i/o/iostream>`
  * 使用pimpl，不过会引入一层间接寻址和堆内存分配
  * 话说，尽量避免forward，可参考cppcon 2017，Live at HEAD
* [#89 Solution: Smart Pointers](https://herbsutter.com/2013/05/29/gotw-89-solution-smart-pointers/)
  * 使用makr_shared而非shared_ptr
    * 减少1次内存分配：减少内存碎片、内存分配overhead、分配是有锁的
    * 内存locality
  * 这个在c++14里有问题，但在c++17里strict evaluation order后，已经没问题了
    * `func(unique_ptr<widget>{new widget{}}, OtherFunc())`
* [#90 Solution: Factories](https://herbsutter.com/2013/05/30/gotw-90-solution-factories/)
  * 工厂别返回裸指针。建议返回unique_ptr
* [#GotW-ish Solution: The ‘clonable’ pattern](https://herbsutter.com/2019/10/03/gotw-ish-solution-the-clonable-pattern/)
  * 如何实现clone接口
