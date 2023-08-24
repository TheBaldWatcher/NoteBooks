[toc]

# Sparks of Artificial General Intelligence

* Process-consistency: detailed context有助于减少GPT4的不稳定性。从output-consistency到process-consistency
* limitation——擅长增量任务；对非增量任务不行
  * Next-word prediction paradigm: 无法像人一样进行多轮、逐步的思考迭代
    * 比如：文章首尾句逆序，且符合语法、语义。
  * 数学算数很差
* 注意
  * 错误（比如hallucination）和真值混在一起，较难识别。不要over-reliance。
  * 双刃剑，如果用于不良目的很危险
  * bias——corpus内在如果有问题。如何比较好的prompt？
  * 对生产关系、人才培养（如教育）方面的影响
  * 其他
    * 有些人接触不到（地域、职业等），而能接触到的人会极大的释放生产力，会产生生产力的分化
    * 隐私问题
    * 知识产权
  * 未来的方向
    * Confidence calibration: 应对hallucination
    * Long-term memory
    * Continual learning
    * Personalization
    * Planning and conceptual leaps
    * Transparency, interpretability and consistency
    * Cognitive fallacies and irrationality
    * Challenges with sensitivity to inputs



## prompt

* 角色扮演，align
  * 以下是发生在患者和内科医生之间的对话
  * 你是一个python专家
* in-context learning
* cot貌似已经内置了
* [Prompt Engineering Guide](https://www.promptingguide.ai/)、https://github.com/dair-ai/Prompt-Engineering-Guide
  * introduction
    * avoid impreciseness
    * to do or not to do？——to do

  * techniques
    * Few-shot prompting
      * label space, distribution of the input text, format重要

    * Auto-CoT: 划分成N个小的zero-shot-CoT
    * Self-consistency: prompt给更多的，具有推导过程的例子
    * generate knowledge prompting: 给一些knowledge，让它给一个knowledge式的解答，或者解释某个结果
    * Tree of Thoughts具体怎么操作不太明白
    * RetrievalAugmentedGeneration, RAG。输入中加上wiki之类的信息，在prompt补充了时效性




* 一些trick
  * 清晰的提示
    * 各种分割符：`""", ```, ---, <>, <tag></tag>`，同时也能避免prompt注入
    * 要求结构化输出，如按json输出，便于解析
    * 检查条件：如果没有xx信息，回答：否
    * few-shot(in context learning)
  * 思考
    * 拆分成小任务，并用few-shot等方式进行解释
    * "请进一步思考", "let's think step by step"
    * 根据结果，逐渐、多步优化prompt
  * 其他
    * 角色扮演

​	



## PPO——近端策略优化(啥意思？)

* [基于RM来调优SFT模型](https://zhuanlan.zhihu.com/p/597100830)
  * 缺点
    * 主观因素
      * 生产demo数据的**人工**标注者的偏好
      * 设计和编写标签说明的研究员
      * prompt质量好坏
      * 标注者偏差既包含在RM模型训练中，也包含在模型评估中
    * 其他
      * 缺乏对照组：如何界定RLHF是不是真的有效果、有多大效果
      * 标注者偏好：本身存在bias、另外就是不同人的bias是不一致的

# 其他

* RLHF: Reinforcement Learning from Human Feedback
* CoT: chain of thought
* IFT: Instruction Fine Tuning
* SFT: Supervised Fine Tuning。一问一答
* RM: Reward Model，奖励模式。1个问题+1个好回答+一个坏回答