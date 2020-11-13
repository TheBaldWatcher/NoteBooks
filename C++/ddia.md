[toc]

### designing data-intensive app

* DB, cache, search index, stream, batch
* reliable, scalable, maintainable(靠扩维)
  * How do you ensurethat the data remains correct and complete, even when things go wrong internally? How do you provideconsistently good performance to clients, even when parts of your system are degraded? How do youscale to handle an increase in load? What does a good API for the service look like?
  * reliable:  
    * hardware faults(MeanTimeToFailure: about 10-50 years)
    * software errors(when error appears, think about: assumptions, testing, isolation, monitor)
    * human errors. 
  * scalable: 
    * what is the load, the performance?
      * p99, ServiceLevelAgreements(p50 responce < 200ms && p99 under 1s == service available. 99% of the year the service is available)
      * need to measure not only at service, but also at client(network delay, head-of-line blocking)
    * how would it grow, and how would our resources be
  * maitainable: operability, simplicity  evolvability

### data model: 

* StructuredQeuryLanguage vs NotOnlySql
  * NoSql: open source(free & modify)
  * Impedance mismatch: one-to-many(resume: a person may have multy career/ edu..,)
    * seperate table: query many times or several joins
    * Xml or Json datetype:
    * document(Json/ Xml)
      * schema flexibility 
      * locality:
        * one query is sufficient.
        * But no matter how much you need, the whole doc is loaded, and updating is expensive if it increases doc-size.
      * surpport for join is weak, consider **many-to-one/many**(resume: many-to-one: edu is refer to another table of schools)
      * cannot refer to nested item directly
* graph: surpport **complex many-to-many** butter than Sql. anything is potentially related to everything
* sql is explicited-schema(assumed on write), document & graph is implicit-schema(assumed on read)

### storage and retrieval:

* On Disk
  * LogStructeredMerge-tree:
    * append-only
      * much  simpler: speed, recovery
      * but it's keys must fit in memory, and cannot do range query
    * sorted key: SortedStringTable(sparse index, and a unsorted disk append-only table for recovery)
    * fast on write. because a keys can lay at different segments, so read is slow(from latest to older), and transaction isolation may need more work(because need to check several files to lock).
    * multi-times write(due to compaction & merging), may need to concern on SSD. -- wirte amplification
    * however, LSM is often sustain higher write throughput, as write amplification is often low, and sequential write
    * handle fragmentation better than B-Tree
    * compaction and merging of LSM may interfere performance, at a high percentile, this could be a problem
    * speed between compaction and merging can mismatch, i.e., compaction < merging. Need monitor
  * B-Tree:
    * fast on read
    * writne twice: one for WriteAheadLog. may be more when page is splite
  * other
    * cluster index: store data inside index
      * multi-column index: query multiple columns, such as `51<latitude<60 AND 0 < longtitude < 20`. HyperDex. 
* in memory —— redis, memcache
  * performance advantage is due to avoid the overheads of encoding in-memory data into a form that can be written to disk, instead of read from disk
  * it can support some impl that hard for on-disk. such as priority-que, sets.
* data warehouse:Extact-Transform-Load
  * for analysts to use, avoid their workload to affect the online service
  * and can have more optimize
  * star schema: fact_table(events): attributes + foreign keys(dimension tables). break dimension into subdimensions: snowflake chema.
  * column-oriented storage
    * read fewer bytes, so it's faster
    * better compression: bitmaps encoding -> run-length encoding(distinct value of this field is small, so there is lots of zeros)
    * allow more row to fit in L1-cache

### encoding and evolution

* forward compatible: old reader& new writer; backward compatible: old writer & new reader.
* protobuf/thrift
  * add/delete field:new added field must be optinal. Or old version is broken; And you cant delete a required field
  * unknown fields: old reader read from new writer. For protobuf < 3.5, when reencode later, Unknown fields is discarded.(protobuf 2.x is Ok)
* Avro ——dynamic generated schema
  * support better compaction, but you need to offer some schema information for reader and writer to compare
  * add/delete a field without default value will break forward/backward compatible
