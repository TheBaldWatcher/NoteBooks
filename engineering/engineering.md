[toc]



# gdb

* set print elements 0

* set print object on  ——派生类打印

* set print pretty on

* set scheduler-locking on|off|step

* step count， util, n count

* save breakpoints

* save breakpoints

* ```bash
  # 构造一个对象
  set $i=(SkillProxy *)malloc(sizeof(SkillProxy))
  $i->SkillProxy::SkillProxy($3, 4574003295787813)
  
  # 打印static variable in function
  $ nm -C a.out | grep test
  (gdb) p 'Instance()::test'
  ```

* 

# gitlab

* CI

  * 添加.yml文件，<font color=red>ruuner设置没有细看，用的是simple的</font>。

  * submodule可能会有权限问题：

    ```yml
    before_script:
       # install ssh-agent
      - 'which ssh-agent || ( apt-get update -y && apt-get install openssh-client -y )'
      # run ssh-agent
      - eval $(ssh-agent -s)
      # add ssh key stored in SSH_PRIVATE_KEY variable to the agent store
      - ssh-add <(echo "$SSH_PRIVATE_KEY")
      # disable host key checking (NOTE: makes you susceptible to man-in-the-middle attacks)
      # WARNING: use only in docker container, if you use it with shell you will overwrite your user's ssh config
      - mkdir -p ~/.ssh
      - echo -e "Host *\n\tStrictHostKeyChecking no\n\n" > ~/.ssh/config
      # begin to update submodule
      - git submodule sync --recursive
      - git submodule update --init --recursive
    ```

* 获取最新的提交

  ```shell
  spfUpdate3rdpartyToLatestTag()
  {
      spt3rdpartyName=$1
      cd $spt3rdpartyName
      git fetch --tags
      LatestTag=$(git describe --tags `git rev-list --tags --max-count=1`)
      if [[ $LatestTag = "" ]]; then
          echo "spfUpdate3rdpartyToLatestTag: $spt3rdpartyName LatestTag is empty ! start git pull"
          git pull 
      else
          git checkout $LatestTag
          echo "spfUpdate3rdpartyToLatestTag: $spt3rdpartyName tag: $LatestTag"
      fi
      cd ..
  }
  ```

