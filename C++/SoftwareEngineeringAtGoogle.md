https://abseil.io/resources/swe-book

* Preface
  * keep in mind:
    * Time and Change
    * Scale and Grouth
    * Trade-offs and Costs
* What is Software engineering
  * The multiperson development of multiversion programs
    * Time： sustainability
    * Scale: 规模扩大后，是否会影响到效率
  * Time and Change——大多程序总是会要change的
    * 有些程序是一次性的，有些程序是长久的。对于不同的任务，engineering的价值也不一样
    * 程序可能会从一次性过渡到长久性的。开始进行升级的过渡期是最难受的
      * 这是一个之前未做过的任务，有很多隐含的坑
      * 缺少经验
      * 工总量往往比预期的大
    * 不要忘了Hyrum's Law
    * 例子：hash ordering
      * ordering是不保证的，但可能会有人就是会依赖这个ordering。
      * "it works" vs "it's correct"。"works now" vs "works indefinitely"。不要"clever/hacky"
  * Scale and efficiency——
    * 当具有一定规模后，改动会有cost，superlinearly cost是无法承受的——无论是人力，还是计算资源、code base等方面
    * polices that don't scale
      * deprecate：不要让user去升级，应该提供方去owner这个事情。这对user来说是一次性、用完即弃的经验
      * dev branch：主干开发，避免游离分支。否则，游离分支改动过大，合入master时可能导致其他游离分支的问题，需要testing和resyncing
    * policies that scale well
      * 如果infrastructure team升级导致了问题，但项目的CI里没有对应测例，锅就不在infrastructure team
      * 分享知识
    * 例子：upgrade compiler
      * 是非常painful的，有5年没升级过了
      * 既然升级是重要的，那就需要做一些列改动让这件事的cost变地可接受：更加频繁的升级，Beyoncé Rule策略等
  * Trade-off and Costs
    * make good decision是工程中另一个非常重要的点。每一个decision需要一个明确的reason支撑
      * 这么做是因为我们must——legal requirement、customer requirement
      * 基于当前情况下，最好的选择
      * Data-driven decision意味着要revisiting decision, admit mistakes。因为数据change over time
    * cost：钱、机器（计算资源）、人力、交易成本、机会成本、社交成本。鸡教人射钱钱
    * 例子：
      * Markers。被人顺走白板笔 vs 顺畅的脑暴。换句话说，ROI？
      * distributed builds：从本地编译切到分布式编译。未预见到的cost增长。意味着得考虑一些buffer？
* How to Work Well on Teams
  * Insecurity is a symptom of larger problem
  * Don't hide：多交流，别隐藏，别怕错，别到最后一步才发表。确保在正确的方向、减少失误、不是重复工作
    * bus factor：多交流，避免人员上的单点
  * three pillars of social interaction: 谦虚、尊重、信任
  * 人际关系很重要，可以极大改善项目推进、救火。
  * lose ego
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
    * over-promise：Hyrum's Law，编译的兼容性并不代表行为的兼容性。——不过话说回来，Live in HEAD也无法保证吧
    * 不是说SemVer没效果，而是说需要一些前提，但这些前提在实践中不好保证
  * 理想情况——并退化成Live at HEAD
    * 所有信息都可见（或者将private部分除开）
    * SemVer要表达的是"In my estimation, this change will be easy(or not) to adopt"，另一个表述方式就是跑所有受影响方的相关测试——包括多个版本
    * 但我们没有这么多资源，所以只能退而求其次，只跑当前最新的、稳定版本的版本的测试——即Live at HEAD
  * 提了个开源的失败例子gFlags
    * 主要是没做好维护，导致internal和external逐渐diverge
    * 然后external的api会比较稳定，比较portable。导致有些团队会同时依赖internal和external
    * 然后interval版本发生了改动，导致编译失败
  * TL;DRs
    * prefer source control to dependency management
    * SemVer is a lossy-compression
    * By comparison, testing and CI provide actual evidence of whether a new set of versions work together