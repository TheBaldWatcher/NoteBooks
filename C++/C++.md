

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
* function: function目前不能偏特化。[另外特化不参与overload](http://www.gotw.ca/publications/mill17.htm)(不然可能导致template base的变动)。
  * Moral #1: 如果要写一个特化版本且必定重载成功，写成non-template
  * Moral #2: 如果可以，从根源上规避Moral#1, 写一个handleoff function, 内部调用class-template来进行重载

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

* **Rvalue Reference, Move Sematics, and Perfect Forwarding**
  
  * move vs forward:
    * move无条件cast，forward有条件cast。
    
    * 别对`const`使用`move`/`forward`，一方面意思相悖；另一方面实际上走的是l-ref的接口，而非预期的r-ref。
    
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
    
      * 但也带来个问题：g-ref版本没有`const`限定了。这个时候forward就很重要了，不然，使用move也会影响l-ref的版本，导致参数非预期的被move。
    
  * 只有格式为`T&&`且存在类型推导时，才是forward reference。`void func(vector<T>&& v)`和模板已经具现化后的`push_back(T&&)`都只是r-ref
  
  * move的时候，使用`move_if_noexcept`，以保证异常安全。
  
  * `return move(param_xx)`：对于RVO不起效时使用。（RVO起效时，没有收益：RVO会优先in-place，不行再move）
  
  * 尽量不要重载forward_reference：forward版本总能产生最好的match，而如果non-forward需要implicit转换，那么会优先级偏低
  
    * ```c++
      struct Person {
          template<typename T>
          explicit Person(T&&);			// func_1
          
          explicit Person(int n);			// func_2
          Person(const Person&);			// func_3, generate by compiler
      };
      short a = 1;
      Person(a);		// func_1
      
      Person p;
      Person p1(p);	// func_1
      Person const cp;
      Person p2(cp);	// func_3
      
      struct SpecialPerson : Person {
        SpecialPerson(SpecialPerson &&rhs) : Person(move(sp)){};	// func_1
        SpecialPerson(SpecialPerson const &rhs): Person(sp){};	// ※ func_1
      };
      ```
  
  * 如果需要重载forward ref：
  
    * 考虑tag dispatch：一个普通函数接口，内部调用impl
    * 对于构造函数，由于有编译器产生的函数，可以考虑trait：`!is_base_of<Person, decay_t<U>> & !is_integral<T>`
    
  * move的效率：move是针对堆上资源来说的，因此没有堆资源就没有move的意义。
  
    * 考虑array，ele可能是move的，但array本身在栈上
    * 首先，要是rvalue；其次，要有move operator（可能被rule of five给depress掉）；另外，考虑`move_if_noexcept`，因此要声明`noexcept`。
    
  * 不能forward的情况：
  
    * initializer_list: 形参为非initializer_list，但是实参是的时候。（此时属于[non-deuced context](https://en.cppreference.com/w/cpp/language/template_argument_deduction#Non-deduced_contexts)）
    * constexpr：由于常量替换，无法取地址，即无法获得引用
    * overload/template function
    
  * 什么时候用emplace：emplace是为了避免temp对象的构造——主要用于vector、deque、string
  
    * 是direct init而非copy init——注意，这个和explicit的影响：`regex r1 = nullptr, r2(nullptr)`。
    * 参数类型和元素类型不一致（这时候多半是要构造临时对象的）
    * 不太会拒绝重复元素（几率小，或者不拒绝）——如果要拒绝，一般还是得构造一个temp来进行比较
    * 资源管理——emplace导致temp不会被构造，但有可能需要用temp来保证异常安全等情况的资源释放。
  
* **Type Deduction**

  * `auto` and `template` type deduction is the same, except for `initializer_list`

  * for function **return type**, or lambda **auto param**,  **template** type deduction is used, instead of auto's.

    * ```c++
  tempalte<typename T>
      void f(ParamType param);
      f(expr);
      int x = 27;
      const int cx = x;
      const int &rcx = x;
      const int *pcx = &x;
      const char name[] = "literal string";
      const char *const pc_name = name;
      void some_func(int, double);
      // case 1,ParamType is l-ref(XX&)/ptr:
      // ignore expr ref, `ParamType` ~~ `expr`
      void f(T& param);
      f(x);	// T ~ int, ParamType ~ int&
      f(cx);	// T ~ const int, ParamType ~ const int&
      f(rcx);	// T ~ const int, ParamType ~ const int&
      f(name);// ※ T ~ const char [15], ParamType ~ const char (&)[15]
      f(some_func);	// ※ ParamType ~ void(&)(int, double)
      
      void f(const T& param);
      f(x);	// T ~ int, ParamType ~ **const** int&
      
      void f(T* param);
      f(&x);	// T ~ int, ParamType ~ int*
      f(pcx);	// T ~ const int, ParamType ~ const int*
      f(some_func);	// ※ ParamType ~ void(*)(int, double)
      
      // case 2, ParamType is Universal Ref
      // for expr is lvalue: T and ParamType is l-ref
      // for expr is r-ref: like case 1
      void(T&& param);
      f(x);	// T ~ int&, ParamType ~ int&
      f(cx);	// T ~ const int&, ParamType ~ const int&
      f(rcx);	// T ~ int&, ParamType ~ const int&
      f(27);	// T ~ int, ParamType ~ int&&
      
      // case 3, ParamType is Neither ptr nor ref
      // ignore ref, and ignore cv(for pass by value, it's meaningless)
      voif f(T param);
      f(x);	// T ~ int, ParamType ~ int
      f(cx);	// T ~ int, ParamType ~ int
      f(rcx);	// T ~ int, ParamType ~ int
      f(pc_name);	// ※ T ~ const char*, ParamType ~ const char*
      ```
    
    * 
    
  * decltype(auto): for function return, it use template deduction, which will descard `ref`. What if we want preserve it?
  
    * ```c++
      template<typename Container, typename Index>
      decltype(auto)	// preserve ref
      authAndAccess(Container &&c, Index i) {
          return forawrd<Contianer>(c)[i];	// handle l/r-ref
      }
      ```
  
* **lamda**

  * 使用init capture：`[a=a]`或者`[&a=a]`(`[&a]`也可以，不过这样和值捕获格式不一致了)，而不要使用`[=]`、`[a]`、`[&]`或`[]`，一方面将需要capture的写出来；另一方面，static这种全局变量用`[a]`这种其实捕获的是ref。
  * c++14后不要再使用bind：敲更多的字，**构造**bind时copy存储对象瞬时状态
  
* **concurrency**

  * 尽量用feature（asynch）而非thread：避免自己处理thread exhaust/oversubmission/load balance。除非要接触一些底层api或实现thread pool
  
  * 尽量避免使用默认的`asynch::lauch_policy`，除非：不要求异步（比如get时可以阻塞）、不要求特定线程（thread local）、允许永远不执行、等待时间考虑了不执行的情况
  
    * ```c++
      if (f.waite_for(0s) == feature_status::defered) {
          // ...
      } else {
          while (f.wait_for(100ms) != feature_satus::ready) {
              //...
          }
      }
      ```
  
  * 保证所有path都是unjoinable的：析构一个unjoinable会导致terminate。
  
    * RAII并不能很好的解决：无论destructor是`join()`还是`detach()`都不太好，前者阻塞；后者后台运行，且如果有引用的变量，可能进行意外的修改。
    * 最好是手动实现interrupt。
    
  * feature析构时，如果是`asynch()`且non-defer（defer/default），则阻塞。其他的则不阻塞（比如`packaged_task`已经把join还是detach写在代码中了，不然析构joinable会terminate）
  
  * 单次通信用`packaged_task + shared_future`：比如detecting thread检测到event，reacting thread开始处理
  
    * 备选方案condition variable：使用mutex有点没必要，而且要保证thread顺序，保证notify时，reacting thread 已经运行起来了
    * 备选方案flag：`atomic<bool>`，busy wait
  
* tweak

  * 什么时候pass by value，而l&r-ref重载或者forward ref：
    * copyable(~~unique_ptr~~)、cheap to move（类本身，以及function chain）、always copy constructed（assignment在lvalue时比较复杂：资源是否能放入旧资源中？）、且没有继承问题
    * by value优势：比l&r-ref重载少写一个函数、而forward ref是模板，即代码要放到.h中，且有些不能forward、error msg也不好看。但by value在construct时会多一个move的代价。

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
  
* smart_ptr

  * ```c++
    class shared_ptr {
        // constructor: from raw, nullptr_t, unique, shared
        // first three means create control block
        template <typename _Yp>
        shared_ptr(_Yp* __p)	// specilization nullptr_t
            : __shared_ptr<_Tp>(__p) {}
    };
    ```

  * shared_ptr:

    * 作为一个proxy，内含了一个control block和ptr。这中结构也为多线程的堆上对象的析构提供了便利，参见《Linux 多线程服务端编程》
    * 捕获析构动作：可跨模块边界传播、二进制兼容（Foo大小改变，旧客户端可使用新动态库，只要Foo.h中无访问成员的inline函数）
    * stored ptr(sub obj) + manage ptr(main obj). 
    
  * case scenario

    * unique：exclusive ownership
    * shared：shared ownership
    * weak：作为shared补充，用于不影响obj生命周期的情况，比如observer模式。ower持有child的shared_ptr，child持有ower的weak_ptr

  * shared_ptr:从raw、nullptr、unique创建时会创建control block。如果有`emplace_back(this)`可能需要继承自`enbale_shared_from_this`，不然control block没有管理全部的this

  * 尽量使用`make_xxxx` 而非`shared_ptr(new T())`: 

    * pro:
      * engineering: less repeated type
      * exception safe:`func(shared_ptr(new T()), g())`
      * efficiency: only memory allocation once. `shared_ptr(new T())`, use twice. (one for T, one for control block)
    * con:
      * 自定义deleter、传递initialize_list 无法forward(work around: `auto tmp = {x,x}; auto p = make_shared<T>(tmp)`)
      
      * 如果有定制的new和delete，一般来说都缺少考虑到要给control block分配内存
      
      * 大内存T: control block要等到`weak_cont == 0`才释放，如果T和control block是同一块内存，这会导致T的内存其实是延时释放的
      
      * ```c++
        // exception safe version of using shared_ptr(new T())
        shared_ptr<T>tmp(new T, deleter);	// avoid leak when g throws
        process(move(tmp), g());	// move: avoiding atomic ref-count's increasing
        ```
      
      * 

  

* [eof是在读到末尾时设置](https://stackoverflow.com/questions/8763720/istreampeek-curious-behavior-wrt-eof?r=SearchResults)，所以类似`get()`这种只读当前位置的，即便读完之后就是末尾了，也不会设置`eof`。另外，`unget()`可能需要先`clear()`。

* `func({,,,}) vs func(,,,)`: `initializer_list`重载判定较高，会屏蔽掉其他接口。bad example: std::vector. 考虑这两种接口二选一。

* const的成员函数不一定是thread safe的，因为它不一定是只读的：`mutable int a;`。

* why use:

  * `nullptr vs 0`: Type Safe
  * `using vs typdef`: 更好支持模板(dependent type need a `typename`)，可读性
  * `override`: 要人工保证函数签名一致太难了：函数名、参数类型、const/ref this
  * smart ptr: raw ptr无法区分array，是否持有资源，定制dctor，不多不少析构一次，悬垂指针
  
* Rule of five: 

  * ```
     copy_ctor     ---   copy_assign
         |	\			/	|
         |		dctor   	|
       	|	/			\	|
     move_cotr			move_assign	
     最好都声明default。否则，后期加一个dctor会抑制默认move oprator，导致move操作变为copy操作
    另外，template不在规则中，即不会产生抑制效果。
    ```

     
    
  
* operator_arithmetic

  * bitwise shift: `a<<b`，`a>>b`。
    * a为负数：
      * until c++20: `<<`是<font color=red>UB</font>，`>>`是<font color=blue>IB</font>，不过一般都是arithmetic
      * since c++20: `<<`超出部位直接丢弃，`>>`为arithmetic，即保留符号
    * b为负数、或超过Type的bit数：<font color=red>UB</font>。
  * 除法为向0截断。













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
      
        









hi：

1、想问问薪资能否再上浮1k呢，因为薪资是范围内最小值，但简历应该不算踩着最低门槛~

2、另外想问问岗位的职级 ， d6

3、以及什么级别会有期权，大概是怎么分配的呢，D8

4、内部竞争机制和薪资涨幅方便说下吗~  35  910月，10-20%

5、另外，还有个腾讯的面试在流程中，想问问offer的答复能往后延迟一下吗？





1、想问问什么时候会有今天的面试结果、如果有后续面试，总体的面试流程会比较长吗？（听说可能会有个把月）

2、工作地点、和工作强度能说下吗？











主要做什么，岗位对我的定位和期望

近期、中期目标，pcg最近都在招人