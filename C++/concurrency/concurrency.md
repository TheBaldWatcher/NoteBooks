[[toc]]



## LockBased

* Guidelines
  * 其他线程不能看到中间状态
  * api要仔细避免race condition
  * :star2:**留意exception**。
    * 第八章
      * 使用`packaed_task`或者`async`而不是`thread`，进行exception的传递
      * RAII的join()
  * :star2: 减少死锁：减少scope of locks and avoiding nested locks
  * 减少临界区、细化成多个细粒度的mutex

```c++

namespace LockBased {
template <typename T>
class ThreadSafeStack {
 public:
  ThreadSafeStack() = default;
  ThreadSafeStack(const ThreadSafeStack &other) {
    auto lock = std::lock_guard{other.m};
    data_     = other.data_;
  };
  ThreadSafeStack(ThreadSafeStack &&other) noexcept {
    auto lock = std::lock_guard{other.m};
    data_     = std::move(other.data_);
  };

  ThreadSafeStack &operator=(const ThreadSafeStack &other)     = delete;
  ThreadSafeStack &operator=(ThreadSafeStack &&other) noexcept = delete;

  ~ThreadSafeStack() = default;

  void Push(T value) {
    auto lock = std::lock_guard{mutex_};
    data_.push(std::move(value));
  }

  auto Pop() -> std::shared_ptr<T> {
    auto lock = std::lock_guard{mutex_};
    if (data_.empty()) {
      return nullptr;
    }
    std::shared_ptr<T> const res(std::make_shared<T>(std::move(data_.top())));
    data_.pop();
    return res;
  }

  auto Empty() const -> bool {
    auto lock = std::lock_guard{mutex_};
    return data_.empty();
  }

 private:
  std::stack<T> data_ = {};
  mutable std::mutex mutex_;  // mutable
};

template <typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() : head_(make_unique<Node>()), tail_(head_.get()) {}

  ThreadSafeQueue(const ThreadSafeQueue &other)     = delete;
  ThreadSafeQueue(ThreadSafeQueue &&other) noexcept = delete;

  ThreadSafeQueue &operator=(ThreadSafeQueue &&other) noexcept = delete;
  ThreadSafeQueue &operator=(const ThreadSafeQueue &other)     = delete;

  ~ThreadSafeQueue() = default;

  void Push(T new_value) {
    auto new_data = std::make_unique<T>(std::move(new_value));
    auto p        = std::make_unique<Node>();
    {
      auto lk              = std::lock_guard{tail_mutex_};
      tail_->data          = std::move(new_data);
      Node *const new_tail = p.get();
      tail_->next          = std::move(p);
      tail_                = new_tail;
    }
    data_cond_.notify_one();
  }

  auto WaitAndPop() -> std::unique_ptr<T> {
    auto old_head = WaitPopHead();
    return old_head->data;
  }

  auto TryPop() -> std::unique_ptr<T> {
    auto old_head = TryPopHead();
    return nullptr == old_head ? nullptr : std::move(old_head->data);
  }

  [[nodiscard]] auto Empty() const -> bool {
    auto lock = std::lock_guard{head_mutex_};
    return head_.get() == GetTailWhenHeadLock();
  }

 private:
  struct Node {
    std::unique_ptr<T> data{};
    std::unique_ptr<Node> next = {};  // 小心析构导致stackoverflow
  };

  auto GetTailWhenHeadLock() -> Node * {
    auto lock = std::lock_guard{tail_mutex_};
    return tail_;
  }

  // 必须在head_mutex_内，否则读到的tail后，tail被其他线程修改
  // 不存在死锁：lock顺序一致
  auto PopHeadWhenHeadLock() -> unique_ptr<Node> {
    if (head_.get() == GetTailWhenHeadLock()) {  // empty
      return nullptr;
    }
    auto old_head = unique_ptr<Node>(std::move(head_));
    head_         = std::move(old_head->next);
    return old_head;
  }

  auto WaitForData() -> std::unique_lock<std::mutex> {
    auto head_lock = std::lock_guard{head_mutex_};
    data_cond_.wait(head_lock, [&] { return head_.get() != GetTailWhenHeadLock(); });
    return std::move(head_lock);
  }

  auto WaitPopHead() -> std::unique_ptr<Node> {
    auto head_lock = WaitForData();
    return PopHeadWhenHeadLock();
  }

  auto TryPopHead() -> std::unique_ptr<Node> {
    auto head_lock = std::lock_guard{head_mutex_};
    if (head_.get() == GetTailWhenHeadLock()) {
      return nullptr;
    }
    return PopHeadWhenHeadLock();
  }

  // a dummy node, so pop、push operate on different nodes
  std::mutex head_mutex_;
  std::mutex tail_mutex_;
  std::unique_ptr<Node> head_;  // declare before tail, 构造函数里有依赖顺序
  Node *tail_;

  std::condition_variable data_cond_;
};

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class ThreadSafeLookupTable {
 public:
  using KeyType   = Key;
  using ValueType = Value;
  using HashType  = Hash;

  ThreadSafeLookupTable(unsigned num_buckets = 19, Hash const &hasher = Hash())
      : buckets_(num_buckets), hasher_(hasher) {
    for (size_t i = 0; i < num_buckets; ++i) {
      buckets_[i].reset(new BucketType{});
    }
  }

  ThreadSafeLookupTable(const ThreadSafeLookupTable &other)     = delete;
  ThreadSafeLookupTable(ThreadSafeLookupTable &&other) noexcept = delete;

  ThreadSafeLookupTable &operator=(const ThreadSafeLookupTable &other)     = delete;
  ThreadSafeLookupTable &operator=(ThreadSafeLookupTable &&other) noexcept = delete;

  ~ThreadSafeLookupTable() = default;

  auto ValueFor(Key const &key, Value const &default_value = Value()) const -> Value {
    return GetBucket(key).ValueFor(key, default_value);
  }

  void AddOrUpdateMapping(Key const &key, Value const &value) { GetBucket(key).AddOrUpdateMapping(key, value); }

  void RemoveMapping(Key const &key) { GetBucket(key).RemoveMapping(key); }

  auto GetAsMap() -> std::map<Key, Value> const {
    auto locks = std::vector<std::unique_lock<std::shared_mutex>>{};
    for (auto &bucket : buckets_) {
      locks.push_back(std::unique_lock<std::shared_mutex>(bucket.shared_mutex_));
    }

    auto res = std::map<Key, Value>{};
    for (auto &bucket : buckets_) {
      for (auto &item : bucket.data_) {
        res[item.first] = item.second;
      }
    }
    return res;
  }

 private:
  class BucketType {
   public:
    using BucketValue    = std::pair<Key, Value>;
    using BucketData     = std::list<BucketValue>;
    using BucketIterator = typename BucketData::iterator;

    auto ValueFor(Key const &key, Value const &default_value) const -> Value {  // return by value, not ref
      auto lock        = std::shared_lock{shared_mutex_};
      auto found_entry = FindEntry(key);
      return (found_entry == data_.end()) ? default_value : found_entry->second;
    }

    void AddOrUpdateMapping(Key const &key, Value const &value) {
      auto lock        = std::unique_lock{shared_mutex_};
      auto found_entry = FindEntry(key);
      if (found_entry == data_.end()) {
        data_.push_back(BucketValue(key, value));  // exception safe
      } else {
        found_entry->second = value;  // user type should provide exception safe
      }
    }

    void RemoveMapping(Key const &key) {
      auto lock        = std::unique_lock{shared_mutex_};
      auto found_entry = FindEntry(key);
      if (found_entry != data_.end()) {
        data_.erase(found_entry);  // noexcept
      }
    }

   private:
    auto FindEntry(const Key &key) -> BucketIterator {
      return std::find_if(data_.begin(), data_.end(), [&](const BucketValue &item) { return item.first == key; });
    }

    mutable std::shared_mutex shared_mutex_;
    BucketData data_;
  };

  auto GetBucket(Key const &key) const -> BucketType & {
    size_t const bucket_index = hasher_(key) % buckets_.size();
    return *buckets_[bucket_index];
  }

  std::vector<std::unique_ptr<BucketType>> buckets_;
  Hash hasher_;
  //   size_t bucket_mask_ = 0;
};

}  // namespace LockBased
```



