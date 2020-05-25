# 基础概念

* operator：source/sink、transformation、rolling aggregation、window operation
* time&state
  * processing time VS event time。前者是operator's machine time，而非event发生时间。导致难以重放过去的处理。
    * watermark：标注event time不同路数据的到达情况，但无法解决straggler task的问题。即有可能有数据晚于其watermark到达
    * processing time不需要watermark，所以延迟低
* 架构
  * failure recovery
    * flink对于standalone cluster下的process failure没法restart，这种情况建议运行standby JobManager和TaskManager
  * data transfer
    * buffer：输出和输出都需要buffer，如果某sender task有多个receiver，则需要多个buffer。（在large setups时，可能要tune一下buffers参数）
    * communication: edit-based flow control、task chaining
    * time: flink在event-time模式下，依赖于time的align。如果一个input stream不再advance watermark、或者input streams间watermark差异过大，会导致内存、latency暴涨——那要咋办呢？
  * state：task会有local版，所以需要一个backend来负责同步。
    * operation state：List、Union list、Broadcast
    * keyed state：value、list、map
    * save/check point：state copies are organized by backend storage提供的operator id + state name。但id依赖于其preceding operators，这会限制app的evolve。因此尽量不要用flink default，而是手动assign ids。
* Time-based and Window operators
  * best practice: assign timestamps and generate watermarks as close to the sources as possible
  * TimeAssinger & watermarks: Only for event-time. And watermarks = balance between latency and result completeness
    * AssignerWithPeriodicWatermakrs: periodically
      * assignAscendingTimeStamps
      * BoundedOutOfOrdernessTimeStampExtractor——handle late events
    * AssignerWIthPuncuatedWatermarks: based on input events
      * `checkAndGetNextWatermark()` + `extractTimestamp()`
    * ProcessFunctions: a family of low-level, and can access timestamps & watermarks
      * TimerService(inside Context and OnTimerContext):
        * register/deleteXXXTimeTimer is only for keyed stream. For no-keyed stream, use a dummy key to convert. 
        * for each key and timestamp, exactly one timer can be registered.
        * keyedProcessFunction: processElement + onTimer
      * Emitting to Side Outputs: Process Function提供了一个到其他type流的办法
      * CoXXXFunction
    * Window function
      * Custom trigger需要在`trigger.clear()`中清除state和timer——这部分window不会进行管理
      * trigger可以实现类似**process** function的功能。例子: be used to compute and emit eadly results from an event-time windows, before the watermark reaches the windows's end timestamp. 优点：low latency
      * merge：处理好各state。只可用于ListState, ReduceState, AggregatingState
* Statful Operators and applications
  * implement stateful functions
    * Broadcast：
      * 不能从`KeyedBroadcastProcessFunction::processBroadcastElement`去access keyed state，而应该是其中的`applyToKeyedState`。——broadcast是没有keyless的
      * parallelism＞1时，各parallel的envent到底次序不定。
    * 一个checkpoint是否完成，取决于所有的op，所以只有JM才知道。对于一些需要知道checkpoint完成的op，可以implement `CheckpointListener`接口，但这个接口并不保证每个checkpoint都会被通知到……
  * maintainous
    * unique identifier & maximum parallelism在启动后不可修改，从checkpoint重启时，这俩参数不能变——因此设置时要考虑到后期维护
  * Performance & Robustness
    * StatePrimitive：在RocksDBStateBackend上
      * ValueState每次都会全部deserialize
      * ListState只deserialize entry，且append开销小
      * MapState只deserialize访问到的key/value
      * 另外，一个op中尽量只update一次state
* 其他：`xxx._2`是第二个元素，即1-based







# proto3

* Scalar message fields: 无法区分默认值vs未设置！
* enum
  * 不建议用负数，编码效率不高
  * [proto2的enum不能再proto3中被直接使用](https://developers.google.com/protocol-buffers/docs/proto3#using-proto2-message-types)
* [field的更新](https://developers.google.com/protocol-buffers/docs/proto3#updating)
* encoding：
  * . **Always add a default case or explicitly call `Foo_IsValid(int)` outside of the switch to handle unknown enum values.**

abase lf hl   ！wj

端口：worker_slave.conf,  woker.conf  

Log4j____  R.File  Slow.File   Empty.file

search::ies::proto::VideoInfo