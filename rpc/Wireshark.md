# Wireshark网络分析就是这么简单/艺术

* A、B同属一个网关，但B子网掩码配错了
  * B找A：正常通信
  * A找B：B沉默，导致通信不了（我猜的）
  * 同一子网，直接发包；非同子网，走默认网关。（触发ARP？）
  
* tips
  * 抓包：`ping -c 1 -l step_num`来进行多步抓包
    * `ping -l 1472 -f` 发送1472字节，且f设置DontFragment关闭分片。在超过MTU时会导致被丢弃。
  * 右键Follow TCP/UDP Stream可以自动进行过滤
  * 自动分析：
    * Wireshark的Analyze-->Expert Info Composite
      * Note中会显示重传。可以留意下重传的RTO（RecoveryTimeObject？）是否过长
    * Statistics-->TCP Stream Graph
  
* 重传
  * 万分之一的重传率也对吞吐有影响。超过千分之一有明显影响
  * SACK和NewReno有利于提高重传效率
    * 尤其是开启了延迟确认。如果没有SACK，会导致一个一个包确认，且每次延迟200ms
  * 超时重传和快速重传不一样，前者对吞吐影响大，后者影响小
    * 超时重传在等待RTO后，会调整拥塞窗口，进入慢启动、拥塞避免
    * 快速重传是接到了ack，但是< 发的seq。
  * 丢包对小文件影响比大文件严重：快速重传需要攒够3个Dup Ack，小文件攒不起会从【快速重传】退化至【超时重传】
  * 可能的问题：乱序导致快速重传，导致性能降低。如果乱序太大（3个Dup Ack未能在超时窗口到达），则导致超时重传，进入慢启动
  * 如果服务器带宽过大，客户端带宽小，客户端网络接不住这么大吞吐量，会导致丢包重传，影响吞吐量，可以考虑配暂停帧（只影响相邻设备）
  
* tcp延迟确认：交互式场景下，上一个包的确认信息延迟发送（200ms？），看看能不能装到下个包的中。——ack会推迟
  * Nagle：发出去的数据未确认前，将这段时间新增的数据merge到一起，等ack来了再把这个merge的包发出去。——发包应立即跟在ack后面
  * 可能会导致吞吐量下降
  * 粗略筛选延迟确认的包`tcp.analysis.ack_rtt > 0.2 and tcp.len == 0`。过滤的包数X0.2即浪费的时间
  
* NAT
  * 举的例子有些跟NetworkAddressTranslation有关。主要是客户端的ip经过NAT后被改掉了，导致server看到的是别的ip，导致之后的client请求被拒绝。
  
* 三板斧
  * Statistics-->Summary 看负载
  * Statistics-->Service Response Time-->ONC-RPC-->Program:NFS Version:3--> Create Stat 看各项操作耗时
  * Statistics->TCP StreamGraph->TcpSequenceGraph(Stevens)可以看下是否有卡顿
  * Analyze-->Expert Info Composite 看网络问题，如重传、乱序等
  
* 可能的尝试
  
  * ```bash
    # 失败的握手
    (tcp.flags.reset == 1) and (tcp.seq == 1)
    # 重传握手
    (tcp.flags.syn == 1) and (tcp.analysis.retransmission)
    # 粗略筛选延迟确认包
    tcp.analysis.ack_rtt > 0.2 and tcp.len == 0
    
    ```
  
* wireshark 提示
  
  * ```bash
    # [Packet size limited during capture]
    # 可能是抓包方式问题，调大tcpdump
    tcpdump -i eth0 -s 1000
    
    # TCP Previous segment not captured
    # 可能是漏抓（有对方回复的ACK）；可能是丢包（无对方回复的ACK）。后者可能是链路上某个MTU较小
    
    # [TCP window Full] [TCP zerowindow]
    # 前者表示无法向其发送数据，在途数据已满；后者表示其无法接受数据
    ```
  
* 拥塞点估算——在途字节数
  
  * 通过expert info找到第一个重传包
  * 过滤`tcp.seq == xxx`，其中xxx为第一个重传包的seq，找到重传包的原始包
    * LargeSegmentOffload(LSO)时，需要改为`tcp.seq < xxx`。此时分包会从cpu移交给网卡，比如：发方seq+len=0+2000，收放看到的是seq+len=0+1000, 1000+2000，此时按`tcp.seq==1000`查不到原始包
  * 清楚过滤，向上查找上一个服务端的包。并根据Seq+Len-Ack计算在途字节数。一般来说主要看client的（只回复ACK的话，在途字节数为0），即client发出的的Seq + len - sever发过来的ACK
  
* 一个劫持的例子
  
  * 运营商复制请求，在正常请求前，返回一个假的地址
  * ttl：3次握手在劫持前。可以根据握手的ttl去判断
  * 网络层identification每次会递增一定值。劫持的会有大的跳变
  * tcp层：劫持的看不到windows scale，会导致windows size特别大，切out of order
  
* 杂项

  * TTL：TimeToLive没经过一次路由就减少1，一般初始值为64