# LockFree

* guideline

  * Memory_order是优化，最开始应使用`memory_order_seq_cst`
  * 留意ABA问题，尤其是内存重复使用时
  * 留意busy-wait，看看能否让threads间相互帮助完成任务，比如queue::push

* 定义

  * *Obstruction-Free*—If all other threads are paused, then any given thread will com- plete its operation in a bounded number of steps.
  * *Lock-Free*—If multiple threads are operating on a data structure, then after a bounded number of steps one of them will complete its operation.
  * *Wait-Free*—Every thread operating on a data structure will complete its opera- tion in a bounded number of steps, even if other threads are also operating on the data structure.

* pros and cons

  * mutex是串行的
  * robustness。在持有lock时thread挂了，对mutex，所有thread的data structure is broken。lock-free只是那个线程的没了。
  * 不会有deadlock，但是会有live lock
  * 可能会decrease overall performance。atomic虽然快，但出现频率比mutex高

* Hazard pointer等核心要解决的问题是，head已经从list上拿下来后，什么时候能delete的问题。一般会放到个zombie list里，之后从这个list上进行删除

  * hazard pointer主要思想：每个thread有自己的hp，当读head时，把自己的hp置位；读取完后恢复。删除数据时遍历所有thread hp，确保没有thread在使用这个head。如果有的话，就延迟删除

  * 由于要遍历hp数组里的所有ptr，感觉稍微有点慢

  * ```c++
    // hazard pointer
    namespace LockFree {
    
    // TODO(hongfengdong) 两个优化点：thread_local和批量检查
    namespace HazardPointerDetail {
    // 每个thread有自己的hazard pointer
    struct HazardPointer {
      static constexpr size_t MaxHazardPointers = 100;
    
      std::atomic<std::thread::id> id_{};
      std::atomic<void *> pointer;
    };
    // TODO(hongfengdong) thread_local and global
    static std::array<HazardPointer, HazardPointer::MaxHazardPointers> g_hazard_pointers{};
    
    class HazardPointerOwner {
     public:
      HazardPointerOwner() {
        // 在hazard_pointers_找个槽位，把当前thread id放进去
        for (size_t i = 0; i < HazardPointer::MaxHazardPointers; ++i) {
          auto old_id = std::thread::id{};
          if (g_hazard_pointers[i].id_.compare_exchange_strong(old_id, std::this_thread::get_id())) {
            hp_ = &g_hazard_pointers[i];
            break;
          }
        }
        if (nullptr == hp_) {
          throw std::runtime_error("No hazard pointers available");
        }
      }
    
      HazardPointerOwner(const HazardPointerOwner &)     = delete;
      HazardPointerOwner(HazardPointerOwner &&) noexcept = delete;
    
      HazardPointerOwner &operator=(const HazardPointerOwner &)     = delete;
      HazardPointerOwner &operator=(HazardPointerOwner &&) noexcept = delete;
    
      auto GetPointer() -> std::atomic<void *> & { return hp_->pointer; }
    
      ~HazardPointerOwner() {
        hp_->pointer.store(nullptr);
        hp_->id_.store(std::thread::id{});
      }
    
     private:
      HazardPointer *hp_ = nullptr;
    };
    
    // 类似于单链表+unique_ptr
    struct DataToReclaim {
      template <typename T1>
      void DoDelete(void *p) {
        delete static_cast<T1 *>(p);
      }
    
      void *data;
      std::function<void(void *)> deleter;
      DataToReclaim *next;
    
      template <typename T1>
      explicit DataToReclaim(T1 *p) : data(p), deleter(&DoDelete<T1>), next(nullptr) {}
    
      ~DataToReclaim() { deleter(data); }
    };
    // TODO thread_local
    static std::atomic<DataToReclaim *> nodes_to_reclaim = nullptr;
    
    auto GetHazardPointerForCurrentThread() -> std::atomic<void *> & {
      thread_local static HazardPointerOwner hazard_owner;
      return hazard_owner.GetPointer();
    }
    
    auto OutstandingHazardPointersFor(void *node) -> bool {
      return std::end(g_hazard_pointers) == std::find_if(std::begin(g_hazard_pointers), std::end(g_hazard_pointers),
                                                         [node](auto const &i) { return i.pointer.load() == node; });
    }
    
    void AddToReclaimList(DataToReclaim *node) {
      node->next = nodes_to_reclaim.load();
      while (!nodes_to_reclaim.compare_exchange_weak(node->next, node)) {
        // loop
      }
    }
    
    template <typename T1>
    void ReclaimLater(T1 *node) {
      AddToReclaimList(new DataToReclaim(node));
    }
    
    void DeleteNodesWithNoHazardPointers() {
      DataToReclaim *current = nodes_to_reclaim.exchange(nullptr);
      while (nullptr != current) {
        DataToReclaim *const next = current->next;
        // TODO(hongfengdong) 批量检查：不要每次pop都检查，每N次检查一次，这样每次pop检查(MaxHazardPointers + N) / N个节点
        if (!OutstandingHazardPointersFor(current->data)) {
          delete current;
        } else {
          AddToReclaimList(current);
        }
        current = next;
      }
    }
    }  // namespace HazardPointerDetail
    
    template <typename T>
    class ThreadSafeStack {
     public:
      void Push(T const &data) {
        auto new_node  = new Node(data);
        new_node->next = head_.load();
        while (!head_.compare_exchange_weak(new_node->next, new_node.get())) {
          // loop
        }
      }
    
      auto Pop() -> unique_ptr<T> {
        auto &hazard_ptr = HazardPointerDetail::GetHazardPointerForCurrentThread();
        Node *old_head   = head_.load();
    
        // extract head
        do {
          Node *temp = nullptr;
          // set harzard ptr
          do {
            hazard_ptr.store(old_head);
            temp     = old_head;
            old_head = head_.load();
          } while (old_head != temp);
        } while (nullptr != old_head && head_.compare_exchange_strong(old_head, old_head->next));
    
        hazard_ptr.store(nullptr);
    
        auto res = make_shared<T>();
        if (nullptr != old_head) {
          res.swap(old_head->data);
          if (HazardPointerDetail::OutstandingHazardPointersFor(old_head)) {
            HazardPointerDetail::ReclaimLater(old_head);
          } else {
            delete old_head;
          }
        }
        return res;
      }
    
     private:
      struct Node {
        std::unique_ptr<T> data;
        Node *next;
        explicit Node(T const &data_) : data(std::make_unique<T>(data_)) {}
      };
    
      std::atomic<Node *> head_ = nullptr;
    };
    }  // namespace LockFree
    ```

