* brpc参考：

  * $qps * latency \approx cores$: 使用同步；$>> cores$于则使用异步
  * bthread：1、无顺序要求；2、且不是并发rpc，而是并发计算&耗时>1ms。一般本线程实行最耗时的部分，以掩盖bthread切换的耗时
    * 不是coroutine：有work stealing和butex（bthread和pthread相互等待和唤醒）
    * 多个bthread工作于一个pthread
    * bthread里调用阻塞的pthread或系统函数，可能会影响rpc运行。规避办法
      * 加worker不一定管用：如果都在等待同一个资源
      * 区分io/worker线程：等于将rpc的阻塞转移到io/worker之间。但rpc可能在做无用功，没有意义。
      * 要么就限制最大并发
  * client
    * Channel
      * `Channel.Init()`线程不安全，`Channel.CallMethod`线程安全；Channel在发送异步请求后可以析构
      * 链接服务集群：不应每次请求时，动态创建：开销过大。
      * 同步访问时，CallMethod后可析构response、controller；但异步时，done可能用到二者，所以得保证其生命周期。
        * 使用`NewCallBack`构造。但需要分配3次内存：response+controller+done
        * 使用`google::protobuf::Cloure`: 代码不好看。
        * 非http的一般不用`Channle.CallMethod`，而是用`XXX_stub`。另外，记得`Join(call_id)`。另外异步时的`call_id`要在事先保存下来，因为rpc调用后controller随时可能被done删除。
    * Controller：对应某**一次**rpc的设置
      * 默认线程不安全；一般不共享所有权。且在rpc完成前，不能释放。
      * `ERPCTIMEDOUT`代表Rpc超时，不可重试；`ETIMEOUT`代表连接超时，可重试
    * 用BAIDU_REGISTER_ERRNO来注册error code，避免不同模块不一致
  
* basic

  * 使用pb3，并注意google规范
    * repeated field变量名要用复数
    * enum的0值需跟UNSPECIFIED后缀

* demo

  * Proto 里定义好rpc接口

  * Client.cc

    * ```
      ParseCommandLineFlags();
      GetCommandLineFlagInfo();
      
      trpc::TrpcConfig::GetInstance()->Init();
      trpc::ClientConfig client_config =
            trpc::TrpcConfig::GetInstance()->GetClientConfig();
            
      trpc::TrpcPlugin::GetInstance()->RegistryPlugins();
      
      client.InitCoroutine("spp_mt"); // ???
      
      GetProxy();
      
      trpc::coroutine::TrpcTaskExecuter exec;
      ```

    * 