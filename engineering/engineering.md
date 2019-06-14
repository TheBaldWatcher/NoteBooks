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

  



# Android

* [android接口不推荐用枚举](https://www.jianshu.com/p/504e50d4d320)。
* 查看符号`./aarch64-linux-android-addr2line -Cspfe xxx.so line`
* 安装apk：`adb install -r -t -d`

# pod

* pod repo push MeituRepo mtrealtimebodypose.podspec --use-libraries --allow-warnings --verbose --sources=http://techgit.meitu.com/iOSModules_public/specs,http://techgit.meitu.com/iosmodules/specs,http://techgit.meitu.com/MTlabBinarys/3rdpartyBinary/libopencv --skip-import-validation
* [pod关掉部分架构](https://stackoverflow.com/questions/42645429/cocoapods-wrapping-around-a-static-library-without-i386-architecture)：`s.pod_target_xcconfig = { 'VALID_ARCHS' => 'arm64 armv7' }`。



# 其他

* 在线化的xmind：www.processon.com

