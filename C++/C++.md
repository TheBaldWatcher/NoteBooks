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
    * 多用string_view
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
* strict aliasing：https://stackoverflow.com/tags/strict-aliasing/info


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

