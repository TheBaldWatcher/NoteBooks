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
    * The rationale behind the numbers, and whether they were derived from experimental or observational data. Even if the SLOs are totally ad hoc, this fact should be documented so that future engineers reading the document don’t make bad decisions based upon ad hoc data.
  * Improving the Quality of Your SLO
    * 如果SLI波动和用户使用的场景对不上，则可能是有部分情况SLI没cover住，需要refine
* 推广：
  * 高层共识、文化推进（文化衫、训练营、分享）
* monitoring: metrics vs logs。log更详细，metric更直观。前者发现原因，后者用于日常监控
  * metrics with purpose有哪些指标是要监控的
    * intend changes: version of bin, command-line flags, conf
    * direct dependencies
    * Saturation: 
      * Java: The heap and metaspace; Go: number of goroutines
      * 有些是hard limit，比如RAM、disk、Cpu；有些是软上线，比如threads, waiting times in queue, written logs
    * status of served traffic
* Alerting on SLOs
  * precision、recall、detection time、reset time
  * 报警的逐步优化
    * 瞬时值 > 阈值则告警：precision很差，很多误告警
    * 消耗的error budget大于某个时间窗口的配额则告警：precision很高，但reset time 过长
    * 或改为持续一段固定时间则告警：precision很高，但recall很差
    * burn rate: precision还行，但是recall还是有问题，低于燃烧率的不会告警
    * Multiple burn rate alerts定制多层告警: precision,recall 都很好；但需要管理更多参数、且需要做高优告警对低优告警的suppresion
    * Multiwindow, Multi-Burn-Rate Alerts，相较于上个，加个了1/12的短窗口，来优化reset time；但这会有更多的参数
  * 对于极端available goals需调整告警参数：相同错误率下，low-available的燃烧率会很低，导致告警不会触发；high-available的会导致收到告警时已经来不及处理，错误预算就被消耗了。Chapter16有改进的方法？
* Eliminating Toil
  * 衡量下roi，因为eliminating toil本身也是要话时间开发的，搞不好还会话很多时间
  * 如何衡量toil及其收益
    * 时间、 applied patch, a completed ticket, a manual production change, a predictable email exchange, or a hardware operation. As long as the unit is objective, consistent, and well understood
    * Track these measurements continuously **before**, **during**, and **after** toil reduction efforts
  * toil management
    * 尝试从源头上去除
    * reject the toil或者有意延迟（可以把toil集中起来处理，并归纳共性）
    * Use SLO to reduce toil——如果不去做并不打破SLO
    * Start with human-backed interfaces
    * promote toil reduction as a feature
    * start small and then improve
    * increase uniformity
* 书签
  * Eliminating Toil