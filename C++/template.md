[TOC]





# 随想

* template 为了限定类型，所以有trait、enbale_if等机制
* 除了类型，还得考虑ref，比如`T a;`如果T是ref就有问题；或者还得考虑const？

# Part I: Basics

## Chapt1: function templates

* ```c++
  template <typename T1, typename T2>
  auto max(T1 a, T2 b); // c++17
  auto max(T1 a, T2 b) -> typename decay_t<decltype(true?a:b)>; // c++11, 这里不需要填b<a?, decay为了应对引用
  auto max(T1 a, T2 b) -> common_type_t<T1, T2>; // c++11
    
  // default type-parameter可以不是最末尾
  template<typename RT = long, typename T>
  RT func(T );
  // 别忘了，要在函数调用前，让所有的函数重载、特化可见
  ```

* 对于模板，尽量pass-by-value。C1.6.1

  * 语法更简单、compiler optimizze better、move让开销没那么明显
  * 对于模板，会影响到简单类型的instantiation；调用方可以显式通过ref传引用

## Chapt 2 class template

* ```c++
  // 别忘了，模板类里，该类参数可省略模板参数。建议省略，避免手写写错。
  // 另外，构造和析构这种只用到名字，而不是type的，只能写Stack
  // 模板类declare、define不能在function或block scope，只能在global/namespace中
  template<typename T>
  class Stack {
    Stack(Stack const&);  // 等价于Stack(Stack<T> const&)
  }
  
  // friend。简单来说，优先inline，不行就introvert
  // 两个办法：https://stackoverflow.com/questions/4660123/overloading-friend-operator-for-template-class
  //  * inline，这样实例话的是个non-template friend
  //  * extrovert
  template <typename T>
  class Stack {
    // inline
    friend ostream& operator<<(ostream& os, Stack const&s)  {}
    // extrovert. bad: all instantiations of this template are my friends
    template <typename U>
    friend std::ostream& operator<<( std::ostream&, const Test<U>& );
    // introvert: 需要forward两个声明，且要跟个<T>特化。写起来比较繁琐
    // template <typename T> class Test;
    // template <typename T> std::ostream& operator<<( std::ostream&, const Test<T>& );
    friend std::ostream& operator<< <T>( std::ostream&, const Test<T>& );
  };
  ```

* |                   | function template | class template |
  | ----------------- | ----------------- | -------------- |
  | partial specilize | not               | ok             |
  | partial deduce    | ok                | not            |

  

## Chapt 3-5 Nontype template paramter/Variadic/Tricky

* ```c++
  // 可以是auto
  template<auto val>
  void func(){}
  template<decltype(auto) val>  // int&
  void func(){}
  
  // 类型除了整形外，还可以是ptr/ref
  // 对于ptr/ref，不能是string literal，temporaries，data member，subobjects
  extern char const s03[] = "hi"; // external linkage
  char const s11[] = "hi";  // internal linkage
  int main() {
    static char const s17[] = "hi";  // nolinkage
  }
  
  // variadic expression
  template <typename T>
  void printDoubled(T const& ...args) {
    print (args + args...); // 有点类似fold expression。展开后是print(a1+a1, a2+a2, ...)
  }
  
  // 在访问一个基于template parameter的base class时，使用using this-> or Base<T>::或者使用using
  // 参考13.4
  template <typename T>
  class Base {
    public: 
    	void bar() {cout<<"base\n";}
  };
  template <typename T>
  class Derived: Base<T> { // 这里默认private基础，则Base的public接口只在Derived中可见
    public: 
    	void foo() { this->bar();} // 或者使用Base<T>
  };
  
  // .template
  template<int N>
  void print(bitset<N> const &bs) {
    // bs.to_string<char...> 这里区分不出来第一个<是模板的意思，所以要加.template
    // 类似的还有function的f.template target<FuncType*>();
    cout<<bs.template to_string<char, char_traits<char>, allocator<char>>();
  }
  
  // template template parameter
  template<typename T,
   					template<typename> class Cont = deque> // Elem ommited
  class Stack{};
  Stack<int, vector> v_stack; // vector<int> -> vector
  ```

