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

    * ```c++
      atomic<T *> T::pInstance {nullptr};
      T * T::Instance() {
        T* p = pInstance; // 这里以及下面使用T*，减少一次atomic读。视频24:00左右
        if (nullptr == p) {
          lock_guard lg{mut_T};
          if (nullptr == (p = pIntance)) {
            pInstance = p = new T{};
          }
        }
        return p; // 这里的atomic读被优化掉了
      }
      ```
  
  * part 2。32:00左右：解决ABA问题（reader）；解决find（find的结果可能被pop删掉了）
  
    * ```c++
      template<typename T> class slist {
        struct Node{T t; shared_ptr<Node> next;}; // T* -> shared_ptr to solve ABA
        atomic<shared_patr<Node>> head;
      };
      ```
      
    * ABA问题：lock-free是计算后while(CAS)不断尝试。一般来说，这个值是地址。那么存在A地址被pop并释放后，新的对象复用了这个地址。那么问题的关键就是在CAS完成前，不要复用这个地址，即不要释放。
  
      * 办法有多个。part2 16:00
      * 其中有shared_ptr，另外由于是多线程，所以head得是atomic_shared_ptr
      * [stackoverflow](https://stackoverflow.com/questions/31277130/atomic-shared-ptr-for-lock-free-singly-linked-list) 上一篇讨论。
  
  * to improve scalability, we need to minimize the contention
  
    * reduce the size of critical sections
    * reduce sharing by isolating threads to use different parts of the data structure
      * moving cleanup from producer to consumer lets consumers touch only the head, producers touch only  the tail
    * reduce false sharing of different data on the same cache line, but adding alignment padding.
      * Separate variables that should be able to used concurrently by different threads should be far enough apart in memory
  
* [Walter E. Brown "Modern Template Metaprogramming: A Compendium, Part I"](https://www.youtube.com/watch?v=Am2is2QCvxY)

  * 


## 2016

* [A ＜chrono＞ Tutorial](https://www.youtube.com/watch?v=P32hvk8b13M&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=64)
  * 为啥要弄个库：
    * 10ms明确地表示10毫秒
    * 可以快速筛出代码中的时间，和其他用途的数字区分开。并快速切换单位，如s -> ms。f(seconds)
    * 灵活的时间转换比如 `frame = duration<int32_5, ratio<1,60>>`，输出时按ms进行cout
  * 什么时候需要time_point_cast和duration_cast: 隐式转换会丢信息时，比如1234ms->1s

* [tuple＜＞: What's New and How it Works](https://www.youtube.com/watch?v=JhgWFYfdIho&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=76)
  * 简化代码：字典序比较:`tie(a,b,c) < tie(other.a, other.b, other.c)`

* [Variants: Past, Present, and Future](https://www.youtube.com/watch?v=k3O4EKX4z1c&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=33)

  * ```c++
    template <typename LeafData>
    struct BinaryTree;
    
    template <typename LefeData>
    struct BinaryTreeBranch {
      shared_ptr<BinaryTree<LeafData>> left; // maybe unique_ptr better?
      shared_ptr<BinaryTree<LeafData>> left;
    };
    
    template <typename LeafData>
    struct BinaryTree {
      using Value = variant<LeafData, BinaryTreeBranch>;
      Value value;
    }
    ```

* [std::accumulate: Exploring an Algorithmic Empire](https://www.youtube.com/watch?v=B6twozNPUoA&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=37)

  * ```c++
    // 简化代码
    int sum;
    for (auto val : val_vec) {
      str += val;
    }
    // vs
    int sum = accumulate(val_vec.begin(), val_vec.end(), 0, [] (auto pre, auto s) {return pre + s;});
    // accumulate + associativity -> reduce
    int sum = reduce(val_vec.begin(), val_vec.end(), 0, [] (auto pre, auto s) {return pre + s;});
    ```

  * [monoids](https://www.cnblogs.com/richieyang/archive/2021/03/08/14323842.html) ——可以应用accumulate

    * Closure: 作用后依然是monoids
    * associativity: 结合律
    * Identity element: 有0元素
    * [Integer to Roman](https://leetcode.com/problems/integer-to-roman/) 。
      * `optional<To, From> func(From)`
    

* [Variadic expansion in examples](https://www.youtube.com/watch?v=Os5YLB5D2BU&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=47)

  * ```c++
    // 一个例子
    template <typename... Ts>
    auto double(variant<Ts...> v) {
      using visitor_t = void (*)(variant<Ts...>);
      static visitor_t handlers[] = {[](variant<Ts...> v) {
        using T = Ts;  // 获取当前的T
        cout << get<T>(v) + get<T>(v) << endl;
      }...};
      handlers[v.index()](v);
    }
    
    variant<int, float, string> d = "s";
    double(d);
    
    // 实现个variant的构造函数
    variant(const variant &other) : tag(other.tag) {
      using visitor_t = void (*)(variant & self, variant const &other);
      auto generator = [](auto type) {
        using Arg = typename decltype(type)::type;
        return [](cariant &self, const variant &other) {
          new (&self.storage) Arg(*reinterpret_cast<const Arg *>(&other.storage));
        };
      };
      static visitor_t copy_ctors[] = {generator(id<Ts>())...};
      copy_ctors[tag](*this, other);
    }
    
    ```

* [The Exception Situation](https://www.youtube.com/watch?v=Fno6suiXLPs&list=PLHTh1InhhwT7J5jl4vAhO1WvGHUUFgUQH&index=43)

  * 打日志是不可取的，程序出错了就不应该再以错误的状态跑下去
  * 可以考虑c风格的bool出参，或者进一步，c++17的optional、tuple。后者写起来很干净。不过会改掉原代码的api、会污染原本的code path
  * exception更干净：
    * 几乎不会对原代码有太多影响。api、code path等
    * 并且隔离开了disappointment detection和handling（detection处一般缺少如何正确handle的context）
    * 构造函数无返回值，只能走exception（或者两步init）
    * 除了极端情况（视频里就举了一个，且应该后来被修复了），exception都比其他方式性能好。当然exception比完全不处理还是会有些开销的，视频里说是3%
  * abseil
    * mutex提供了读写锁。debug模式下的dead lock
    
  



## 2017

* [“C++ as a "Live at Head" Language”](https://www.youtube.com/watch?v=tISy7EJQPzI&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=2)——SoftwareEngineeringAtGoogle有一章专门提了这个
  * Engineering is programming integrated over time（programming完成，engineerring维护）
  * engineering 和time强相关
    * vcs：可以回到某个历史
    * ci：最近的改动没有broken the system
    * ut：将来的代码别有问题
    * 重构：从旧到新
    * 设计模式：to plan for future changes without knowing exactly what it will be
    * 依赖管理：解决libraries的变化
  * 讲了下sematic version。想想都头疼。20:22
  * [hyrum law](https://www.hyrumslaw.com/): With a sufficient number of users of an API, it does not matter what you promise in the contract, all observable behaviors of your system will be depended on by somebody
  * 以下条目不要依赖std、或者其他代码（比如第三方库），应只限制在你自己的代码中。assume the "call only" interface
    * 不要forward declaration。可能他们版本升级后，和你的就不一致了。属于breaking change
    * 别依赖一个函数的地址。也是类似的。签名可能会变，比如加个overload的版本
    * 还有类似的就是meta programming/introspection on type properites。比如emplace_back的返回类型在c++17中就变了。
    * 不要依赖ADL。比如通过`absl::string_view`查找到`absel::StrCat`，然后`absl::string_view`在c++17版本下会是`std::string_view`的别名。这时候`absel::StrCat`就找不到了。
  
* [Herb Sutter “Meta: Thoughts on generative C++”](https://www.youtube.com/watch?v=4AfRAVcThyA&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=3)
  * 主要是如何让代码更简洁。以space operator切入，讲了一些想演进的语法
  
* [Matt Godbolt “What Has My Compiler Done for Me Lately? Unbolting the Compiler's Lid”](https://www.youtube.com/watch?v=bSkpMdDe4g4&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=5)
  * 一些汇编
  * compiler挺聪明的，O3太反直觉了
  * 乘法可能会改成shift；手写的shift可能会被替换成乘法：`a*65599=(a<<16) + (a<<6) - a`, 约24:00
  * 除法、取模也会做适当拆解成shift
  * 甚至会识别你的意图：
    * count二进制表示的1，直接替换成对应的指令
    * 求和1+2+..+n甚至会直接用`n*(n+1)/2`进行替换
  
* [Walter E. Brown “Programming with C++ Constraints: Background, Utility, and Gotchas"](https://www.youtube.com/watch?v=n4ie7tPMXOQ&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=7)
  * 以前：static_assert, enable_if, SFINAE
    * 比如求幂：先写个一般迭代，再写个终结的特化（可以用enable_if写的好看些）
  * c++17: constexpr if：不用再写SFINAE啦
  * c++20: Concept
  * 以swap为例——std要求T需要为movable type
    * movable=copyable || has-move-ops
  
* [Stephen Dewhurst “Modern C++ Interfaces..."](https://www.youtube.com/watch?v=PFdWqa68LmA&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=13)
  * 主要是怎么在模板编程里提供一个好的接口。通过trait去表达你的思想，你的各项要求、优化开关
  * Is_transparent: 可以参考[这个](https://www.fluentcpp.com/2017/06/09/search-set-another-type-key/)例子。简单来说，我们声明这个函数是transparent的，可以接受和容器KeyType不同的参数。此时会有另一组函数重载被开启。count`, `lower_bound`, `upper_bound` and `equal_range等
    * 后面的思想是，框架留好口子（SFINAE，`enable_if<T::SomeProperty>`）。我提供函数是，可以选择是否要使用这些口子，即定义`XXX::Someproperty`。——感觉就是type trait的一个形式
  * 37:00提举了个例子，不同c++标准下的实现。c++17的fold expression非常简洁
  * 24:18 当一个模板实例化后，其内部的模板函数可能发生非预期的匹配，因此要加上一些trait限制
  
    * ```c++
      template <typename T>
      class Heap {
        Heap(size t, T const&v);
        // range init. Heap<int>(5, 2)会命中这个构造函数, ooops
        // 因此加上using xxx = typename iterator_traits<It> ...
        // template<typename In, typename = IsIn<In>>
        template<typename In>
        Heap(In b, In e);
      };
      
      template<typename T>
      class X {
        void operaotr(T const&);
        void operaotr(T &&);
        // operator(T&)会命中这个
        // template <typename S, typename = NotSimilar<S,T>>
        template <typename S>	
        void operator(S&&);
      };
      ```
  
    * 
  
* [Ben Deane & Jason Turner “constexpr ALL the Things!”](https://www.youtube.com/watch?v=PJwd4JLYJJY&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=15)
  * [constexpr_all_the_things](http://github.com/lefticus/constexpr_all_the_things.git)。函数式编程+constexpr json
  
  * ```c++
    template <typename P, typename T, typename F>
    constexpr pair<T, parse_input_t> accumulate_parse(parse_input_t s, P &&p, T init, F &&f) 
    // Parser: optional<pair<To, From_remain>> parser(From);
    // Func: To func(To, From_remain)
    ```
  
  * [Don't constexpr All the Things - David Sankel [CppNow 2021]](https://www.youtube.com/watch?v=NNU6cbG96M4) 
  
    * constexpr的依赖是传递性的，需要所有调用链上的都满足。这会极大限制constexpr的使用。
    * Core constant expressions(?) are too difficult to understand
    * One function implemented in two languages is error prone (constexpr and non-constepxr)
  
* [Carl Cook “When a Microsecond Is an Eternity: High Performance Trading Systems in C++”](https://www.youtube.com/watch?v=NH1Tta7purM&list=PLHTh1InhhwT6bwIpRk0ZbCA0N2p1taxd6&index=20)——一些性能tips

  * 配置化不一定需要特别多的虚函数，以多场景配置化举例：

    * ```c++
      // 3个虚函数表(XX_Base) vs 1个需函数表(ManagerBase)
      struct Manager{
        RecallBase *recall_;
        RankingBase *ranking_;
        RerankBase *rerank_;
        void DoRecommend();
      };
      
      template<typename Recall, typename Ranking, typename Rerank>
      struct Manager: public ManagerBase{
        Recall recall_;
        Ranking ranking_;
        Rerank rerank_;
        void DoRecommend() final;
      };
      ```

    * Don't be afaid to use exceptions(They are zero cost if they don't throw)



## 2022

* [C++ in Constrained Environments](https://www.youtube.com/watch?v=2BuJjaGuInI&list=PLHTh1InhhwT6c2JNtUiJkaH8YRqzhU7Ag&index=145)
  * language is just a part of developer's toolbox
  * All problems in computer science can be solved by another level of indirection, except for the problem of too many layers of indirection—— [Butler Lampson](https://en.wikipedia.org/wiki/Butler_Lampson)。https://en.wikipedia.org/wiki/Indirection
  * variant 比union好：express its intent
  * error code vs exception
    * error code
      * a failure is normal and expected
      * an immediate caller can reasonably be expected to handle the failure
      * an error happens in one of a set of parallel tasks and we need to know which task failed
* [Can C++ be 10x Simpler & Safer? ](https://www.youtube.com/watch?v=ELeZAKCN4tY)——cppfront