* [Windows 使用 bash](https://www.windowscentral.com/how-install-bash-shell-command-line-windows-10)。

* centos需要在gitlab-runner下配置ssh。可以把root下配置好的，`cp /home/gitlab-runner && chown gitlab-runner: gitlab-runner ...`

* 获取当前git信息

  ```shell
  echo "non_win set_up_variants..."
  export BASE_DIR="$(pwd)"
  export BUILD_DIR="${BASE_DIR}/sdk_build"
  export SHARED_FILE_DIR="${BASE_DIR}/shared_files"
  export PhotoDetectorOutput="${BASE_DIR}/OutputGit/libmtphotodetector"
  export CURRENT_TAG="$(git tag -l --points-at HEAD)"
  export CURRENT_MESSAGE="$(git log -1 --pretty=%B)"
  export CURRENT_BRANCH="$(git branch --all --contains HEAD |  sed -E 's/(remotes\/origin\/)(.*)/\2/p' | uniq)"
  ```

* git 获取 commit

  * ~~~bash
    # 1. 如何获取当前分支迁出来的分支的第一个commit:
    git cherry -v master | sed 's/[ ]\+/ /g' | cut -d ' ' -f 2 | head -n 1
    
    # 2. 如何获取一个commit ID的父commit ID:
    git log --pretty=%P -n 1  THE_COMMIT_ID
    
    # 3. 如何获取当前master的commit ID:
    git log --pretty=%H -n 1 origin/master
    ~~~

# git

* 一些命令
  
  * ```bash
    # 查找公共提交？
    # https://stackoverflow.com/questions/8475448/find-merge-commit-which-include-a-specific-commit
    ```
  
  * 
  
* git log 

  * `git log --no-mergs`
  * <img src="./git log common option.png" alt="git log common option" style="zoom:50%;" />
  * <img src="./git pretty format.png" style="zoom:50%;" />
  * <img src="./git log-limit output.png" alt="git log-limit output" style="zoom:50%;" />
  * git config --global alias.last 'log -1 HEAD'

* branch
  * `gco [--track|banchYY] origin/branchXX`
  * `git branch -u origin/serverfix`  设置upstream branch
    * `git merge @{u}` shortcut for upstream branch

  * `git rebase --onto master server client`，将client从server基础上diverged的commit, rebase到master

* 多人合作distributed git

  * 建议使用`git format-patch`

  * 建议少用`git apply -i`（-i 表示交互模式），多使用`git am`

    * `git am --resolved`解决冲突。如果有patch的base commit，可以`git am -3`开启3-way maerge。

    * `git apply --check`可以查看结果

  * `git dif A...B`查看基于A、B公共commit的的B的diff。（3个点...而不是2个..）

  * `git config --global rerere.enabled true` 减少重复解决conflict的工作量

  * `git describe`显示最近tag到当前commit的信息。另外这个信息可用于check out

  * 归档：

    * ````shell
      # archive
      git archive master --prefix='project/' | gzip > `git describe master`.tar.gz
      # log
      git shortlog --no-merges master --not v1.0.1
      ````


# Android

* [android接口不推荐用枚举](https://www.jianshu.com/p/504e50d4d320)。
* 查看符号`./aarch64-linux-android-addr2line -Cspfe xxx.so line`
* 安装apk：`adb install -r -t -d`

# pod

* pod repo push MeituRepo mtrealtimebodypose.podspec --use-libraries --allow-warnings --verbose --sources=http://techgit.meitu.com/iOSModules_public/specs,http://techgit.meitu.com/iosmodules/specs,http://techgit.meitu.com/MTlabBinarys/3rdpartyBinary/libopencv --skip-import-validation
* [pod关掉部分架构](https://stackoverflow.com/questions/42645429/cocoapods-wrapping-around-a-static-library-without-i386-architecture)：`s.pod_target_xcconfig = { 'VALID_ARCHS' => 'arm64 armv7' }`。



# Bazel

* 查看依赖
  
  * ````
    sudo apt-get install graphviz
    
    xdot <(bazel query --notool_deps --noimplicit_deps "deps(:recommend_server_trpc)" --output graph)
    
    dot -Tpng my_depdency -o my_dependency.png
    ````
  
* dependency
  * bazel :[`local_repository`](http://docs.bazel.build/be/workspace.html#local_repository), [`git_repository`](https://docs.bazel.build/versions/master/repo/git.html#git_repository) or [`http_archive`](https://docs.bazel.build/versions/master/repo/http.html#http_archive)。
  * Non-bazel: prefix`new_`。 e.g., [`new_local_repository`](http://docs.bazel.build/be/workspace.html#new_local_repository)。
  * 获取依赖：`bazel build`自动获取。手动获取用`bazel fetch`(partly) 或`bazel sync`(all, update cache)。
  * diamond依赖：在`WORKSPACE`中也声明两个版本，并在`local_repository` 中用`repo_mapping = {"@testrunner" : "@testrunner-v1"}`将依赖映射到不同名字中，以共存。
  * [`--override_repository=foo=/path/to/local/foo`](https://docs.bazel.build/versions/master/command-line-reference.html#flag--override_repository)：覆盖依赖。可用于debug或mock。
  * 跨project transitive: WORKSPACE:A->B:C，依赖项C需要在A中声明，即不会传递依赖。这会造成BUILD膨胀，但能规避一些版本冲突。
  * layout:`ls $(bazel info output_base)/external`
  * 拿sha256
  
    * ```
      http_archive(
        name = "com_google_absl",
        urls = ["https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip"],  # 2020-02-11T18:50:53Z
        strip_prefix = "abseil-cpp-98eb410c93ad059f9bba1bf43f5bb916fc92a5ea",
        sha256 = "aabf6c57e3834f8dc3873a927f37eaf69975d4b28117fc7427dfb1c661542a87",
      )
      ```
  
    * To get the `sha256` of this URL, run `curl -sL --output - https://github.com/abseil/abseil-cpp/archive/98eb410c93ad059f9bba1bf43f5bb916fc92a5ea.zip | sha256sum -`。
  
* rule
  * 一个BUILD代表一个package，其中的元素为target
  * Label: `@myrepo//my/app/main:app_binary` ，即project + package + target。
    * 注意：label以`//`打头。因此`my/app`不是label，而是一个package
    * 注意，如果要被external引用，不应略去本project的名称，因为`//a/b/c`会代表external下的路径。应使用`@//a/b/c`
    * 简化：如果在同一project ->`@//my/app/main:app_binary` 
    * 简化：如果target和package末尾相同：`//my/app:app` -> `//my/app`
  * dependency: `src, deps, data`，其中data建议使用`data = glob(["testdata/**"]) `，而非label。`data = ["//data/regression:unittest/."] `只有文件夹增删时才会察觉到改变，修改其中文件则不会。
  * Debug: `$ bazel query --output=build 'attr(generator_function, my_macro, //my/path:all)'`

* bazel con
  
  * [BazelCon 2018 Day 2: Collecting Code Coverage With Bazel](youtube.com/watch?v=P51Rgcbxhyk)
  





# 安全规范

* 文件传输：

  * 文件名重命名为16位+的随机字符串
  * 敏感文件：加密、鉴权、水印

* 外部输入：各种校验：数据长度、数据范围、数据类型与格式

* 注意对环境的假定：命令注入、非法参数、路径劫持：

  ```c++
  // Bad 命令注入
  std::string cmdline = "calc ";
  cmdline += user_input;
  system(cmdline.c_str());
  
  // bad
  snprintf(buf, sizeof(buf), user_input);
  // good
  snprinf(buf, sizeof(buf), "%s", user_input);
  
  // 路径穿越
  char name[20] = "../file.txt";
  if (strstr(name, "..") != NULL){ return; }
  
  // 路径劫持
  HINSTANCE hinst = ::LoadLibrary("dll_nolib.dll");
  ```

* rand函数前需正确初始化

  ```c++
  // Good
  srand(time(0));	// caution, if not, rand() == rand(1)
  int foo = rand();
  ```



# 测试

## gmock

* > to do it "just right."  

  * 不要把行为描述的太具体，也不要太模糊
  * 所以`EXPECT_CALL`中用的是[matcher](https://github.com/google/googletest/blob/master/docs/gmock_cheat_sheet.md#matchers-matcherlist)，而不是具体是多少。——如果是non-overload，甚至可以直接省略

* `EXPECT_CALL`

  * [逆序匹配](https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#using-multiple-expectations-multiexpectations)。这样可以在开头放一个default版，后面覆盖一个特化版。
  * `InSequence seq`可以[限定次序](https://github.com/google/googletest/blob/master/docs/gmock_for_dummies.md#ordered-vs-unordered-calls-orderedcalls)。
  * [Controlling When an Expectation Retires](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#controlling-when-an-expectation-retires): 逆序匹配和`InSequence`的对比。这里有个`RetiresOnSaturation`，不过我觉得最好不要用，不然逻辑弄得太复杂了。

* [simpler interface](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#simplifying-the-interface-without-breaking-existing-code-simplerinterfaces): 用于接口参数较多、或者有多个重载时、在mock class的api中转发给MOCK_METHOD。这样只用mock少部分参数

* [delegating to fake/real](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#delegating-calls-to-a-fake-delegatingtofake)：对于一些方法，想用Fake/Real里的行为作为默认实现时：

  * 在`ON_CALL, EXPECT_CALL`前，调用`ON_CALL().WillByDefualt([]{...})`。一直有效
  * 或者在`EXPECT_CALL`里指定一下`WillOnce([]{...})`。只能有效一次

* [casting matchers](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#casting-matchers-safematchercast)：用来做类型转换，比如`int`->`long`, `base*`->`derived*`。

* [参数关系](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#matching-multiple-arguments-as-a-whole)。

  * `foo(x, y, z)`中$x<y<z$。：`With(AllOf(Args<0,1>(Lt()), Args<1,2>(Lt())))`
  * $0 <= x <= 100, and\ x \ne 50$。: `Matches(AllOf(Ge(0), Le(100), Ne(50)))`
  * 字符串: `EXPECT_THAT(Foo(), StartsWith("Hello"));`
  * 正则表达式: ` EXPECT_THAT(Bar(), MatchesRegex("Line \\d+"));`
  * 自定义Pred的Matcher: `EXPECT_CALL(foo, Bar(Truly(My_Pred_Functor)));`
  * 指针指向的参数: `EXPECT_CALL(foo, Bar(Pointee(Ge(3))));`

* [Matcher Container](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#matching-containers):

  *  ` EXPECT_CALL(mock, Foo(UnorderedElementsAre(1, Gt(0), _, 5)));`

  * ```c++
    EXPECT_CALL(foo, SendValues)
          .WillOnce(DoAll(SaveArg<1>(&actual_array), SaveArg<2>(&actual_proto)));
    EXPECT_THAT(actual_array, ElementsAre(1, 4, 4, 7));
    EXPECT_THAT(actual_proto, EqualsProto( ... ));
    ```

* [Ordered Call](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#expecting-ordered-calls-orderedcalls)

  * `InSequence`-block, or `InSequence`method( DAG)

    * [Changing a Mock Object's Behavior Based on the State](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#changing-a-mock-objects-behavior-based-on-the-state)

    * [invoke without args](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#invoking-a-functionmethodfunctorlambdacallback-without-arguments):

      * ```c++
         // WithArgs
         EXPECT_CALL(foo, ComplexJob(_))
              .WillOnce([] { Job1(); });
              .WillOnce(InvokeWithoutArgs(NewPermanentCallback(Job2, 5, 'a')));
          
          foo.ComplexJob(10);  // Invokes Job1().
         foo.ComplexJob(20);  // Invokes Job2(5, 'a').
          
        // Unused: type A, type B, ... -> 一个 Unsed
        double DistanceToOrigin(Unused, double x, double y) { ... }
        EXPECT_CALL(mock, Foo("abc", _, _))
            .WillOnce(Invoke(DistanceToOrigin));
        ```
    
    * 

* Action

  * [return live value](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#returning-live-values-from-mock-methods)：`Return(x)`会在内部存一个x的copy，之后都使用这个copy；如果要使用live的x，要使用`ReturnPointee(x)`, 
  * [Selecting an Action's Arguments ](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#selecting-an-actions-arguments-selectingargs): `EXPECT_CALL(mock, Foo).WillOnce(WithArgs<0, 2, 3>(Invoke(SomeFunc))); `

* 杂项

  * [Copy only](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#mocking-methods-that-use-move-only-types)

    * `Return(ByMove(...))`
    * 使用lambda或者functor

  * [compile faster](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#making-the-compilation-faster)：compile耗时主要是在构造/析构上。 如果被多处使用时，这块可以优化：在`.cc`文件使用一个空的构造/析构函数，即避免implicit inline。

  * [force verification](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#forcing-a-verification)

    * mock析构时会检查所有的expectation。但存在不析构的情况（内存泄漏或者其他）。可以手动触发: `VerifyAndClearExpectations`

  * [checkpoint](https://github.com/google/googletest/blob/master/docs/gmock_cook_book.md#using-check-points-usingcheckpoints)：到某个checkpoint点时希望处于某种状态

    * 清除状态：比如构造完成后，清除构造时的expectation：`VerifyAndClearExpectations`或`VerifyAndClear`。后者还会清除`On_Call`

    * 调用状态：`InSequence`。

      ```c++
      MockFunction<void(string check_point_name)> check;
      {
        InSequence s;
      
        EXPECT_CALL(mock, Bar("a"));		// ①
        EXPECT_CALL(check, Call("1"));	// ②
        EXPECT_CALL(check, Call("2"));	// ③
        EXPECT_CALL(mock, Bar("a"));		// ④
      }
      Foo(1); 	
      check.Call("1");  // 一定已经调用过①
      Foo(2);
      check.Call("2");	// 没有调用过④
      Foo(3);
      ```

    * verbose: 

      * 执行语句：` ::testing::FLAGS_gmock_verbose = "error";`
      * 堆栈信息：`--gtest_stack_trace_depth=max_depth`



* 接口测试：

  * 基本流、备用流、异常流
  * 用例精简——组合数学-正交实验
  * trpc自动生成用例&验证
    * `./trpc-cli -protofile=./comment.proto --interfacename=list -outputjson Danmu.json`
    * `./trpc-cli -datafiles=testdata.json -tarfet=ip://9.141.2.204:11006`



# granfana

* 

# 重构

* 镶金玫瑰：多个item，多层if嵌套，且判断有交集
  * 利用代码覆盖，简化多层if中不需要的if
  * 将if改为switch
  * 将case中的代码，改为多态对象的行为



# 持续交付2.0

* 聚焦业务，结果导向：持续交付2.0 + 精益创业（最小化可行产品用以验证）
  * 少做：主要矛盾、快速剪枝（分解需求，快速尝试&反馈&决策，量化效果）

# 其他

* 在线化的xmind：www.processon.com
* [cmake修改vs设置](https://my.oschina.net/u/221947/blog/413652)。

# Shell

* ```shell
  ${#arr[@]}  # 数组arr的元素长度
  ```



# AddressSanitizer

* [原理](https://www.usenix.org/system/files/conference/atc12/atc12-final39.pdf)
  * shadow memory：用来标记byte可用性
    * addr>> N + offset, N∈[1,8),
      * Byte可用性表示在bit(以Byte为单位存储这些bit)中。一个Byte有8bit，所以N<8
      * 表示2^N + 1个值：0~2^N个Byte是否可读。负数代表错误码
    * $ShadowMemorySize = \frac{ApplicationMemorySize}{2^N}$
    * $$ redzone >= 2^N$$  ，会存一些internal data：size, thread_id，malloc call stack之类的
  * Run-time library
    * malloc: 会分配额外内存，n regions对应n+1个 redzones
    * free：先隔离，再释放，FIFO队列，固定长度
  * 无法应对：
    * 大部分都为**都为false neg**，即应报未报。没有false pos。
      * 未对齐时的越界。比如从最后个Byte内中间的bite开始读——无很好的办法，被忽略
      * out-of-bounds太远，以至于落到了合法内存上，而非redzone
      * user-after-free：中间又有太多释放，导致隔离队列已被冲到
    * [也有个false pos的例外](https://github.com/google/sanitizers/wiki/AddressSanitizerContainerOverflow#false-positives), 不过太少见了。[D_FORTIFY_SOURCE](https://github.com/google/sanitizers/wiki/AddressSanitizer)

# ThreadSanitizer

* [知乎](https://zhuanlan.zhihu.com/p/38687826)、[论文-待读](https://static.googleusercontent.com/media/research.google.com/zh-CN//pubs/archive/35604.pdf)。[git](https://github.com/google/sanitizers/wiki/ThreadSanitizerAlgorithm) ，
  * 并行：充要条件：X、Y不可比，且$LS(X) \cap LS(Y) = \empty$
  * SegmentSet: ${S_i}$, 其中元素均不可比（date race的备选集）
  * TSan维护两个备选集：$SS_{write}, SS_{read}$
    * $SS_{write}$: write这一内存的事件
    * $SS_{read}$: 除read这一内存外，$\forall S_r \npreceq S_w$，即$S_w \prec S_r, or 不可比 $ 。
    * 判断是否data race：
  * 使用事项：
    * 需要代码都按flag进行重编。即对库、std支持不好。会有false pos/neg问题。可考虑[这些flag](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual#non-instrumented-code)。



# DDD

* 通过模型链接领域专家和开发人员。使用Ubiquitous Language
* 图+文档结合来表达模型。以文档为主，因为文档可以补充细节。如果以图为主会导致图臃肿繁琐
  * 图可以是模型的uml
  * 也可以使解释性的非uml、非标准化的、更随意的一些图
* 简单的项目不需要MDD+LayeredArchitecture，roi太低
* 第二章
  * 关联：简化：
    * 规定一个遍历方向
    * 添加限定符以减少多重关联
    * 消除不必要的关联
  * entity object：需用id进行标识和区分。这些id的构造可能需要领域知识
  * value object：不需要用id进行区分（区分是不同的、或是相同的）
  * service：若干操作的集合（这些操作不是entity obj或value obj的自然组成部分）
  * Module: 注意不要分的太细，这样不够内聚。理解时需要串联多个module。

