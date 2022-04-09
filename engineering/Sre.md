# ??

## Chapter 1

* 自动化：琐事、日常等事物
* 模糊Dev/Ops的界限：共同解决问题，不要分隔（比如可能注重不同的指标：上线数vs服务可用率）。尽量使用相同的工具集，这样便于交流
* SRE偏上线，DevOps偏大方向。





# SRE -Google

## chapter 1

* Unix系统内部细节+1-3层网络知识
* 拒绝复杂性、拥抱自动化
* 需求预测&容量规划：自然增长（准确的预测模型）+业务feature（准确的统计）；周期压测以将原始资源和业务容量对应起来
* [the for golden signals](https://sre.google/sre-book/monitoring-distributed-systems/#xref_monitoring_golden-signals)
* SLO -> SLI sepcification & SLI implementation
  * 如何制定SLI，注意，别太复杂
    * 定义SLO、定义user、定义user的使用方式、draw a high-leveld architecture diagram of your system
    * 可考虑的方面
      * Request-driven: availability, latency, quality(比如：有损服务)
      * pipeline: 漏斗相关？打分、doc数等
      * Storage: 比如时效性，需要留意，一般来说，最近一段时间的时效权重比较高(存储了10年的数据，但是用户可能只关心今天的)
  * Documenting the **SLO** and **Error Budget** Policy
    * The authors of the SLO, the reviewers (who checked it for technical accuracy), and the approvers (who made the business decision about whether it is the right SLO).
    * The date on which it was approved, and the date when it should next be reviewed.
    * A brief description of the service to give the reader context.
    * The details of the SLO: the objectives and the SLI implementations.
    * The details of how the error budget is calculated and consumed.
    * The rationale behind the numbers, and whether they were derived from experi‐ mental or observational data. Even if the SLOs are totally ad hoc, this fact should be documented so that future engineers reading the document don’t make bad decisions based upon ad hoc data.
  * Improving the Quality of Your SLO
    * 如果SLI波动和用户使用的场景对不上，则可能是有部分情况SLI没cover住，需要refine
* 推广：
  * 高层共识、文化推进（文化衫、训练营、分享）