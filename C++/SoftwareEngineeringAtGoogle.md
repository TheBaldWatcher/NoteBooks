https://abseil.io/resources/swe-book

* live_at_head
  * "the most important problems in software engineering"
  * dependency provider需要明确：
    * how much compatibility is assumed?——旧
    * How much evolution is assumed?——新
    * How are changes handled?——handling
    * For how long are releases supported?——deprecated
  * 举例
    * c++ : indefinite backward compatibility
    * abseil：尽量保证，但不是indefinite，以避免太过拖累
    * boost：无保证
  * 当要引入一个dependency时
    * 接入前：What、Who、Time（多久+频率）
      * 其提供怎样的兼容性、谁提供的、我们会依赖多久、其更新频次是怎样的
    * 接入后：Who、How
      * 谁来升级，升级有多难
  * google由于采取大仓模式，dependency管理退化成source control
  * 一个例子：总之外部依赖要谨慎。boost这种广泛使用但不提供兼容性的库其实是有风险的
    * A引入了依赖，并只用了依赖中的part1
    * 之后这个依赖就对其他人，比如B可见了，其他人依赖了part2
    * 若干时间之后要升级该依赖（比如C发现了个安全问题），难以升级：A不了解part2，甚至也不知道part1随升级会怎样；对B来说这个库不是他引入的
  * google的解决办法：live at HEAD，即只依赖最新的稳定版本+CI+test。这样每个dependency在commit前需要过这些test。burden都在commiter这边。
  * 对比：SemVer无法满足要求
    * over-constrain：lib中包含了Foo和Bar，app只用了Foo，但库升级Bar会导致app无法编译
      * 顺带说下，breaking change取决于上下文，比如这里的例子，Bar的更新是否算breaking change要看app是否用到
    * over-promise：Hyrum's Law，编译的兼容性并不代表行为的兼容性。——不过话说回来，Live in HEAD也无法保证吧。