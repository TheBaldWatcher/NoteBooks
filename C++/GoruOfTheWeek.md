

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
  * error code vs exception
    * error code
      * a failure is normal and expected
      * an immediate caller can reasonably be expected to handle the failure
      * an error happens in one of a set of parallel tasks and we need to know which task failed
