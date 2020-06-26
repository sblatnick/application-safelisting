#!/bin/bash
shopt -s extglob #set extended globbing
set -e
cd ${0%/*}

#Utils:
function error() {
  >&2 echo -e "$(date +'%F %T') \033[31mERROR\033[0m: $@"
  exit 1
}
function log() {
  echo -e "$(date +'%F %T')   \033[0;1mLOG\033[0m: $@"
}
function warn() {
  >&2 echo -e "$(date +'%F %T')  \033[33;1mWARN\033[0m: $@"
}

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
      packages=(
        libattr
        attr
        linux-firmware
        kernel
        kernel-headers
        kernel-devel
        mpfr
        libmpc
        cpp
        libgomp
        #glibc-*?
        gcc
      )
      rpm -q ${packages[*]}

      log "pre: ready"
    ;;
  make) ##build the kernel module
      log "make"
      make
      log "make done"
    ;;
  install) ##Install the kernel module
      log "install"
      log "install done"
    ;;
  *) ##rsync to root@hostname and build
      NODE=${1}
      log "Copy to ${NODE}"
      cd ../
      rsync -uaP --no-o --no-g --exclude='.*' application-whitelisting root@${NODE}:~/
      ssh root@${NODE} "~/application-whitelisting/build.sh run"
    ;;
esac