## Chapter 6-8 enable_if、Pass by Value/Ref

* `forward`也会传递const

* ```c++
  // 构造函数的匹配: non-const Person会匹配到模板
  struct Person{
    Person(Person const&p) {}
    Person(Person&& p){}
    
    template<typename Str, typename enable_if_t<is_covertible_v<Str, string>>>
    explicit Person(Str&& n){}
    string name;
  };
  
  // 如何emplate构造函数——clever的代码，别用，这里只是做下记录，以免后续碰到
  template <typename T>
  struct C {
    // 通过这个阻止compiler自动生成C(C const&)构造函数。
    // 顺带说下，C()这个默认构造函数在有模板时是不会自动生成的。12.3末尾的-脚注7也提到了这点
    C(C const volatile&) = delete;
    template<typename U, typename = enable_if_t<!is_integral+v<U>>>
    C(C<U> const&) {}
  };
  ```

* value / ref

* ```c++
  // const的右值会匹配T&
  template<typename T>
  void outR(T&) {}
  
  outR(string("hi"));	 // error
  outR("hi");  // ok, T deduced as char const[3]
  outR(ReturnString());  // error
  outR(ReturnConstString());  // ok, T deduced as string const
  outR(move(a_str));  // error
  outR(move(a_const_str));  // ok, T deduced as string const
  ```

  * 先by value（把by ref视为一个优化）。能方便地处理decay的情况。如果有性能影响，可以考虑借助std::ref

  * 可能要by ref的情况

    * in/out parameter。可能要disable掉const ref

    * 需要forward给下层调用。可能需要std::decay或者std::common_type来处理decay

    * ```c++
      template<typename T1, typename T2>
      auto makr_pair(T1&&a, T2&&b)->constexpr pair<typename decay_t<T1>, typename decay_t<T2>>{}
      ```

    * 性能至关重要，且不会需要一个local copy

* SFINAE

  * ```c++
    // 只支持有t.size()的类型T。void是为了避免有用户重载了,操作符
    template<typename T>
    auto len(Tconst &t) -> decltype((void)(t.size()), T::size_type){}
    ```

## Chapter 11 GenericLib

* ```c++
  //// invoke用处
  // case1: 提供了统一的形式：mem-func不能用func(...)来调用，得是obj.func(...)
  // case2: 在wrapper中使用
  template<typename Callable, typename ...Args>
  decltype(auto) call(Callbale &&op, Args&&... args) {
    // something
    decltype(auto) ret {invoke(forward<Callable>(op), forward<Args>(args)...)};
    // something
  }
  
  //// 使用auto，而非decltype(auto)，以避免deduce成引用
  // int size = 10; Arr<int, size>x;  size+=100; x.print();
  // oops,可能按10个元素初始化但按110个元素遍历
  template<typename T, auto SZ> // 如果代码依赖了SZ（比如说for循环），这里又是个引用，可能会有问题
  class Arr{};
  
  //// defer evaluation
  template<typename T>
  class Container {
    // oops, T is incompleted, 但is_move_constructible_v需要它是completed
    auto foo () -> typename conditional_v<is_move_constructible_v<T>, T&&, T&>;
    // defer到使用时
    template<typename D = T>
    auto foo () -> typename conditional_v<is_move_constructible_v<D>, T&&, T&>;
  }
  ```

* 一些工具、tips

  * invoke;
  * adressof(); // 以免T重载了&
  * decay; // 避免return一个local ref
  * In/out T要考虑T是const的情况

# Part II: Templates in Depth

## Chapter12：Fundation in Depth

