* data type

  * list:
    * 场景：获取最近n调记录、生产者消费者
    * Lpush/Rpop  -> brpop/blpop wait-time，减少redis调用
    * lmove: 场景reliable queue，circular list比如监控轮询
  * set
    * 发扑克牌
    * scard: cardinality，计数。srandmember
    * sunionstore set1 new_set 可以用来进行set复制
  * zset
    * zrange, zrangebyscore, zrevrange, zrank
    * zrangebylex, zlexcount,  zrangebylex hakers [B [P
    * zremrangebyscore
  * bitmap。页面浏览量。一般会拆分成小的bitmap，避免过长。
  * HyperLogLogs：pfadd，pfcount

* 杂项

  * ```bash
    redis-cli -h 11.146.32.10 -p 6379 -a 'DV!NUP4WjVrYve~%Ex'
    hmget packet_14480 fragment_num_1022
    
    ttl key
    cluster keyslot  ke_name # 查热key对应的分片
    ```

    