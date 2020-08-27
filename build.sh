#!/bin/bash
cd ${0%/*}
source lib/utils.sh

case "$1" in
  '')
      echo "Usage: ${0##*/} [hostname|action]"
      echo "  Actions:"
      sed -n "/^${section}\$/,/^##/ p" $BASH_SOURCE | grep -Eo '[^ (]+\)\s+#.*$' | \
      while read help
      do
        params=${help#*#}
        params=${params%%#*}
        col=$(printf "\033[33;1m%s\033[0m %s" "${help%%)*#*}" "${params}")
        printf "    %-30s %s\n" "${col}" "${help##*#}"
      done
      exit
    ;;
  run) ##run == pre, make, install
      $0 pre
      $0 make
      $0 install
    ;;
  pre) ##Check for dependencies (rpm)
      log "pre: checking for packages"
      #FIXME: remove unneeded packages
      packages=(
        rsync
        epel-release
        yum-utils
        rpmdevtools
        man-db
        autoconf
        automake
        libtool
        make
        telnet
        openssl
        net-tools
        wget
        less
        vim-enhanced
        which
        bzip2
        genisoimage
        createrepo
        squashfs-tools
        libattr
        attr
        linux-firmware
        kernel-${KERNEL}
        kernel-headers-${KERNEL}
        kernel-devel-${KERNEL}
        mpfr
        libmpc
        cpp
        libgomp
        glibc
        glibc-headers
        gcc
        jq
        p7zip
        p7zip-plugins
        the_silver_searcher
        expect
      )
      IFS=$'\n'
      set +e
      missing=$(rpm -q ${packages[*]} | grep 'is not installed' | cut -d' ' -f2)
      set -e
      for missed in $missing
      do
        log "yum install -y $missed"
        yum install -y $missed
      done

      log "pre: ready"
    ;;
  make) ##build the kernel module
      log "make"
      make
      log "make done"
    ;;
  install) ##Install the kernel module
      set +e
      log "remove old"
      rmmod asl
      set -e
      log "install"
      insmod asl.ko
      log "install done"
    ;;
  *) ##rsync to root@hostname and build
      NODE=${1}
      log "Copy to ${NODE}"
      cd ../
      rsync -uaP --no-o --no-g --exclude='.*' application-safelisting root@${NODE}:~/
      ssh root@${NODE} "~/application-safelisting/build.sh run"
    ;;
esac