* rpc evolution: assume we upgrade service first, then client. So we need backward compatible on request, forward compatible on responce.

### replication: for failure(include offline), scalability, latency

* single-leader:

  * synch vs async(leader - follower):
    
    * synch: follower's date is up-to-date, but latency is unsure. One node's failure blocks whole DB. 
    * Semi-synch is more practical: only keep one follower is synch(if it slows down, a new synch node would be choosed)
    * Often, leader-based replication is configured to be completely asynchronous.(but data may lost when leader fails and is not recoverable)
    
  * set-up a new follower: get a snapshot, copy it to new node, catch up the changes since snapshot.

  * Node Outage:

    * leader-failure: determing leader has failed
      * may base on time out. but what is the proper timeout? when at load spike, unnecessary failovers make things worse.
    * choosing a new leader
      * split brain. may cause data lost or currupted.
    * reconfiguring to the new leader, and handling the old leader when it come back
      * some data in old leader may become conflict(may be discarded)

  * replication log:
    * statement-based replication: (not preferred)
      * leader send statements to followers. 
      * Defect: NOW(), WHERE<some condition>, side effect
    * WAL: 
      * like SST
      * Defect: too low level(bytes), may make followers use a differentversion harder, i.e. roll-upgrade harder. may require downtime.
    * logical(row-based)log replication: 
      * log about how row value should be changed. 
      * decouple logical log from storage engin. provide better compatible, and easier for external tools to parse and process.
    * trigger-based replication:
      * handle replica at app-level code: only repicate a subset/ from one DB to another/ handling conflicts
      * use a triggers/ stored precedures have greater overhead, and more error prone, but gain more flexibility.

  * consistency garantee:
    * read your own writing garantee: decide whether read from leader or follower, 

      * based on app content(such as user's own profile), or WAL, 
      * time(within 1 minutes, read from followers; else, from leader); or  compare timestamp(logical/wall clock) between last writes and replica.
      * multi-data centers and multi-devices makes things more difficult.

    * monotonic garantee:

      * read new data first time, and old data second time. 
      * Make first&second time read is from same replica. but if the replica failes, reroute to another replica is inevitable.

    * consistent prefix garantee: 

      * >  "About ten seconfs usually.", "How far into the future can you see?".

      * > make sure that any writes that are causally related to each other are written to the same partition

* multi-leader:

  * case 

    * mutli-datacenter.

      * compare to single-leader: performance(write to leader)/ tolerance of leader failure(no need to promte a new leader in another datacenter)/ network
      * downside: **conflict**, subtle and error prone. should better be avoided if possible.

    * clients with offline operation

    * collaborative editing: 

      * > We don’t usually think of collaborative editing as a database replication problem, but it has a lot in common with the previously mentioned offline editing use case."

  * conflict:

    * solving conflict 
      * Avoid conflict: write & read the same datacenter(i.e. the same leader group)--but this can be  violated, such as datacenter migrating/ datacenter outage.
      * LastWriteWin: decide on timestamp, may cause data lose. Other ways that simplely decides on a value have the same effect.(such as base on replica ID)
      * keep all conflicts, and let app to resolve it.

  * topology:

    * circular/star: one leader's failure stops whole communication.
    * all-to-all: causality problem. similar to 'consistent prefix garantee.'

* leaderless: quorum consistency

  * With totally `n` nodes, need at least `w` nodes successful for writing, and `r` nodes is read for reading. `w + r > n`.
  * how to catch up for a recovered node: read apair(check when read), and anti-entropy process(regularly scanning)
  * monitoring staleness: leader-based can metric on replication log, but leaderless has no such thing. And if  there's no anti-entropy process, data can be stale unlimitedly.
  * defect
    * sloppy quorum: may cause `w` nodes to be different nodes other than `r` nodes. Write is temporarily stored on some neighbour nodes when datacenter is unavailable.(Sloppy quorum is not a quorum, it's a useful assurance of durability) 
    * concurrent conflict, and read-repaire & hinted-handoff can arise conflict two.
    * read when writing
    * when writting, totally failure(fewer than `w`) doesn't make the successed roll back
    * fail with new value but recovery with old value, now quro fall below `w`.

  * conflict resolving
    * on read(if a data is seldom read, it may keep stale forever), anti-entropy process

* conflict resolving: 
  * LastWriteWin(or something like UniqId), may cause data lose, even for some data that is not concurrent.-- is data loss acceptable for app? or make data only written once, such as use UUID to be a key.
  * keep conflict as "A/B", or resolve on read.(app-level)
  * happens-before: 
    * don't know each other, instead of 'at the same time': 1,time measure is tricky for distributed system; 2,network delay or imformation transmit(impossible faster than light's speed) make process time inaccurate; 3,event time may lead to LWW-data loss.
    * version vector: 
      * for a replica: keep version number, read & write will return all concurrent subsets. When next write, merge them(tomestone for deleting).
      * for multi-replica, version number -> version vector. It's Okay to read from one replica, and write to another.

### partition

* skewed(some partitions have more data or queries than others)->hot spot.

* partition by primary key
  * by key-range: supports range query, but may have hot spot(on reading, such as celebraty; or on writing, such as records use timestamp to be the key)
  * by hash: tries to avoid hot spot, but can't do range query
    * if all workloads access a single key, it is still hot spot, for eg., celebraty
    * you can add pre/suffix to hot key to spread it into several nodes. But when query, you need to access all these nodes; and for non-hot-key, this overhead is not needed.
  * mix: partition by first column, sort by seond and follow column.(user_id + update_timestamp)

* partition by second index:
  * by document(local index): simple, write is simple, but read need to query all partitions(scatter&getter)-- try to structure your partitioning scheme so that secondary index queries can be served from a single partition
  * by term: read is simple, but write is complex(think about what if there's more than one second indexes. May lead to distributed transaction across all patitions.), and secondary indexes take time to get up-to-date.

* rebalancing
  * hash by key: number of partition is independent of the number of nodes(key boundry should use ranges instead of `%`)
  * fixed number of patitions: 
    * partition number is unlikely to change after setup. Need to choose a just right number
    * but it is hard. Too small number means single partition size becomes larger; too large number brings management overhead.
  * for above two, dynamic partition is that spliting or merging when size meet some condition.

  * partition by nodes: 
    * fixed number of partitions per node: partitions number will unlikely change
    * hash partition is used, in order to picking boundries randomly.
  * automatic vs manual: rebalancing is a very heavy op, consider some case(a node is slow at load spike, and other nodes conclude it is dead, and rebalance automaticly,  may make things worse, even cascading failure), manually do it is better.
  * request routing: roll poling/ routing tier(Kafka)/ client is awre of rebalancing

### transaction

* ACID: AICD, Abortability(A), Serializability(I)
  
  * Consistency means data in 'good state'. This also need application's effort, app defines what is valid or not, DB only stores it.
  * replication and durability: writing to disk & relicating to remote machines & backups should be used together.
  
* Abortable(atomic)

  * single vs multi obj opration
    * multi obj op: Abortability&Isolation, Sql DBs usually base on TCP connection; whereas noSql DBs usually don't have a way.
  * handling error & aborts:(abort and retry)
    * successed but ack failed due to network: need app-level deduplication mechanism.
    * overload: need to limit retries, or use exponential backoff
    * retry a permanent error is pointless, only retry temporary error(workload, lock, failover, network...)
    * if transaction has side effect(eg. email), you may need a 2PC.
    * if client fails during retrying, the data is lost.

* isolation

  * > *serializable* isolation means that the database guarantees that transactions have the same effect as if they ran *serially*

  * but serializable isolation has a performance cost, so weak isolation appears.

  * weak isolation 

    * read committed: no dirty reads/ writes

      * no dirty writes: row-level lock
      * no dirty reads: the same lock-- bad performance. So, most DB remember both old & new data when writing. And read will return the old data until write committed.

    * snapshot isolation: 

      * read committed is not enough. consider account transfer, between `A -100$` and `B + 100$`, the money is temporarily missing. This is *read skew(unrepeatable read)* .For some situation this is not acceptable(Backup, analytic queries&integrity checks)

      * impl: 

        * also use write locks to prevent dirty writes, but read use Multi-VersionConcurrentControl(**MVCC**). **write & read won't block each other.**

        * or copy B-trees from change node upto root

        * in PostgreSql: 

          * ```
            created by = 3
            deleted by = nil
            id = 1
            balance = 500
            ```

      * visibility

        * ignore: current in progress & aborted & later transaction ID.

    * lost updates: 

      * concurrent 2 writes, later over writer the other. Incrementing a counter/account balance, parsing Json & write it back, updating a wiki.

      * solution:

        * atomic write operations provided bt DB(recommended)

        * app-level lock. (sometimes the logic can not be represented by DB, you have to lock at app-level)

        * automatic detect and then abort the offending transaction

        * CompareAndSet: `UPDATE wiki SET content = 'new val' WHERE id = 1234 AND content = 'old val'`. DB should not allow WHERE clause to read from old snapshot, otherwise it's meaningless.

        * when take replica into consideration: 

          * > Atomic operations can work well in a replicated context, especially if they are commutative

          * CAS need a single up-to-date data, but for multi-leader/ leaderless, there's no such thing. You may need app-level confict solving or special data structures.

    * write skew(kind of generalized *lost updates*)

      * eg. : meeting room booking, multiplayer game(move to the same position), claiming a username, double-spending(spend more than you have).
      * `Query- Decision- Write`. phantom: `Write` change the `Query` result.
      * solution
        * the solutions for *lost updates* have no help: ~~atomic, automatic detect(for most impl)~~
        * may use: tiggers/ materialized view/  explicitly lock the rows that the transaction depends on.
        * if there's nothing to lock(e.g. `add`). consider materializing conflicts.(This is the last choice. A serializable isolation level is much preferable in most cases.)

  * serializability: week isolation has some defect. If it is not appropriate for you situation, consider serializable isolation

    * strongest isolation level. concurrent is as if executed one at a time.

    * actual serial execution: 
      * transaction must be small and fast; ant active dataset should fit in memory; wirte is slow.
      * limited to single CPU core, and transactions may be structured differently from traditional form
      * use stored procedure to avoid interactive waiting.-- different DB vendor may mave different lauguage, and it's grammar may seem ugly compare to today's. And it's hard to debug.
      * partition: 
        * some case we can seperate so that transaction only access one partition. This can  alleviate the workload.
        * but some case has to access multi-partition, that means lock patitions, which leads to terrible performance.

    * 2-phase lock: lock-acquire phase + all-lock-release phase

      * like a read-write-lock. **write block read, and vice versa**. -- for snapshot isolation, write and read never block each other.

      * bad performance, and dead lock may take place.

      * predicate lock: 

        * > it belongs to all objects that match some search condition

        * > If two-phase locking includes predicate locks, the database prevents all forms of write skew and other race conditions, and so its isolation becomes serializable.

        * diffcult to impliment and compare, usually decay to range-lock.

    * Serializable snapshot isolation: read-write need to be short, so conflict rate is low, so fewer abort.
      * prevent outdate premise:(detect doing&new added write)
        * detect **reads** of a stale MVCC object version(doing)
        * detect **writes** that affect prior reads.(new added)
          * every read update a transaction set, every write notify these transaction that their read may outdate. the first some commits success, laters abort.

### truble with distributed system

* you have to assume that you may get **no** responce: network, time synch, progress pause

  * unreliable **time out**: the time out is unbounded

    * TCP is packet switching, for which delay is unbounded; circuit switch's delay is bounded, but currrent tech does not support to use it.

  * unreliable **clock**: 

    * you have to assume that the time may be not sync, and can be paused. multi-CPU may have multi-timer, they may be mot synchronized.

    * wall-clock(NetworkTimeProtocal): 

      * > the best possible **accuracy** is probably to the tens of milliseconds, and the error may easily spike to over 100 ms when there is network congestion

      * defect: 

        * time drifts. (17s per day, depends on temperature)

        * need to connect to NTP server to synchronize, so it suffers network delay(from 35ms to seconds); And NTP servers can has a wrong time. maybe your PC is firewalled off from NTP servers.

        * time jump(sync's force resetting or VM's 'pause the world')

        * leap second(server impl), different NTP server's impl may vary.

        * may be set to some specific time by someone.

        * > Such accuracy can be achieved using GPS receivers, the Precision Time Protocol (PTP) . and careful deployment and monitoring. However, it requires significant effort and expertise, and there are plenty of ways clock synchronization can go wrong

        * relying on synchronized clock: problems with clock is hard to notice, program can still be runing. You need to carefully monitor clock offsets between all machines.

      * Timestamp for ordering events:

        * LastWriteWin: 
          * write from node with a lagging clock may be dropped.
          * cannot distinguish between quick succession and conccurent.

    * monotonic clock: for measuring time interval, not the time point.

    * the tricky cases:

      * Timestamp for ordering events:(wall-clock)
        * LastWriteWin: 
          * write from node with a lagging clock may be dropped.
          * cannot distinguish between quick succession and conccurent.
      * process pause: consider a leader node use *lease* to determine wheather it's still the leader.
        * wall-clock is not ok(synch), monotonic clock also has its problem(process pause)

* the truth is defined by the majority: 

  * leader node(or something that is unique in a system) is desided by qurom. consider this case: a node belieave itself the leader, but already been declared dead by others
  * only using a lease to determine leadership is not enough, use **fence token** along. This require the resource itself (and server,too) to participate in checking, instead of only client.
    * lock service(with a lease and later expire) -> fencing token(monotonic id)
    * by the way, ByzantineFault is too complex, so we don't consider
  * safety & liveness property
    * If a safety property is violated, it cannot be undone
    * liveness may not hold for some time, but it may **eventually** be satisfied.

### consistency and consuse: linearizability -> causality -> consensus

* linearizability: 

  * > The basic idea behind linearizability is simple: to make a system appear as if there is only a single copy of the data.    ... and all operations on it are atomic.

  * few system implementes linearizability bacause it's performance. In a network with highly variable delays, the response time of linearizable reads and writes is inevitably going to be high.

  * linearizability vs serializability:

    * serializability is for transaction: multi operations, multi objects, arbitrary total order
    * linearizability is for an individual object: single operation, single object, real-time order
    * **SSI** does not provide linearizability, as it excludes any writes after the snapshot.
    * linerizability + serializability = strict serializability. (trans A, trans B: for strict serializability, there order is A->B, with only serializability, the order may be B->A).[参考](http://www.bailis.org/blog/linearizability-versus-serializability/)。

  * relaying on linearizability:

    * lock&leader selection: a **linearizable** storage service is the basic foundation for these coordination tasks
    * uniqueness guarantee(require only one up-to-date value, username/back account)
    * cross-channel timing dependency: two users refreash the football score(Alic sees the match is over, but Bib sees it's still on going; image storage and then resize service(images mismatch).

  * implementing

    * single-leader replication: **potential** linearizability.
      * not always linearizability: SSI in use, bug: splict brain, or 'With asynchronous replication, failover may even lose committed writes'(recover and commited data conflicts)
    * consensus algorithm: linearizability
      * 'bear a resemblance to single-leader replication', and prevent split brain and stale replica(i.e. single-leader's problem).
    * multi-leader replication: not linearizability
    * leaderless replication: **probably not** linearizability
      * strict qurom does not provide linearizability: some medium transaction state is exposed.(update Replica1, A read R1&R2, B read R2 & R3,update R2&R3)-- a **synch** read repair can fix read/write, but can't fix CAS operation(it need consensus)
      * LWW conflict resolution based on time-of-day may be not consistent with event ordering time
    * `CAP` is misunderstanding and confusing, avoid use it.

* Odering: linearizibility -> causally

  * causally consistency(SSI provides this) is a partial order, linearizibility is total order(no concurrent).
  * linearizibility provides causally consistency.
  * you need to track of causal dependencies. But in fact it is impractical, and the transaction may only depend on some little subset of its reads.
    * using sequence number ordering.An increasing number for all replica
      * single-leader: transaction log may be used for generate this number(**potentially** but not always linearizability). The number is providing causal consistency by providing linearizability.
      * multi-leader/leader-less: 
        * Lamport timestamp: 
          * (counter, node_id). every node track `max_counter`. when it realize there is a greater value, update `max_counter`.
          * it is a little like version vector(distinguish concurrent&causally dependent), but Lamport  offers linearizability, and it's more compact but can't distinguishes concurrent. 
          * but Lamport is not sufficient to solve many common problems in distributed systems:
            *  you can decide the order only after all operations arrive(username). when a operation arrives, you may don't know whether there is another operation with a earlier timestamp, so it will insert and change the decided-order.---- total order broadcast is needed.
        * total order broadcast <=> linearibility <=> consensus
          * no message is lost & delivered to every node in the same order.
          * it is similar to linearibility, as both indicate there's an order of operations. but broadcast does not garantee the time of msg delivery.
          * implement a CAS op by total order broadcast: 
            * append a msg to log; read untile the msg back to you, check the winner(first) of the msg, to decide to abort or successed.
            * CAS is a write operation. For a read operation to be linearibility: it is similar to CAS, i.e. and a msg for read, in the log.  (or same other ways, skipped, read the book.)
          * as linearibility is equivalent to each other, we can build total order broadcast by linearibility: 
            * for every operation, increament-and-get a linearibility id
            * it is different from Lamport timestamp: there's no gap, you have to wait id-5-msg to arrive before you send id-6-msg.

* consensus

  * FLP: there is no algorithm able to reach consensus if there is a risk that a node may crash.  it is proved in the **asynchronous** system model, which can not use clocks or timeout(a random number is even sufficient for indentifyinga node failure). So FLP does not mean it's impossible for distributed system to make consensus.
  * 2-PhaseCommit: prepare-commit
    * after coordinate decide to `commit/abort`, there's no way wo abort, you need to keep retrying until success.
    * if a participant respond 'yes', it has to wait coordinate forever, even if coordinate fails. -- when waiting, the participant is *in dout*. and it may hold some **lock**, for 2-PL, this block both write and read.
    * 2-PC has a performance penalty(for MySQL, 10x slower), due to disk forcing(for coordinate recovery) and network round-trips.
    * heterogeneous distributed transaction: XA. many XA implementations have an emergency escape hatch called `heuristic decisions`, which may violate *atomicity*. It allows participant to decide to commit or abort unilaterally. This is for emergency, because when coordinate recovers, there may be some *orphaned* in-doubt transaction(due to log lost, corruption, software bug), and need manually resolved.
  * limitation of distributed transaction
    * coordinate iteslt is a DB, but often implemented un-highly available. it's failure block the whole DB
    * if coordinate is a part of the app, the app becomes stateful.
    *  Since XA needs to be compatible with a wide range of data systems, it is necessarily a **lowest** common denominator(e.g. it cant detect deadlock, does not work with SSI)
    *  distributed transaction fails when any participant fails, this is *amplifying failures*, which runs counter to our goal of building fault-tolerant systems.
  * limitation: 
    * consensus is kind of sync, so there is performance penality
    * require more node(3 nodes for tolerate 1 failure), but with linearizability, these failure nodes will be blocked.
    * consensus use timeout to decide weather a node is failed. so high network delay may cause more leader selection, and harm the performance.
  * zookeeper: to handle small data that can be stored in memory, and change on scale of minutes or hours. and it offer some useful features.

### batch processing

* map-reduce: break into blocks -> map -> sort -> reduce
  * reduce side join & grouping: 
    * sort-merge join/grouping: mapper(user, profile) -> sortted user/profile -> reduce
    * hot key: 
      * some nodes may process signifiantly more records. and M-P is unfinished until every node is finished.
      * randomnly send hot key to several reducer, and the other related imformation may need be replica to all these reducer.
      * or as Hive, specified hot-key explicitly, and then use a map-side join.
    
  * map-side join: no sort&reduce. Need some information about **size, partition, sorting**.
    
    * the output is patitioned & sorted the same way with the large input, whereas reduce-side join is by the join key.
      
    * broadcast hash join: the small dataset is broadcast to all nodes
      * the small dataset is completely loaded into memory
      * or stored by a read-only index on the local disk(the frequently used parts  will remain in the OS’s page cache)
    
    * partition hash join: both source is partitoned the same way, So mapper only to read one partition for every input
    
      * > If the inputs are generated by prior MapReduce jobs that already perform this grouping, then this can be a reasonable assumption to make
    
    * map-side merge join: both source is partitioned(partition hash join) & **sorted** the same way
    
      * a prior MapReduce job, it did **sorte** but didn't **merge**(maybe other task need the unmerged data). 
  
* stream processing
  * message system
    * what happens if producer send faster than consumer can process: drop, buffer, backpressure
    * what happens if nodes crash of temporarily go offline: any msg lost?
    *  the combination of load balancing with redelivery inevitably leads to messages being **reordered**
  * log-based msg broker: DB -> stream
    * combining the durable storage approach of DBs with the low-latency notification facilities of messaging
    * short come
      * consumers <= partitions
      * head-of-line blocking
    * case:
      * high message throughput, where each message is fast to process, msg ordering matters
  * ChangeDataCapture&EventSourcing: stream -> DB
    * CDC: 
      * impl: log-based broker. init-snapshot + log compaction 
      * mutable DB, low level & app transparent, log compaction can discard previous events for the same key
    * EventSourcing
      * immutabe event, app level, need the full history of all events to reconstruct the final state
    * immutabe 
      * pro: 
        * decouple DB&event-log about evolving, storing, 
        * easier sync among derived systems. 
        * concurrency control
          * downside: read your own write: synch or distributed transaction.
          * but it also simplifies some aspects of concurrency control. self-contained to avoid changing in several different places like multi-obj transaction. If the event log and the application state are partitioned in the same way,  then need no concurrency control for writes
  * peocessing stream
    * ComplexEvemtProcessing(query is saved, data is scaned)
    * analytics
    * maintaining *materialized views*
  * event or process time
    * event time(device) + event sent time(device) + server receive time(server)
  
* stream joins: 
  
  * stream-stream join:search id + click event
  
  * stream-table join:event + user profile table, the table maybe a DB with CDC(material view).
  
  * table-table join: Both input streams are database changelogs
  
  * time may matters(follow and the unfollow). as broker like Kakfa has no ordering between partitions, the join may be not reproducable.
  
    * > it is often addressed by using a unique identifier for a particular version of the joined record: for example, every time the tax rate changes, it is given a new identifier, and the invoice includes the identifier for the tax rate at the time of sale. This change makes the join deterministic, but has the consequence that log compaction is not possible, since all versions of the records in the table need to be retained.
  
  * faile ouver: exactly once
  
    * distributed transactions(consensus among different part)
    * idempotence(Kafka offset)
    * op-state also need to be checkpointed.
  
* final chapter
  * Data Integration
    * the first challenge is then to figure out the mapping between the software products and the **circumstances** in which they are a good fit.
    * so you inevitably end up having to cobble together **several** different pieces of software
  * Data Integration
    * reasoning about source(sync)
  * Unbundling database
    * federated DB(unify reads), unbundled DB(unify writes)



头条：模拟sliding win，stream &stream join


# other
* algorithm
  * game: S-P
    * https://leetcode.com/problems/flip-game-ii/
    * g(x) = mex{x_follow}; g(x) = {g(x_subset1) ^ g(x_subset2)}





背景