* Atomic<shared_ptr>

  * ```c++
    
    template <typename T>
    class ThreadSafeStack {
     public:
      void Push(T const &data) {
        auto const new_node = std::make_shared<Node>(data);
        new_node->next      = head_.load();
        while (!head_.compare_exchange_weak(new_node->next, new_node)) {
          // loop
        }
      }
    
      auto Pop() -> shared_ptr<T> {
        auto old_head = head_.load();
    
        while (old_head && !head_.compare_exchange_weak(old_head, old_head->next)) {
          // loop
        }
        if (nullptr != old_head) {
          old_head->next = {};
          return old_head->data;
        }
        return {};
      }
    
      ~ThreadSafeStack() {
        while (Pop()) {
          // loop
        }
      }
    
     private:
      struct Node {
        std::shared_ptr<T> data;
        std::atomic<shared_ptr<Node>> next;
        explicit Node(T const &data_) : data(std::make_unique<T>(data_)) {}
      };
    
      std::atomic<shared_ptr<Node>> head_ = nullptr;
    };
    ```

  * double counter：internal(atomic) + external(non-atomic)

  * ```c++
    namespace LockFree {
    
    // why 2 counter: decrease is atomic, but add can be non-atomic
    template <typename T>
    class ThreadSafeStack {
     public:
      void Push(T const &data) {
        CountedNodePtr new_node{.ptr = new Node(data), .external_count = 1};
        new_node.ptr->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(new_node.ptr->next, new_node.ptr, std::memory_order_release,
                                            std::memory_order_relaxed)) {
          // loop
        }
      }
    
      auto Pop() -> shared_ptr<T> {
        CountedNodePtr old_head = head_.load(std::memory_order_relaxed);
        for (;;) {
          IncreaseHeadCount(old_head);  // 先登记，再进行后续的deref
          Node *const ptr = old_head.ptr;
          if (nullptr == ptr) {
            return nullptr;  // no data
          }
          // aquire->relaxed: ptr->next > load: ptr->next > head && head > IncreaseHeadCount && IncreaseHeadCount > load
          if (head_.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) { 
            auto res = shared_ptr<T>{};
            res.swap(ptr->data);
            // -2: removed from list. you're no longer accessing the node
            auto const count_increase = old_head.external_count - 2;
            // release: swap > fetch_add
            if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
              delete ptr;
            }
            return res;
            // aquire->relaxed + aquire: fetch_add > delete. 可优化为relaxed + aquire, 只有一个thread会进行delete
          } else if (ptr->internal_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
            ptr->internal_count.load(std::memory_order_acquire);
            delete ptr;
          }
        }
      }
    
      ~ThreadSafeStack() {
        while (Pop()) {
          // loop
        }
      }
    
     private:
      struct Node;
      struct CountedNodePtr {
        int external_count;
        Node *ptr;
      };
      struct Node {
        std::shared_ptr<T> data          = nullptr;
        std::atomic<int> interncal_count = 0;
        CountedNodePtr next;
        explicit Node(T const &data_) : data(std::make_shared<T>(data_)), interncal_count(0) {}
      };
    
      void IncreaseHeadCount(CountedNodePtr &old_counter) {
        CountedNodePtr new_counter;
        do {
          new_counter = old_counter;
          ++new_counter.external_count;
        } while (
            !head_.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
      }
    
      std::atomic<CountedNodePtr> head_;
      //   std::atomic<shared_ptr<Node>> head_ = nullptr;
    };
    }  // namespace LockFree
    ```

  * queue

  * ```c++
    
    namespace LockFree {
    
    // TODO(hongfengdong) 有不少new-delete
    // Common techniques for optimizing memory allo- cation include having a separate memory allocator on each thread and
    // using free lists to recycle nodes rather than returning them to the allocator
    template <typename T>
    class ThreadSafeQueue {
     public:
      ThreadSafeQueue() : head_(new Node), tail_(head_.load()) {}
      ThreadSafeQueue(const ThreadSafeQueue &other)            = delete;
      ThreadSafeQueue(ThreadSafeQueue &&other)                 = delete;
      ThreadSafeQueue &operator=(const ThreadSafeQueue &other) = delete;
      ThreadSafeQueue &operator=(ThreadSafeQueue &&other)      = delete;
      ~ThreadSafeQueue() {
        while (auto const old_head = head_.load()) {
          head_.store(old_head->next);
          delete old_head;
        }
      }
    
      auto Pop() -> std::unique_ptr<T> {
        auto old_head = head_.load(std::memory_order_relaxed);
        for (;;) {
          // IncreaseExternalCount + ReleaseRef
          IncreaseExternalCount(&head_, &old_head);
          auto ptr = old_head.ptr;
          if (ptr == tail_.load().ptr) {
            return nullptr;
          }
          CountedNodePtr next = ptr->next.load();
          if (head_.compare_exchange_strong(old_head, next)) {
            auto const res = ptr->data.exchange(nullptr);
            FreeExternalCounter(old_head);
            return res;
          }
          ptr->ReleaseRef();
        }
      }
    
      void Push(T new_val) {
        auto new_data = std::make_unique<T>(std::move(new_val));
        auto new_next = CountedNodePtr{.ptr = new Node{}, .external_count = 1};
        auto old_tail = tail_.load();
    
        for (;;) {
          // ++tail_->external_count;
          // IncreaseExternalCount + ReleaseRef
          IncreaseExternalCount(&tail_, &old_tail);
          T *old_data = nullptr;
          if (old_tail->data.compare_exchange_strong(old_data, new_data.get())) {
            CountedNodePtr old_next = {0};
            // 其他线程已经帮我们设好了tail
            if (!old_tail->next.compare_exchange_strong(old_next, new_next)) {
              delete new_next.ptr;
              new_next = old_next;
            }
            SetNewTail(new_next, &old_tail);
            new_data.release();
            break;
          } else {
            CountedNodePtr old_next = {0};
            if (old_tail->next.compare_exchange_strong(old_next, new_next)) {
              old_next     = new_next;
              new_next.ptr = new Node{};
            }
            SetNewTail(new_next, &old_tail);
          }
        }
      }
    
     private:
      /*  CountedNodePtr
       *  _____________________            Node
       *  | external_count int |    _______________________          NodeCounter
       *  | ptr Node*          | -> |  data T*             |      __________________________
       *   ---------------------    |  count NodeCounter   | -- > |  internal_count int    |
       *                            |  next CountedNodePtr |      |  external_counters int |
       *                             -----------------------      --------------------------
       *  head, tail: atomic<CountedNodePtr>
       */
      struct Node;
      struct CountedNodePtr {
        int32_t external_count;
        Node *ptr;
      };
      // 相比stack的internal_count+external_count, queue多了一个tail，因此多一个external_counters
      struct NodeCounter {
        int32_t internal_count : 30;
        int32_t external_counters : 2;
      };
      struct Node {
        std::atomic<T *> data = nullptr;
        std::atomic<NodeCounter> count;
        std::atomic<CountedNodePtr> next;
    
        Node() {
          // data = nullptr
          auto new_count = NodeCounter{.internal_count = 0, .external_counters = 2};
          count.store(new_count);
    
          next.ptr            = nullptr;
          next.external_count = 0;
        }
    
        void ReleaseRef() {
          NodeCounter old_counter = count.load(std::memory_order_relaxed);
          NodeCounter new_counter;
          do {
            new_counter = old_counter;
            --new_counter.internal_count;
          } while (!count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                                  std::memory_order_relaxed));
    
          if (!new_counter.internal_count && !new_counter.external_counters) {
            delete this;
          }
        }
      };
    
      static void IncreaseExternalCount(std::atomic<CountedNodePtr> *counter, CountedNodePtr *old_counter) {
        auto new_counter = CountedNodePtr{};
        do {
          new_counter = *old_counter;
          ++new_counter.external_count;
        } while (!counter->compare_exchange_strong(*old_counter, new_counter, std::memory_order_acquire,
                                                   std::memory_order_relaxed));
        old_counter->external_count = new_counter.external_count;
      }
    
      static void FreeExternalCounter(CountedNodePtr *old_node_ptr) {
        auto const ptr           = old_node_ptr->ptr;
        int const count_increase = old_node_ptr->external_count - 2;
        NodeCounter old_counter  = ptr->count.load(std::memory_order_relaxed);
        NodeCounter new_counter;
        do {
          new_counter = old_counter;
          --new_counter.external_counters;
          new_counter.internal_count += count_increase;
        } while (!ptr->count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                                     std::memory_order_relaxed));
        if (new_counter.internal_count == 0 && new_counter.external_counters == 0) {
          delete ptr;
        }
      }
    
      void SetNewTail(CountedNodePtr const &new_tail, CountedNodePtr *old_tail) {
        auto const my_tail = old_tail->ptr;
        while (!tail_.compare_exchange_weak(my_tail, new_tail) && my_tail == old_tail->ptr) {
          // loop if spurious failure
        }
    
        if (old_tail->ptr == my_tail) {
          FreeExternalCounter(old_tail);
        } else {
          // 其他线程已经帮我们修改了tail
          my_tail->ReleaseRef();
        }
      }
    
      std::atomic<CountedNodePtr> head_;  // declare before tail_, 构造函数里有依赖顺序
      std::atomic<CountedNodePtr> tail_;
    };
    
    }  // namespace LockFree
    ```

  * 