* Virtual member function: 
  * 受制于compiler/linker实现，template member function不能是virtual。因为vtable需要知道全局的虚函数个数（fixed-array），对于虚函数，这得等到代码全都translate。
  * 注意，这和template class的virtual function是不一样的（上面说的是virtual function是template的）。
* fold expression
  * 对于可能有empty pack的、且涉及`||`、`&&`、`,`的情况，建议避免用unary op的形式，而是用binary op
  * empty `&&`: true
  * empty `||` : false
  * Empty `,`: void expression
* friends
  * 什么时候可以是definition：只有当函数名不带`::`限定符、且不和当前可见的non-template function冲突（即是first declaration）时，才能是definition。
  * 只能定义primary template为friend。特化会被自动视为friend

## Chapter 13: Name

* ADL
  * unqualified、ordinary lookup failed、function name不是被括号包围
* Inheritance & Base Classes
  * 先查找nondepent，查不到再查depent。所以base里如果有T的话会把模板参数的T给屏蔽掉
  * 访问基类成员使用this或者Base::xx或者using

## Chapter 14: Instantiation

* 考虑到嵌套调用，函数的point of instantiation是在后方的最近点、variable的poi也是后方。class则符合直觉，在前方（也必须是在前方，因为函数可能用到了类信息，得保证在使用时可见）

* two-phase instantiation中，第二步是ADL，所以有可能会报找不到

  * ```c++
    // 函数的POI
    template <typename T>
    void f1(T x) {
      g1(x);  // #1
    }
    void g1(int) {}
    int main() {
      f1(7);  // ERROR: g1 not found!
    }
    // #2 POI for f1<int>(int)
    
    // 传递性的POI
    template <typename T>
    struct S {  using I = int; };
    // #1 POI: S<char>
    template <typename T>
    void f() {
      S<char>::I var1       = 41;
      typename S<T>::I var2 = 42;
    }
    int main() { f<double>(); }
    // #2 : #2a for S<T>, #2b for f<double>
    
    ```

* 编译优化：以string为例

  * 在头文件中通过extern抑制自动instantiation，并手动执行instantiation definition

  * ```c++
    // ===== t.hpp:
    template<typename T> void f() { }
    
    extern template void f<int>();
    extern template void f<float>(); // declared but not defined
    // ===== t.cpp:
    template void f<int>(); // definition template void f<float>(); // definition
    ```



## Chater15: Deduction

* 一些case

```c++
// case1
template<typename T>
void f(T, T);
void (*pf)(char, char) = &f; // pf指向f<char>

// case 2
template< typename T, typename... Rest> void h1(pair<T, Rest> const &...);
template< typename... Ts, typename... Rest> void h1(pair<Ts, Rest> const &...);
void f(pair<int, float> pif, pair<int, double> pid, pair<double, double> pdd) {
  h1(pif, pid); // OK: T-int, 					Rest-{float, double}
  h2(pif, pid); // OK: T-{int, int}, 		Rest-{float, double}
  h1(pif, pdd); // Error
  h2(pif, pdd); // OK: T-{int, double}, Rest-{float, double}
}
```

