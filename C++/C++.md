# 读书

* 一些c++17、20的例子，看上去还比较有趣  https://www.cppstories.com/2018/06/variant/
* 查llcm-mca时找到的资料，好像是高性能计算的，瞅瞅https://en.algorithmica.org/hpc/profiling/



https://www.codingninja.co.uk/best-programmers-quotes/

c++blog https://brevzin.github.io/c++/2019/07/28/comparisons-cpp20/





cppcon视频：https://www.youtube.com/user/CppCon/playlists?reload=9

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
      * ~~string_view~~
      * hex floating: 0x5'1p2
      * Designated init: const Point p  {.x = 0x5'1p2};
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

### ~~[年付 $15 阅读全部 O'Reilly / safari 的书](https://www.douban.com/note/743690382/)。~~

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
    * cv，using/unsigned、char类型没有问题。
    * 如何正确type tune：memcpy
    * Wstrict-aliasing=1，Wall的等级是3，有些会报不出来
  * https://stackoverflow.com/questions/98650/what-is-the-strict-aliasing-rule/7005988#7005988
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

* -Wmissing-field-initializers: list_initialization在struct添加成员后，编译时不会报错。该选项可以检查这个
* [不同std是可以link的，但不同gcc版本的lin有限制](https://stackoverflow.com/questions/46746878/is-it-safe-to-link-c17-c14-and-c11-objects)。
* [unordered_set<function>](https://stackoverflow.com/questions/18039723/c-trying-to-get-function-address-from-a-stdfunction)。
* `void_t`主要是作为模板里的dummy type。[理论上](https://stackoverflow.com/questions/31745818/why-is-void-t-necessary-to-check-for-the-existence-of-a-member-type)，也可以是int或者其他类型



## smart_ptr

* [shared_ptr实现源码](https://www.cnblogs.com/ukernel/p/9191157.html)，面试前可以看下
  
  * 存在weak_ptr时，被管理对象T可能被释放，但管理对象M一直要等到weak_count也归零
  * Make_shared只有一次堆内存分配，shared_ptr(new T)有两次
  
* const and &
  
  * ```c++
    void sink(unique_ptr<T>);	// sink, consume
    void reseat(unique_ptr<T>&); // will or might reseat, in-out parameter
    // void thinko(const unique_ptr<T>&) meaningless
    
    void share(shared_ptr<T>);	// share, will retain refcount
    void reseat(shared_ptr<T>&);	// will or might reseat, in-out parameter
    void may_share(shared_ptr<T> const&);	// might retain refcount. -- the ptr and refcount is not const
    ```
  
* don't deref non-local pointer. 如果是global的指针，可能在别处，甚至在当前函数中被释放掉，而此时我们接下来可能会使用这个dangling ref

## auto

* Correctness, maintainability, performanc, usability——cppcon 2014, back to the basics

  * Correctness: 

    * 左右两边不匹配，比如`vec::non_const_iter = const_vec.begin()` ——编译期

      * ```c++
        string s1 = "123\0\0aa";  // 123
        auto s2 = "123\0\0aa"s;  // 123  aa
        ```

    * 不会写出未初始化变量——运行期

  * Maintainability: 比如类型变更`int i = f()*42`，f的类型从int变为了float

  * performance: no implicit conversion

  * usability:hard to  write,比如迭代器

* [gotw 94](https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/)

  * 会有些可读性上的问题。

    * 但和收益相比是可接受的。另外`auto x = T{};`也是可以标明类型的

    * 日常工作中我们也常常忽略具体类型——是面向接口编程思想的延伸：屏蔽type与data。

      * ```c++
        // 如下例子，一个类型也没有
        template<class Container, class Value>
        void append_unique( Container& c, const Value& v ) {
            if( find(begin(c), end(c), v) == end(c) )
                c.emplace_back(v); 
            assert( !c.empty() );
        }
        ```

      * 



# CRTP

* https://zhuanlan.zhihu.com/p/137879448
* 子类实例创建、析构计数
* 多态链。即函数返回的是子类，这样可以调用子类方法
* 多态的clone函数


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



# C++17

* std::launder

  * https://wanghenshui.github.io/2019/04/27/launder.html
  
  * https://stackoverflow.com/questions/39382501/what-is-the-purpose-of-stdlaunder
  
  * memory launder避免编译期追踪内存是从哪儿来的，避免一些优化。用于const T*
  
* Evaluation order

  * ```c++
    std::string s = "I heard it even works if you don't believe"; s.replace(0,8,"").replace(s.find("even"),4,"always").replace(s.find("don't believe"),13,"use C++17");  // it always works if you use C++17
    
    int i = 0;
    std::cout << ++i << ' ' << --i << '\n';	 // 1, 0
    ```

* Template Features

  * ```c++
    // Deduction Guide
    template <typename T>
    struct C {
      C(const T&) {};
    }
    
    C x{"hello"}; // T deduced as char[6], NOT DECAYED
    template<typename T>C(T) -> C<T> // T deduced as const char*
    
    // Constexpr if
    // case 1: perfect return of a generic value
    template<typename Callable, typename... Args>
    decltype(auto) call(Callable op, Args&&... args) {
      if constexpr(std::is_void_v<std::invoke_result_t<Callable, Args...>>) { // return type is void:
        op(std::forward<Args>(args)...);
        return; 
      } else { // return type is not void:
        // 如果没有上一个分支, 这一行在函数返回类型为void时会报错
        decltype(auto) ret{op(std::forward<Args>(args)...)};
        return ret;
      }
    }
    
    // Fold Expression
    // unary left fold: (... op args) -> (arg1 op arg2) op arg3
    // unary right fold: (args op ...) -> arg1 op (arg2 op arg3)
    // case1: 打印, 要加上空格
    template<typename First, typename... Args>
    void print(const First &first, const Args&... args) {
      cout << first;
      auto outWithSpace = [](const auto & arg){
        cout << ' ' << arg;
      };
      (..., outWithSpace(args));
      cout<<"\n";
    }
    // case2: 基类打印
    template <typename... Bases>
    class MultiBase : private Bases... {
      public:
        void print() {
          (..., Bases::print());
        }
    };
    // case3: Tree遍历
    struct Node {
      int val;
      Node *left{nullptr}, *right{nullptr};
      
      template<typename T, typename ...TP>
      static Node* traverse(T np, TP... pathes) {
        return (np-> *... -> *pathes);  // np ->* paths1 ->* paths2 ...
      }
    };
    Node* node = Node::traverse(root, Node::left, Node::right);
    
    // Template Parameters
    // using auto for non-type template parameter
    template <auto N> class S {};
    S<42> s1;
    S<'a'> s2;
    // simplify code
    template <typename T, T... Elements>
    struct sequence {};
    using indexes = sequence<int, 0, 3, 4>;
    
    template<auto... Elements> 
    struct sequence_2 {};
    using indexes_2 = sequence_2<0, 3, 4>;
    
    // Extended Using Declarations
    // case1 simplify code
    template<typename... Ts>
    struct overload : Ts... {
      using Ts::operator()...;
    };
    // why need this deduction guide? re-read the book
    template<typename... Ts>
    overload(Ts ...) -> overload<Ts...>;
    
    auto twice = overload {
      [](string& s) {s += s;},
      [](auto &v) {v *= 2;}
    };
    
    ```

* New Libaray Components

  * Optional

  * ```c++
    // case 1: return values
    // careful 1: accessing the value: `value()`， `operator *` is reference
    for (int i : getVector().value()) {
      cout << i<<"\n";  // oops, i is refered to deleted obj
    }
    // careful 2: value_or return by value
    cout<< o.value_or("fall_back"); // nice interface, but maybe inefficient
    cout<< o ? o->c_str() : "fallback";
    // careful 3: nullopt is less than non-nullopt
    optional<bool> bo;
    bo < false ; // true
    optional<size_t> so;
    so < 0; // true
    // careful 4: optional of boolean or pointer, so, use function `has_value`
    optional<bool> ob{false};
    if (!ob) {...} // !ob is false
    if (ob == false) {} // ob == false is true
    ```

  * variant

    * Motivation: union不知道持有的类型、不能有non-trivial、不能derive。可以搞个限定类型集合的polymorphism
      * comparing variant polymorphism
      * pro: 不需要基类及其接口(不过这个比较反直觉)、不需要virtual、不需要pointer(值语义，因此没有指针的一些问题，如freed memory, memory leak, no heap allocation)
      * con: 限定类型（增删维护不方便）、按最大元素分配内存、copy可能会比较expensive


    ```c++
    // visit
    // 方法1
    auto generic_lambda = [](auto &val) {
      if constexpr(is_convertible_v<decltype(val), string>) {val = val + val;} 
      else { val *= 2;}
    }
    visit(generic_lambda, var);
    // 方法2 参考template feature中的overload
    auto twice = overload{
      [](string&) {},
      [](int i) {},
    };
    visit();
    
    // careful: string literal converts better to bool than string
    variant<bool, string> v;
    v = "hi"; // v.index is 0
    ```

  * string_view

    ```c++
    // careful 1: 不是\0结尾
    // careful 2:  要保证被指向的string生命周期够长
    // case 1: Do Not return string_view to strings
    auto a = createPerson().getName(); // string_view getName() const { return name; }
    // case 2: function templates should use return type AUTO
    string operator+(string_view sv1, string_view sv2) {return string(sv1) + string(sv2);}
    template<typename T>
    T concat(const T& x, const T& y) {return x + y;}
    string_view hi = "hi";
    auto xy = concat(hi, hi); // T is deduced as string_view
    
    // Summary of safe use of string_view
    // 1. Do not use string view in APIs that pass argument to a string
    //   * Do not initialize string members for string_view parameters
    //   * No string at the end of a string_view chain
    // 2. Do not return a string view
    // 3. function templates should NOT return generic type T
    ```

  * std

    * parallel:
      *  accumulate
        * reduce: associative
    
      * Inner_product
        * transform_reduce: non-associative比如平方求和。以便将元素变换、和求和操作分开
    
      * partial_sum
        * inclusive_scan, exclusive_scan
      
    * string
      
      * boyer_moore_searcher，boyer_moore_horspool_searcher时间换空间
    
    * other utility function
      
      * as_const, clamp
      * sample
      
    * containers
      
      * ```c++
        // map, set可以改变key了.
        auto nh = m.extract(2);
        nh.key()=4;
        m.insert(move(nh));
        // merge
        dst.merge(src);
        
        // emplace_back有返回值了
        foo(myVector.emplace_back(...));
        // try_emplace 在emplace失败时，不会move
        m.try_emplace(42, std::move(t1));
        // insert_or_assign和try_emplace相反，必定会move。
        m.insert_or_assign(42, move(t1)); // 等价m[42] = move(t1), 但可以拿到iter，即少一次find。另外避免默认构造，然后赋值；是一步到位
        ```
      
    * Multi-thread
      
      * ```C++
        // 非精确值，hint
        hardware_destructive_interference_size; // 不在一个cache line的最小size
        hardware_destructive_interference_size; // 在一个cache line的最大的size
        ```
    
  * expert utilities

    * pmr
    
        ```c++
        ///////////////////////////////
        //// memory resources /////////
        ///////////////////////////////
        // new_delete_resource: singleton. new & delete
        // null_memory_resource: singleton. throw
        //   synchronized_pool_resource: thread-safe
        // unsynchronized_pool_resource: not thread-safe
        // monotonic_buffer_resource: not thread-safe
        array<byte, 2'000'000> buf;
        pmr::monotonic_buffer_resource pool{buf.data(), buf.size()}; // 超出buffer后，会退化为new
        std::pmr::vector<std::pmr::string> coll {&pool};
        
        // monotonic_buffer_resource可以用来跳过deallocation
        {
          pmr::monotonic_buffer_resource pool;
          pmr::vector<pmr::string> coll{&pool};
          for (int i = 0; i < 100; ++i) {
            coll.emplace_back("just a non-SSO string");
          }
          coll.clear(); // destruction but no deallocation
        } // deallocates all memory
        
        /////////////////////////////////////////
        //// defineing custom memory resource ///
        /////////////////////////////////////////
        class Tracker : public pmr::memory_resource {
          void *do_allocate(size_t bytes, size_t aligment) override;
          void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;
          bool do_is_equal(pmr::memory_resource const&other) const noexcept;
        }
        
        /////////////////////////
        //// 自定义类型里使用PMR ///
        /////////////////////////
        struct PmrCustomer {
          pmr:: string name_;
          
          using allocator_type = pmr::polymorphic_allocator<byte>; // type is irrelvant, will be rebound
          // init constructor
          explicit PmrCustromer(pmr::string n, allocator_type alloc = {}) :name {move(n), alloc} {}
          // copy/move。由于move要对比alloc是不是interchangable，有可能会退化成copy，所以不保证noexcept
          PmrCustromer(const PmrCustromer &c, allocator_type alloc) :name {c.name, alloc} {}
          PmrCustromer(PmrCustromer &&c, allocator_type alloc) :name {move(c.name), alloc} {} // not noexcept
        };
        
        // 类型转换：string、pmr::string都可以implicit转换至string_view
        string s{"Paul Kalkbrenner"};
        PmrCustomr c3{pmr::string{s}};  // implicitly converts s to string_view
        ```
    
    * 在main的最开头创建resource，以保证尽量晚地析构。另外不要用resource创建static对象
    
    * 
    
  * 其他

    * ```c++
      gcd();
      lcm();
      hpot(); // 计算二维、三维距离
      ```

    * 



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

* [别用rand](https://www.youtube.com/watch?v=LDPMpc-ENqY)。leetcode 384

  * rand 范围小(32k)，长度短(执行一段时间之后出现重复)；另外大家喜欢`rand()% Val`，但这个分布不正确
    * `rand()%10`的分布并不是均匀的，比如rand范围为0-12，会导致0~2的概率翻倍
    * 更一般的，鸽巢原理，从理论上就不可能得到预期的分布，除非是`Rand_Max`的倍数

  * `srand(time(nullptr))`中time的单位是秒，即1s内执行多次时，结果会相同
  * `mt19937` 运行很快（构造比较慢），可复现+ 平台间一致
  * `random_device`用于选个开始值，慢 + 无法复现+平台间不一致
  * `uniform_int_distribution`，可复现+平台间不一致
  * 不要用`random_shuffle`，使用`shuffle`
  * URNG/distribution function op是non-const的，多线程不安全

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




* [const T&&](https://www.codesynthesis.com/~boris/blog/2012/07/24/const-rvalue-references/): 由于`const T&`可以绑定lvalue和rvalue，在某些情况下需要区分二者。比如`template <class T> void ref (const T&&) = delete;`

  * 不建议返回的临时变量为const: RVO不受影响（但是编译时还是会检查）；另外RVO失效时，会导致不能move，只能copy
  * 另外函数不建议是const，比如函数链式调用时，内层函数const会导致外层可能被关掉move（原因同上）
* [copy elision](https://en.cppreference.com/w/cpp/language/copy_elision): c++17

  * c++17强制要求: return, initial 一个prvalue时，Mandatory

    * 基类时不执行





