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

  



# Android

* [android接口不推荐用枚举](https://www.jianshu.com/p/504e50d4d320)。
* 查看符号`./aarch64-linux-android-addr2line -Cspfe xxx.so line`
* 安装apk：`adb install -r -t -d`

# pod

* pod repo push MeituRepo mtrealtimebodypose.podspec --use-libraries --allow-warnings --verbose --sources=http://techgit.meitu.com/iOSModules_public/specs,http://techgit.meitu.com/iosmodules/specs,http://techgit.meitu.com/MTlabBinarys/3rdpartyBinary/libopencv --skip-import-validation
* [pod关掉部分架构](https://stackoverflow.com/questions/42645429/cocoapods-wrapping-around-a-static-library-without-i386-architecture)：`s.pod_target_xcconfig = { 'VALID_ARCHS' => 'arm64 armv7' }`。



# 其他

* 在线化的xmind：www.processon.com
* [cmake修改vs设置](https://my.oschina.net/u/221947/blog/413652)。

