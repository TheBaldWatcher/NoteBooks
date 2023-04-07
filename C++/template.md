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

  

## Chapt 3 Nontype template paramter

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
  ```

* 