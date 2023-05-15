

[toc]





[CppCon 2018: Kris Jusiak “State Machines Battlefield - Naive vs STL vs Boost”](https://www.youtube.com/watch?v=yZVby-PuXM0&list=PLHTh1InhhwT6V9RVdFRoCG_Pm5udDxG1c)

* 用variant实现state machine。不满足开放闭合原则，但其他方式似乎也有这个问题

* ```c++
  class Connection {
    struct Disconnected {}; // use struct instead of enum, can have data
    struct Connecting {};
    struct Connected {};
    
    variant<Disconnexted, Connecting, Connected> state = Disconnected{};
    constexpr void process_event(Connect const &) {
      visit(overload{
        	[&](Disconnected) { establish(); state = Connecting{}; },
          [](auto) { } // no changes
  	    }, state);
    }
  };
  
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

* 
