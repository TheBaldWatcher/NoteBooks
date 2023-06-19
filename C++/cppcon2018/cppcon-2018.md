
一些杂想：
* c++的default是错误的，所以我们从代码规范、新特性方面进行了各种限制，来表达我们的想法
  * 代码规范：初始化、const、override
  * 默认by value传值
* c++17主要聚焦于易用性、与readability
  * 易用性: fallthrough等attribute、structure binding、if/switch init
  * readability：option、variant、

* 画heap：https://en.cppreference.com/w/cpp/algorithm/ranges/is_heap_until

[toc]





[CppCon 2018: Kris Jusiak “State Machines Battlefield - Naive vs STL vs Boost”](https://www.youtube.com/watch?v=yZVby-PuXM0&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c)

* 用variant实现state machine。不满足开放闭合原则，但其他方式似乎也有这个问题

* ```c++
  class Connection {
    struct Disconnected {}; // use struct instead of enum, can have data
    struct Connecting {};
    struct Connected {};
    //
    variant<Disconnexted, Connecting, Connected> state = Disconnected{};
    constexpr void process_event(Connect const &) {
      visit(overload{
        	[&](Disconnected) { establish(); state = Connecting{}; },
          [](auto) { } // no changes
  	    }, state);
    }
  };
  //
  // 顺带提一下，gcc8.x的汇编有问题，用到了vtable。9+的是inline的。demo如下
  struct Connect{};
  template <class... Ts>
  struct overload : public Ts... {
    using Ts::operator()...;
  };
  template <class... Ts>
  overload(Ts...) -> overload<Ts...>;
  int main() {
      Connection c;
      c.process_event(Connect{});
  }
  ```

[CppCon 2018: Chandler Carruth “Spectre: Secrets, Side-Channels, Sandboxes, and Security”](https://www.youtube.com/watch?v=_f7O3IfIR2k)
* terminology：
  * heartbleed: information leak
  * side channel
* 分支预取会更新cache，导致side channel。17:00有个例子，不过不是特别明白
* variant一些变种
  * #1, if-else 可能会走到非预期的分支. 比如篡改了stack信息、vptr之类的。不是很理解，如果会这样的话，代码正确性不就有大问题了么
  * #2，编译器会将一些代码调用做成switch table，其key可能存在hash collision。理论上可以构造处特定的hash值，导致碰撞后走非预期的函数调用
  * #4，speculative bypass。比如`for (auto &[callback, key] :vec) {callback(key); }`可能会导致callback用了上次循环的key
* Nothing to steal，or no one to steal -> nothing to secure
* mitigation
  * prerequisite: you need the OS mitigations
* 杂项：
  * 分支预测对代码的加速有20x。1:20:40
  * 分支预测在被终止前肯能有上百个指令。1:22:24

[CppCon 2018: Fred Tingaud “A Little Order: Delving into the STL sorting algorithms”](https://www.youtube.com/watch?v=-0tO3Eni2uo&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=10)
* 对于N个元素，我们只排序前k个。——partial_sort居然比sort耗时多了约50%，nth_element+sort只有全局sort的50%，我们来看看为什么
* 复杂度：
  * sort：O(N)；
  * partial_sort：O(N * log(k))。即意味着partial_sort复杂度有两个自由度：
    * 如果是找中位数，k=N/2，复杂度为O(N * log(N))
    * 如果是在海量数据里找top10，则复杂度为O(N)
  * partial_sort vs nth_element_sort: O(N * log(k)) vs O(N + k * log(k))
  * 所以当k很小时，建议使用partial_sort，当k较大时，使用nth_element_sort——14:11
    * 举例：log按log10处理，以N=10e4：
    * k = 10: partial vs nth: N vs N + 10, partial胜出
    * k = N/10: partial vs nth: N(logN - 1) vs 0.9N + 0.1N * logN, nth胜出


[Bob Steagall “Fast Conversion From UTF-8 with C++, DFAs, and SSE Intrinsics”](https://www.youtube.com/watch?v=5FQ87-Ecb-A&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=16)
* 一个关于utf-8转换的DFA实现。条例蛮清晰的
* 实现
  * step 1. 使用lookup table做DFA的状态跳转
  * step 2. 优化：ASCII避免查找
  * step 3. 优化：输出是32bit的，所以对于ASCII可以进行并行，SSE
  ```
  // 45:50.  1:06:00似乎提到汇编代码AVX能更高效的一些
  zero = _mm_set1_epi8(0);  // 16-bit zero
  chunk = _mm_loadu_si128((__m128i const *) pSrc);  // load
  mask = _mm_movemask_epi8(chunk);  // 最高位bit是否为1的mask

  half = _mm_unpacklo_epi8(chunk, zero);  // 拼接：ab cd -> a0 b0 c0 d0
  qrtr = _mm_unpacklo_epi16(half, zero);  // 拼接：a0 b0 -> a0 00 b0 00
  ```

[Steven Simpson “Source Instrumentation for Monitoring C++ in Production”](https://www.youtube.com/watch?v=0WgC5jnrRx8&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=19)
* 什么时候是production ready?: observable
* instrument的好处
  * monitor, measure performance, diagnose error
* logging
  * 建议使用structure logging，因为对后续的日志的处理更友好，避免写一堆regex
  * 细化一点就是tracing了。logging更偏向debug，解决当前问题；tracing更偏向monitor，更偏向应对将来问题
    * begin, end, span
    * 层级递进: `auto t2 = t1.trace(...)`, 并可以和error联合使用，把底层的error和最上层的调用联系起来
* metrics
  * 历史、告警

[Nir Friedman “Understanding Optimizers: Helping the Compiler Help You”](https://www.youtube.com/watch?v=8nyq8SNUTSc&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=22)
* ⭐️：lambda、by-value、template枚举bool的true、false，39:00
* ```
  // case 1: lambda make things faster。
  // 39:29 因为：传函数指针时，compiler能看到的只是一个value，没有类型信息；传lambda时是有类型信息的，可以进一步优化。
  bool my_cmp(double x, double y) {return x > y;}
  //
  void g(vector<double>& v){
    std::sort(std::begin(v), std::end(v), my_cmp);
  }
  // faster, my_cmp is inline
  void f(vector<double>& v){
    std::sort(std::begin(v), std::end(v), [] (auto x, auto y){
      return my_cmp(x,y);
    });
  }
  //
  // case 2: 不一定更好：虽然平均耗时可能降了，但是在最坏情况下，我们多了一个if的开销；而我们往往更关心这种长尾耗时
  int fast_modulues(int x, int m) {
    if (x < m) return x;
    return x % m;
  }
  //
  // case 3: pass-by-ref 不利于优化. 当by-value传b时，if可以被挪到for外，这样只用比较一次，但是by-ref无法这么做。
  void qux(vector<int> const&x, bool const &b) {
    for (auto e:x) {
      bar();
      if (b) foo(e);
    }
  }
  bool my_bool = true;
  void bar() { my_bool = !my_bool; }
  qux(x, my_bool);
  // 顺带说下，const没有被compiler考虑
  void foo(const int&);
  bool bar(int x) {
    const int y = x+1;
    foo(y);  // 除非改成传值，不然y>x还是会进行比较，而不是返回true
    return y > x;
  }
  ```
* inline是最核心的优化。不仅是因为避免了函数调用，更重要的是compiler能有更多信息去优化。。

[Jonathan Boccara “105 STL Algorithms in Less Than an Hour”](https://www.youtube.com/watch?v=2olsGf6JIkU&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=32)
* 一个stl的视频
```
// inplace_merge, 可以用来做归并排序 c++17

// is_*:        is_sorted,      is_partitioned,       is_heap
// is_*_until:  is_sorted_until,              , is_heap_until

// *_copy: remove/unique/reverse/rotate/replace/partition/partial_sort

// accumulate/(transform_)reduce

// partial_sum/(transform_)[inclusive|exclusive]_scan

// mismatch

// adjacent_find

// fill/generate/iota/replace

// uninitialized_*: fill/copy/move
// destroy
```

[Greg Law “Debugging Linux C++”](https://www.youtube.com/watch?v=V1t6faOKjuQ&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c&index=38)
* ctl +x +a: 切换到窗口模式
* ctl +x +o: 切换焦点
* ctl +x +1: 显示src
* ctl +x +2: 显示src+asm