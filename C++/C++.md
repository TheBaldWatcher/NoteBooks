

cppcon视频：https://www.youtube.com/user/CppCon/playlists?reload=9

* c++ blog:
  * [链接](https://brevzin.github.io/posts-by-tags/#c++20)。从c++20 `<=>`进来，感觉写的还不错，其他还没看。
* 一些待看视频：
  * https://www.youtube.com/watch?v=WjTrfoiB0MQ
  * https://www.youtube.com/watch?v=kIoZDUd5DKw
  * https://www.youtube.com/watch?v=yy8jQgmhbAU
  * back to the basic：https://www.youtube.com/watch?v=OtU51Ytfe04
* 一些待看的资料：
  * c++单元测试：https://github.com/boost-experimental/ut
  * 似乎是实现类似hana的一些功能：https://eliasdaler.github.io/meta-stuff/
  * https://abseil.io/tips/
    * 尽量使用：
      * string_view
      * hex floating: 0x5'1p2
      * delegated init: const Point p  {.x = 0x5'1p2};
      * structured binding：for (const auto& [name, age] : ages_by_name)
    * private constructor的类可以考虑用工厂模式。#42
      * 如果涉及到make_unique/shared时，用new XXX的接口；或者用passkey idom。#134
    * 单元测试尽量不要fixture：尽量简单，且是free function。写.h前写下test可以看看api是否好用#122
    * api的bool可以考虑改为enum。#94
    * using namespace建议用全路径。#119
    * 除了copy、move构造函数，建议都explicit。#142
    * 对于function args，在declaration中，top-level的const会被略去；只在definition时才是保留的。#109
    * delete也是函数定义，除了方法，也可以用于函数上；比如避免一些重载：void func(string);void func(char) = delete;或者是避免new[]。#143
    * 关联容器异构key查询：cmp中using is_transparent = ???。可直接用less<>。c++14支持ordered，c++20支持unordered#144。
    * 不要在return后去访问可能已经析构的变量。主要是指一些RAII的情况。#120
    * absl:optinal用于small或者owned（如类成员和return value）。#163
    * 尽量少用bind：bind(void func())给function<void(int)>用，编译器不报错，即：少参数也compile（使用bind_front）；另外void cb(function<void(int)> f)中f如果是bind的，可能和预期的调用不一样。#108
    * copy elision: non-named也可能copy，如构造时是传给base、small-obj
  * gdb：https://www.kancloud.cn/wizardforcel/gdb-tips-100/146748
* blog：
  * https://abcdabcd987.com/sharding/

### [年付 $15 阅读全部 O'Reilly / safari 的书](https://www.douban.com/note/743690382/)。

# macro

* 坑

  * [宏替换](https://bbs.csdn.net/topics/392010394)：非#开头的宏，会先替换其内部的宏；#开头的宏，直接转成字符串，不再向内替换。宏替换后会不继续尝试替换，但也有防递归处理

  ```c++
  #include <stdio.h>
  #define f(a,b) a##b
  #define g(a)   #a
  #define h(a) g(a)
  printf("%s\n", h(f(1,2)));   // 12
  printf("%s\n", g(f(1,2))); // f(1,2)
  printf("%s\n", g(h(f(1,2)))); // h(f(1,2))
  printf("%s\n", h(g(f(1,2)))); // "f(1,2)"
  printf("%s\n", h(h(f(1,2)))); // "12"
  
  #define concat(s,t) s##t
  #define AAA ABC
  concat(A, AA);	// ABC
  #define ROOT AAA CCC
  #define AAA ROOT
  ROOT;		// ROOT CCC
  ```

* switch：

  ```c++
  int a=10;
  switch(a)
  {
    case '1':
      printf("ONE\n");
      break;
    case '2':
      printf("TWO\n");
      break;
    defau1t:		// 笔误，不是default，但switch不会报错，而是认为是标号defau1t
      printf("NONE\n");
  }
  ```
* strict aliasing：
  * https://stackoverflow.com/tags/strict-aliasing/info
  * https://gist.github.com/shafik/848ae25ee209f698763cffee272a58f8
    * 使用memcoy到临时变量，绕后靠编译器优化掉个临时变量。bit_cast在处理`sizeof(From)!=sizeof(To)`时也得这样。
# Hana

* 视频https://www.youtube.com/watch?v=L2SktfaJPuU&list=PLHTh1InhhwT7esTl1bRitiizeEnksGU7J&index=24
* 一个非constexpr对象也可以提取出constexpr信息。——hana使用`value-level`的语法来进行`type-level`的计算。
* 不要依赖hana函数的地址：do not rely on the uniqueness of the address of any global object provided by Hana

```c++
// Variable template
template <int N> const int ctSquare = N*N;
// compile-time branch
hana::if_;
hana::eval_if;
// function lookup table
/*
* 1. User-defined literals: convert "foo"_s to a class
* 2. define a template constexpr hana::basic_type<Signature> function{}, for function register
* 3. overload operator=(), to return a hana::make_pair
*/
"foo"_s = function<void ()>;
```

# template

* [Ugly Number II](https://leetcode.com/problems/ugly-number-ii/):使用到template的一些特性，可以拿来练手，但感觉写的不是很完美，后续可以看看能否优化

* `void_t<T>`可以用来检查T是不是well-formed，以便选择更好的偏特化，比如用于has_xxx

# 技巧

* 返回多个值std::tie
* 后缀返回auto func ()-> Type
* -Wmissing-field-initializers: list_initialization在struct添加成员后，编译时不会报错。该选项可以检查这个
* [不同std是可以link的，但不同gcc版本的lin有限制](https://stackoverflow.com/questions/46746878/is-it-safe-to-link-c17-c14-and-c11-objects)。





# 资料

* 我想了解下，有很熟悉无状态服务这块的童鞋，对于无状态服务 这块的一个标准处理流程。

  * 你要是单纯学习，可以看几种case: 1. 读写谁是主要的,\2. 强弱一致性,3. 隔离级别.

  * 这几个有几个经典设计问题: \1. feed流系统设计 2. 秒杀 3. 已读服务

  * 偏技术理论一点的看 DDIA(Design Data Intensive Applications)，这个是目录 偏业务系统微服务设计，看micro service patterns，没业务背景会难一点

    更多

  * 比较好看的case可以看几个

    * \1. https://blog.cydu.net/weidesign/2012/08/30/weibo-counter-service-design-1/  chuanying的这个讲weibo counter设计的
    * \2. https://www.infoq.cn/article/ZsPwFEvI9eUNF-IccCUV 知乎的已读服务设计
    * \3. twitter/weibo的feed设计，当然字节这个设计的也还不错(还结合了推荐)

* llvm的libcxx还好，gcc的libstdc++东西太多了，有些东西还是可以直接用的，有个prof mode，可以告诉你你的stl数据结构用得合理不合理。google：https://gcc.gnu.org/onlinedocs/libstdc++/manual/profile_mode.html

  * 我给你们找一下那个DoD的震撼视频...可惜这个大佬不show代码 ：https://www.youtube.com/watch?v=yy8jQgmhbAU&t=2s

* modern c++：http://www.modernescpp.com/index.php

* 持久化内存：pmdk，https://pmem.io/book/#TOC



# Effective Mordern C++

* Rvalue Reference, Move Sematics, and Perfect Forwarding
  * move vs forward:
    * move无条件cast，forward有条件cast。
    
    * forward可以做到move所有的工作。但二者区分对待也是有原因的：move更简单(一个参数)，forward多出来一个模板参数。另外，forward一般意味着把参数透传给下一函数，并保留其左/右值属性
    
    * 进一步来说：xvalue用move，glvalue（general ref）用forward。别对`const`使用`move`/`forward`，一方面意思相悖；另一方面实际上走的是l-ref的接口，而非预期的r-ref。
    
    * general ref(g-ref) template：
    
      * 避免维护两个版本非模板版本：`const T&`和`T&&`,随着参数增长，这个工作量是$2^n$倍的
    
      * 另外，维护两个非模板版本时，得写明参数类型。这可能导致多余的临时变量：
    
        ````c++
        struct A {
          template <typename T>
          A(T&& t){ name = forward<T>(t);}
          // don't do following 
          // case: A("some string"): rval-constructor, move, rval-destructor
          A(string const &t) { name = t;}
          A(string &&t) { name = move(t);}
          
          string name;
        };
        ````
    
      * 。但也带来个问题：g-ref版本没有`const`限定了。这个时候forward就很重要了，不然，使用move也会影响l-ref的版本，导致参数非预期的被move。



# 杂项

* [value categories](https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues):  i: indentifiable, I: unidentifiable, m: movable, M:unmovable
  
  * ```
        ______ ______
       /      X      \
      /      / \      \
     |   l  | x |  pr  |
      \      \ /      /
       \______X______/
           gl    r
           
    ```
  
  * iM: lvalue, im: xvalue——i: glvalue, i.e. gl = l + x
  
  * Im: prvalue——m: rvalue, i.e. r = pr+x
  
* [execute policy](https://www.bfilipek.com/2018/11/parallel-alg-perf.html): heavy CPU& light I/O task

* hash:

  * `hash<T>`:
    * [A good hash function should](https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key/57556517#57556517):
      * be bijective to not loose information, if possible and have the least collisions
        * impl时会做乘法，由于要双射，即要求逆元，所以要乘奇数（`a mod N有乘法逆元<=> gcd(a,N) == 1`）。
      * cascade as much and as evenly as possible, i.e. each input bit should flip every output bit with probability 0.5.
    * [Mixer](http://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html)。
  * `hash<pair<T,U>>`:
    * `return seed ^= hasher(v) + Const_A+ (seed<<6) + (seed>>2);  `——boost::hash_combine.
      * `Const_A`:$let phi  = (1 + sqrt(5)) / 2; 2^32 / phi = 0x9e3779b9 (for 32-bit)$。[链接](https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine)。其每一位都等可能性的为0|1。
      * [这里](https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values)似乎有一个更好的实现，但目前没工夫深究。
    * `hash<pair<T,U>> = hash<T> ^ hash<U>`这种实现存在问题
      * `hash<T>`如果本身分布不好，对某一值(如A)有倾向性，则`A ^ A -> 0`容易得到0值。
        * 假设分布为：对值A有$k/n$几率，对其他值有$1/n$几率。则hash为0的几率为$k^2/n^2 + \sum_{n-k}1/n^2=(n-k+k^2)/n^2$。
        * 因此我们本身要加一些噪音，即`Const_A`
      * 对称性：`hash(a,b)==hash(b,a)`，因此要有非对称的`<<`和`>>`。

* read/write lock: c++17有shared_lock。面试可能会问。参考实现[无锁](https://gist.github.com/mshockwave/b314eb78d4019e7e106e705e864e0398)；

  * ```c++
    //  https://blog.csdn.net/zxc024000/article/details/88814461
    class RWLock {
     public:
     public:
      RWLock(mutex &mtx) : mtx(&mtx);
      void RLock() {
        lock_guard<mutex> lck(mtx);
        cv_read_.wait(lck, [&]() { return writers_ == 0; });
        ++readers_;  // ++ after cv.wait, so new coming RLock won't block cur
                     // waiting WLock
      }
    
      void WLock() {
        lock_guard<mutex> lck(mtx);
        ++writers_;  // not like RLock, ++ is before cv.wait
        cv_write.wait(lck, [&]() {
          return readers_ == 0 &&  // until pre RLock all finish
                 !is_writing_;     // win competing among other WLock
        });
        is_writing_ = true;
      }
    
      void RUnlock() {
        lock_guard<mutex> lck(mtx);
        if (--readers_ == 0 && writers_ > 0) {
          cv_writer.notify_all();
        }
      }
    
      void WUnlock() {
        lock_guard<mutex> lck(mtx);
        is_writing_ = false;
        if (0 == --writers_) {
          cv_readers.notify_all();
        } else {
          cv_writers.notify_one();
        }
      }
    
     private:
      std::condition_variable cv_read_;
      condition_variable cv_write_;
      mutex *mtx = nullptr;
      bool is_writing_ = 0;
      int readers_ = 0;
      int writers_ = 0;
    };
    ```

  * 







to_check details later:

* c++20: `<=>`(mingw似乎还没支持)，中文参考[1](https://zhuanlan.zhihu.com/p/101004501?utm_source=com.evernote),[2](https://blog.csdn.net/longji/article/details/104017451),[3](https://www.cnblogs.com/ysherlock/p/9629599.html)；
  * 外文[参考](https://brevzin.github.io/c++/2019/07/28/comparisons-cpp20/):
    * 之前：`<,>,==,<=,>=,!=`虽然逻辑上相关，但代码上相互独立。一般来说，我们需要由`<,==`作为基础，实现其他4个。但：
      * `a<=b`对partial order和total order有坑：total order可以写`!(b<a)`，但partial order不可以（考虑float和NaN），得写成`a==b || a<b`，这变成了两次函数调用。
      
      * 此外：对于heterogeneous comparison，得写一个friend的non-member版本`operator<(X,int)`，而且还得小心implicit conversion: Y->X。
      
      * |                           | Equality | Ordering             |
        | ------------------------- | -------- | -------------------- |
        | **Primary**（reversible)  | `==`     | `<=>`                |
        | **Secondary**(rewritable) | `!=`     | `<`, `>`, `<=`, `>=` |
      
        





