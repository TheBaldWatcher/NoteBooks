[[toc]]

# Chapt1

## function templates

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



# Chapt 2

## class template

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
  
  // 在访问一个基于template parameter的base class时，使用using this-> or Base<T>::
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
    C(C const volatile&) = delete; // 通过这个阻止compiler自动生成C(C const&)构造函数
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

  * 