# Data Structures

* 影响多线程性能的方面
  * contention
  * false sharing——hardware_destructive_interference_size
  * data proximity
* 举例：矩阵乘法，按行划分比按列划分好，减少false sharing；但更进一步，可以划分为若干个小矩阵，这样cache里要放的数据更少了。
  * 多线程访问的数据尽量紧凑放置
  * 减少每个线程要处理的数据量
  * 多线程间的数据，通过`std::hardware_destructive_interference_size`来隔开

# thread pool

```c++
namespace MyThreadPool {
class MovableFunction {
 public:
  template <typename F>
  MovableFunction(F &&func) : impl_(make_unique<ImplType<F>>(std::move(func))) {}

  MovableFunction() = default;
  MovableFunction(MovableFunction &&other) : impl_(std::move(other.impl_)){};
  MovableFunction(MovableFunction const &other) = delete;

  MovableFunction &operator=(MovableFunction &&other) {
    impl_ = std::move(other.impl_);
    return *this;
  }
  MovableFunction &operator=(MovableFunction const &other) = delete;

  void operator()() { impl_->call(); }

 private:
  struct ImplBase {
    virtual void call() = 0;
    virtual ~ImplBase() = default;
  };

  template <typename F>
  struct ImplType : ImplBase {
    ImplType(F &&f) : f_(std::move(f)) {}
    void call() override { f_(); }

    F f_;
  };

  std::unique_ptr<ImplBase> impl_ = {};
};

// TODO 用lock-free queue来实现更自然些
// stack模式，FIFO确保cache友好
class WorkStealingQueue {
 public:
  WorkStealingQueue() = default;

  WorkStealingQueue(WorkStealingQueue const &other) = delete;
  WorkStealingQueue(WorkStealingQueue &&other)      = delete;

  WorkStealingQueue &operator=(WorkStealingQueue const &other) = delete;
  WorkStealingQueue &operator=(WorkStealingQueue &&other)      = delete;

  void Push(MovableFunction item) {
    auto lock = std::lock_guard<std::mutex>{mutex_};
    queue_.push_back(std::move(item));
  }

  auto Empty() const -> bool {
    auto lock = std::lock_guard<std::mutex>{mutex_};
    return queue_.empty();
  }

  auto TryPop(MovableFunction &item) -> bool {
    auto lock = std::lock_guard<std::mutex>{mutex_};
    if (queue_.empty()) {
      return false;
    }
    item = std::move(queue_.front());
    queue_.pop_front();
    return true;
  }

  auto TrySteal(MovableFunction &item) -> bool {
    auto lock = std::lock_guard<std::mutex>{mutex_};
    if (queue_.empty()) {
      return false;
    }
    item = std::move(queue_.back());
    queue_.pop_back();
    return true;
  }

 private:
  std::deque<MovableFunction> queue_;
  mutable std::mutex mutex_;
};

struct ThreadJoiner {
  ThreadJoiner(vector<thread> &threads_) : threads_(threads_) {}

  ~ThreadJoiner() {
    for (auto &t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }
  std::vector<std::thread> &threads_;
};

class InterruptFlag {
 public:
  InterruptFlag() = default;

  void Set() {
    flag_.store(true, std::memory_order_relaxed);
    auto lock = std::lock_guard<std::mutex>{set_clear_mutex_};
    if (nullptr != thread_cond_) {
      thread_cond_->notify_all();
    }
  }

  [[nodiscard]] auto IsSet() const -> bool { return flag_.load(std::memory_order_relaxed); }

  void SetConditionVariable(std::condition_variable *cv) {
    auto lock    = std::lock_guard<std::mutex>{set_clear_mutex_};
    thread_cond_ = cv;
  }

  void ClearConditionVariable() {
    auto lock    = std::lock_guard<std::mutex>{set_clear_mutex_};
    thread_cond_ = nullptr;
  }

 private:
  std::atomic<bool> flag_               = false;
  std::condition_variable *thread_cond_ = nullptr;
  std::mutex set_clear_mutex_;
};
thread_local InterruptFlag this_thread_interrupt_flag;

struct ClearCvOnDestruct {
  ~ClearCvOnDestruct() { this_thread_interrupt_flag.ClearConditionVariable(); }
};

class InterruptibleThread {
 public:
  struct ThreadInterruptibleException : public std::exception {};

  template <typename FunctionType>
  explicit InterruptibleThread(FunctionType f) {
    std::promise<InterruptFlag *> p;
    internal_thread_ = std::thread([f, &p] {
      p.set_value(&this_thread_interrupt_flag);
      try {
        f();
      } catch (ThreadInterruptibleException const &) {
      }
    });

    flag_ = p.get_future().get();  // caution: 通过这样去拿到thread_local flag
  }

  void Interrupt() {
    if (nullptr != flag_) {
      flag_->Set();
    }
  }

  static void InterruptPoint() { throw ThreadInterruptibleException(); }

  template <typename Predicate>
  static void InterruptibleWait(std::unique_lock<std::mutex> *lk, std::condition_variable *cv, Predicate pred) {
    InterruptPoint();
    this_thread_interrupt_flag.SetConditionVariable(cv);
    auto guard = ClearCvOnDestruct{};
    InterruptPoint();  // 1
    // 设置个超时时间，以处理1、2之间出现了interrupt的情况。此时notify在wait前已经执行了
    while (!this_thread_interrupt_flag.IsSet() && !pred()) {
      cv->wait_for(*lk, std::chrono::milliseconds(1));  // 2
    }
    InterruptPoint();
  }

 private:
  std::thread internal_thread_;
  InterruptFlag *flag_ = nullptr;
};

// step1: pool
// step2: global + local
// step3: work stealing
class ThreadPool {
 public:
  using GlobalQueue = LockBased::ThreadSafeQueue<MovableFunction>;
  using LocalQueue  = std::queue<MovableFunction>;
  ThreadPool(size_t thread_count = 10) : done_(false), joiner_(threads_) {
    auto const real_thread_count = std::min<size_t>(std::thread::hardware_concurrency(), thread_count);
    std::cout << "Thread count: " << real_thread_count << std::endl;
    try {
      for (size_t i = 0; i < real_thread_count; ++i) {
        stealing_queues_.push_back(std::make_unique<WorkStealingQueue>());
      }
      for (size_t i = 0; i < real_thread_count; ++i) {
        threads_.push_back(std::thread(&ThreadPool::WorkerThread, this, i));
      }
    } catch (...) {
      done_ = true;
      throw;
    }
  }

  void Done() { done_ = true; }

  ~ThreadPool() { Done(); }

  template <typename MovableFunctionType>
  auto Submit(MovableFunctionType f) -> std::future<typename std::invoke_result_t<MovableFunctionType>> {
    using result_type = typename std::invoke_result_t<MovableFunctionType>;

    // a handler
    std::packaged_task<result_type()> task(f);
    std::future<result_type> res(task.get_future());

    if (nullptr != local_worker_queue_) {
      //   std::cout << "push to local worker queue" << endl;
      local_worker_queue_->Push(std::move(task));
    } else {
      //   std::cout << "push to global worker queue" << endl;
      global_worker_queue_.Push(std::move(task));
    }
    return res;
  }

 private:
  atomic_bool done_ = false;
  GlobalQueue global_worker_queue_;   // declare before threads_
                                      //   static thread_local std::unique_ptr<LocalQueue> local_worker_queue_;
  std::vector<std::thread> threads_;  // declare before joiner_
  ThreadJoiner joiner_;

  std::vector<std::unique_ptr<WorkStealingQueue>> stealing_queues_;
  static thread_local WorkStealingQueue *local_worker_queue_;
  static thread_local size_t my_index_;

  void RunPendingTask() {
    MovableFunction task;
    if (PopTaskFromLocalQueue(task) || PopTaskFromGlobalQueue(task) || PopTaskFromOtherThreadQueue(task)) {
      task();
    } else {
      this_thread::yield();
    }
  }

  void WorkerThread(size_t index) {
    my_index_           = index;
    local_worker_queue_ = stealing_queues_[my_index_].get();
    // cout << index << " " << local_worker_queue_ << endl;
    while (!done_) {
      RunPendingTask();
    }
  }

  auto PopTaskFromLocalQueue(MovableFunction &task) -> bool {
    return nullptr != local_worker_queue_ && local_worker_queue_->TryPop(task);
  }

  auto PopTaskFromGlobalQueue(MovableFunction &task) -> bool {
    auto item = global_worker_queue_.TryPop();
    if (nullptr == item) {
      return false;
    }
    task = std::move(*item);
    return true;
  }

  auto PopTaskFromOtherThreadQueue(MovableFunction &task) -> bool {
    for (size_t i = 0; i < stealing_queues_.size(); ++i) {
      auto const index = (my_index_ + i + 1) % stealing_queues_.size();
      if (stealing_queues_[index]->TrySteal(task)) {
        return true;
      }
    }
    return false;
  }
};
thread_local WorkStealingQueue *ThreadPool::local_worker_queue_;
thread_local size_t ThreadPool::my_index_;

}  // namespace MyThreadPool
```



