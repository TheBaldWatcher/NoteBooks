https://abseil.io/resources/swe-book

[TOC]



## Preface

* keep in mind:
  * Time and Change
  * Scale and Grouth
  * Trade-offs and Costs



## What is Software engineering

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

## How to Work Well on Teams

* Insecurity is a symptom of larger problem
* Don't hide：多交流，别隐藏，别怕错，别到最后一步才发表。确保在正确的方向、减少失误、不是重复工作
  * bus factor：多交流，避免人员上的单点
* three pillars of social interaction: 谦虚、尊重、信任
* 人际关系很重要，可以极大改善项目推进、救火。
* lose ego
* 学会给出、与接受评论
* Blameless post-mortem culture: 复盘很重要。过度的批评会压制复盘文化
  * 复盘简介
  * 时间线
  * 根因
  * 影响、损失评估
  * 操作：近期修复
  * 操作：长期改进
  * 经验教训：bad（需要避免）、good（可借鉴之处）、lucky（发现的其他知识）

## knowledge sharing

* 避免information islands(限于局部、重复工作、错误理解)、避免单点风险、parroting、haunted graveyards
* 文档化（可复用，scalable）
  * 注重中心/团队文档建设。
* 需要有一个好的心智环境：承认自己不知道，不指责别人不知道——TechLeader需要保证这样的环境
  * 鼓励问问题、对解答者给予物质激励
  * 注重peer bonus/kudos，推动这种文化
  * 有论坛、讨论组、中心文档等方式去分享、传递；canonical
* Readibility：通过1-5%的held to highest standards的人来带动氛围。强化&传播影响，比文档有更好的效果
  * static analysis很好
  * Chesterton’s fence。去理解上下文（拓展知识面），尤其是要进行修改时，要明白它在那里的原因

## Engineering for Equity

* 考虑弱势群体、少数群体。我们始终是有bias的，要意识到，并不断改进、提升包容性

## How to Lead a Team

* TL;DRs
  * focus on leadership, influence, and serving your team
  * Delegate where possible, don;t DIY
  * Pay particular attention on the foucs, direction, and velocity of your team

* Manager，协调业务与团队成员
* TechLeader：负责技术细节，将任务按能力、技术栈分配给各同学。TL有时也会进行开发，但也会将任务分配出去（可能会效率会慢一些，但能培养人才）
* TechLeaderManager: TL + M，需要平衡自身任务、任务指派、管理工作
* 为什么要做管理者：比写代码更有意义，是在做一件事，也更高级，影响更大
* 怎么做好管理者：servant leadership。服务于团队，确保团队良好氛围，有安全感。
  * Traditional managers worry about how to get things done, whereas great managers worry about what things get done (and trust their team to figure out how to do it)
  * failure is an option
* anti-pattern
  * hire pushovers —— 和比你聪明的人工作
  * Ignore low performers ——提供帮助，看看是否好转
  * ignore human issues——不通人情
  * be everyone's friend——距离是会存在的，一起就餐作为社交的补充，工作时还是要有态度
  * Compromise the hiring bar——不要降低标准，滥竽充数
  * treat your team like children——给予信任。你的态度会塑造对方的行为模式
* Positive pattern
  * lose the ego——对团队给予信任，接受质疑，承认错误
  * zen master——是众人关注的焦点，一举一动都会为人所解读。keep calm，ask question，三思而行
  * be a catalyst——形成共识，让成员自愿地向某个方向迈进
  * remove roadblocks——解决团队遇到的问题，协调资源、联系人脉
  * be a teacher or mentor——让成员成长
  * set clear goal——劲往一处使
  * be honest——清晰地传达信息（不要用三明治法）
  * trak happiness——关注成员的幸福感，关注他们的需求
  * delegate, but get your hands dirty——不脱离一线，能承担难啃的工作
  * Seek to replace yourself——培养继承者
  * know when to make waves——当断则断，不要侥幸，及时行动
  * shield your team from chaos——屏蔽压力与杂音
  * give your team air cover——与团队站在一起
  * let your team know when they're doing well——不吝赞美
  * Easy to say "yes" to something that's easy to undo——比如成员想尝试些新想法

* people are like plants——如何激励成员，因材施教
  * 相比外因，内因更具驱动性
  * autonomy——owner精神，自治->自驱
  * mastery——让他们能不断成长技艺
  * purpose——赋予意义

## Leading at Scale

* Always be deciding
  * 定义盲区
  * 定义key trade-offs
  * decide, then iterate——小步快跑
* Always be leaving——让团队自驱，减少自身负担。你的时间、精力是有限的
  * 划分权责。负责人的聚焦与自由
  * delegating。让下属成长。做别人做不了的工作，这样才有意义。比如：
    * 承上启下：cover团队，向上沟通
    * 蓝图规划
  * adjusting and iterating。不断调整，不要锚定
* Always be scaling
  * 工作是spiral的：承接工作-解决工作-承接更大的工作（但可能仍是现在的资源）
  * important versus urgent
    * 重要的事不紧急，紧急的事不重要。不要被日常工作中的紧急事项淹没，进入reactive mode。
    * deletegate：do what only you can do
    * schedule dedicated time：定期花2h在重要不紧急的事上，如：team strategy, career paths ofr your leaders, collaborate with neighbouring teams.
    * learn to drop balls：关注top20%，其他80%要允许drop。如果某件80%中的事真的重要，它会想办法回到top20%的
    * protecting your energy：vacation、weekends、breaks during the day(per 90m)

## Measuring Engineering Productivity

* 例子：readability
  * Is It Even Worth Measuring?
    * 提问
      * 预期的结果是什么?why？
      * 需要采取何种行动？——当数据支持/不支持你的预期时
      * Who&When to take action on the result？——如果没有执行者，measuring就没有意义。执行者是否认可这个结论、相信这个数据，主观感受是怎么样的。
    * 我们知道了，对于某项改动
      * 是否afford
      * 是否受其他因素影响：决策者不认可self-report、易被个别情况动摇等
      * 无论measure与否，都会进行/不进行，那measure就没有多少意义
      * metrics不准确。这时候会有主观选择性：支持预期时，忽略准确性问题；不支持预期时，拿准确性说事儿
  * 如何指定metrics——GSM，goal、signal、metrics
    * 以goal为导向制定metrics，而不是基于现在手上有什么metrics
    * 对metrics达成公示，各方认账
    * 界定哪些是可测的，哪些是不可测的
  * goal
    * 常见的问题是，主要聚焦在某些方向，忘了考虑trade off
    * Quants
      * **Qu**ality of code：代码质量
      * **A**ttention from engineers：专注性，context switch频率
      * I**n**tellectual complexity：心智、工作复杂度
      * **T**empo and velocity：完成的速度
      * **S**atisfaction：工程师对任务的满意、认可程度
  * signal：达成goal时的一些信号
  * metrics：signal的proxy，通过这些来量化signal

## live_at_head

* "the most important problems in software engineering"
* dependency provider需要明确：
  * how much compatibility is assumed?——旧
  * How much evolution is assumed?——新
  * How are changes handled?——handling
  * For how long are releases supported?——deprecated
* 举例
  * c++ : indefinite backward compatibility
  * abseil：尽量保证，但不是infinite，以避免太过拖累
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