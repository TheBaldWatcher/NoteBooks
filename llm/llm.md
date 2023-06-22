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



# 其他

* RLHF: Reinforcement Learning from Human Feedback
* CoT: chain of thought
* IFT: Instruction Fine Tuning
* SFT: Supervised Fine Tuning。一问一答
* RM: Reward Model，奖励模式。1个问题+1个好回答+一个坏回答