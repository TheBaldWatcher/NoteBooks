# 概念

* 内部可变性`Cell`,`RefCell`：相当于c++的`mutable`，智能指针、mutex需要这个规避编译检测；但data race的检查还是会有，只是在运行期



#Option:

* 可以用is_some来判断非None。另外链表遍历时，`Option<Box<T>>`会由于move导致问题，解决办法是加一个as_ref。参考LeetCode：rotate list

* Some中获取数据

  ~~~rust
  // map
  let some_string = Some("some_string".to_string());
  let some_len = some_string.map(|e| e.len());	// Some(11)
  // and_then
  fn sq(x:u32) -> Option<u32> { Some(x * x) };
  assert_eq!(Some(2).and_then(sq).and_then(sq), Some(16));
  ~~~

  



# String & Vec & Slice

* vec转String:`res.into_iter().map(|e| e.to_string()).collect::<String>()`

* iota：`(1..n as i32).collect::<Vec<i32>>()`

* 內积：`(1..n as i32).product::<i32>();`

* str: 

  ```rust
  s.chars().nth(n);	// O(n)
  s.as_ptr();	// *const u8
  
  // 使用u8来进行字符串遍历会方便一点？https://leetcode.com/problems/scramble-string/submissions/
  ```

  



# 技巧：

* 计算字符流，最后一个单词长度：`s.chars().rev().skip_while(|&e| e == ' ').take_while(|&e| e != ' ').count() as i32`
* `*s`和`s.deref`不等价，前者是`T`，后者是`&T`。
* [killing unwrap](https://dmerej.info/blog/post/killing-unwrap/)
  * 从`Opt`/`Result`返回`Opt`/`Result`:
    * 直接用`?`。如果`Result`的MyError要从?的BarError做一些转换，加一个`impl From<BarError> for MyError `。
    * 或者用`Result.map_err`或`Opt.ok_or_else`
  * `Reuslt`->`Opt`:
    * `return Some(bar::return_res().ok()?);`
  * `expect`
* 不要用`println`进行debug，用`dbg!`。
* 可空指针`Option<Rc<RefCell<T>>>`：可空`Option<T>`-><font color=red>内部可变性</font>`Option<Rc<T>>`->然后怎么到`Option<Rc<RefCell<T>>>`不太清楚
* `Rust`的safe与unsafe以内存安全进行区分，由于循环引用造成的内存泄漏不可避免，内存安全语义接受析构函数可不被调用，这使得：
  * `forget`这个不再标记为unsafe
  * 全局变量的析构函数会被漏掉，所以之前带有析构函数的类型不可作为static、const变量；现在可以了
  * `std`多线程模块`scoped`保证子线程在父线程前退出：已移出`std`，只作为三方库，由库实现者保证析构函数的调用



# 项目

* game

  * [Packaging a game for Windows, Mac, and Linux with Rust.](https://agmprojects.com/blog/packaging-a-game-for-windows-mac-and-linux-with-rust)

  * <font color=red> rust写的游戏</font>:https://itch.io/games/made-with-rust

  * <font color=red>roguelik 教程</font>[git](https://aimlesslygoingforward.com/blog/2019/02/09/writing-a-rust-roguelike-for-the-desktop-and-the-web/)

  * <font color=red>使用rust_sdl2编写的通用重力模拟器</font>[git](https://github.com/bcamp1/Gravisim)，[reddit](https://www.reddit.com/r/rust/comments/atdkeg/ive_been_making_a_gravity_simulator_using/)

  * <font color=red>波函数坍缩算法</font>[波函数坍缩（Wave Collapse Function）算法。该算法用于生成游戏地图，比如开放世界的无限地图，永远没有尽头](https://gridbugs.org/wave-function-collapse/)

* learning

  * <font color=red>通过实现Punchtop来学习Rust的思考</font>[3700行，适合新人阅读](https://hyperbo.la/w/reflections-on-learning-rust/)

* embed

  * <font color=red>「视频」从零开始使用Rust构建嵌入式传感器节点平台</font>：[地址](https://www.youtube.com/watch?v=S0VI70nY6Vo)

* blockchain&bitcoin

  * <font color=red>[系列视频」如何用Rust构建属于自己的加密货币 Part 1]</font>[系列视频](https://www.youtube.com/watch?v=vJdT05zl6jk&list=PLwnSaD6BDfXL0RiKT_5nOIdxTxZWpPtAv&index=2&t=0s)
  * <font color=red>Bulletproofs</font> [git](https://github.com/dalek-cryptography/bulletproofs)

* engineering
  * <font color=red>「教程」min-sized-rust: 该库演示了如何最小化Rust二进制大小</font>，[地址](https://github.com/johnthagen/min-sized-rust)

* 其他

  * <font color=red>实现TCP Part I</font>[视频](https://www.youtube.com/watch?v=bzja9fQWzdA)，[code](https://github.com/jonhoo/rust-tcp)
  * <font color=red> 类似于numpy的库的介绍</font>[地址](https://www.lpalmieri.com/posts/2019-02-23-scientific-computing-a-rust-adventure-part-0-vectors/)