* SFINAE和immediate context——好像是不具有传递性？

  * SFINAE只在immediate context中生效，其他时候不生效

  * 对于function template substitution，以下**不是**immediate context，即SFINAE不生效

    * 其他class/function template的定义，variable template的初始化

    * 默认参数、默认成员函数初始化

    * exception specification，即`noexcept(nonexistent(T()))`

    * ```c++
      // case 1
      template<typename T>
      struct Array {using iterator = T*;};
      
      template<typename T> // invalid, 但是instantiate Array<int&>, 失败. 此时SFINAE是不生效的
      void f(Array<T>::iterator first, Array<T>::iterator last) {};
      
      template <typename T>
      void f(T, T){};  // valid
      
      int main() {
        int a = 1;
        f<int&>(a, a);
      }
      // case 2
      template<typename T> auto f(T p) { return p->m; } // invalid
      int f(...) {}; // valid
      template<typename T> auto g(T p) -> decltype(f(p));
      int main() { g(42); }
      ```

  * explicit function specification只能从左到右进行指定。

    * 所以不能deduce、需要手动指定的模板参数要放在开头

    * ```c++
      template<typename Out, typename In>
      Out convert(In p) { ... }
      
      auto x = convert<double>(42); // 指定的是Out，In是deduce
      ```

  * auto

    * ```c++
      // 成员类型的简写
      template<typename> struct PMClassT;
      template<typename C, typename MemberT> struct PMClassT<MemberT C::*> { using Type = C; };
      template<typename PM> using PMClass = typename PMClassT<PM>::Type; // 这里specification只有一个参数
      template<auto PMDataT> struct CounterHandle { PMClass<decltype(PMD)> &c;};
      struct S {int i;};
      S s{42};
      CounterHandle<&S::i> h(s);
      // 如果不能写auto，会变成形如CounterHandler<int S::*, &S::i>
      ```

  * 一些其他杂项

    * ```c++
      // recursive + auto
      auto f(int n) {
        if (n > 1) return n*f(n-1); // Error, type of f(n-1) unknown
        else return 1;
      }
      auto f(int n) {
        if (n <= 1) return 1;
        else return n*f(n-1); // Ok
      }
      
      // decltype(auto) + SFINAE, 会导致immediate instantiation, 此时要考虑immediate context
      // 简言之：declauto(auto) 不要和SFINAE拥在一起
      template<typename T, typename U> auto add(T t, U u) -> decltype(auto) { return t + u; }
      ```

  * deduction guide

    * ```c++
      // 可以和aggregate initialization结合
      template<typename T> struct A{ T val; };
      template<typename T> A(T) -> A<T>; // 不需要写出来，这个会implicit生成
      A<int> a1{42}; // OK
      // 另一个例子
      template<typename... Ts> struct Tuple{
        Tuple(Ts...);
        // template<typename... Ts> Tuple(Ts...) -> Tuple<Ts...>;
        Tuple(Tuple<Ts...> const&);
        // template<typename... Ts> Tuple(Tuple<Ts...> const &) -> Tuple<Ts...>
      }
      
      // 对于构造函数：implicit deduction guide
      template<typename T> A(typename T) -> A<T>;
      // case 1: implicit deduction guide——关闭。话说不是特别理解为啥要关闭
      template<typename T> struct ValueArg { using Type = T; };
      template<typename T> struct A{
        using ArgType = typename ValueArg<T>::Type;
        A(ArgType b) : a(b) {}
      };
      // 变成了template<typename> A(typename ValueArg<T>::type) -> S<T>;
      // A x(12); 现在编译会报错了
      
      // case 2:对于右值T&&是关闭的，以避免非预期的匹配
      template<typename T> struct Y {
        Y(T const &);  // template<typename T> Y(T const&) -> Y<T>; #1
        Y(T&&);  //  template<typename T> Y(T&&) -> Y<T>; #2
      };
      
      string s;
      Y y = s; // #2是better match。为了避免这种情况，T&&不会有implicit deduction guide
      ```

* specification

  * specification最好直接跟在primary template后。尽量减少specify第三方库，除非对方明确指出了可以这么用（fmtlib）

  * ```c++
    // pg_0.h
    template <auto N>
    struct MySize { enum { len = 100 }; };
    // pg_1.h, pg_1.cc
    template <>
    struct MySize<1> { enum { len = 10 }; };
    void f1() { cout << "ff" << MySize<1>::len << endl; }
    // pg_2.h, pg_2.cc
    template <>
    struct MySize<1> { enum { len = 100 }; };
    void g2() { cout << "gg" << MySize<1>::len << endl; }
    // pg.cc
    int main() {
      f1(); g2();  // ff10  gg100——invalid的代码，但是在不同编译单元里不会报错
      return 0;
    }
    ```

  * 
