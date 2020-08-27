#!/bin/bash
shopt -s extglob #set extended globbing
set -e
cd ${0%/*}

kernel=$(uname -r)

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

#trap 'error "$BASH_SOURCE \"$BASH_COMMAND\"";exit' ERR

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
        kernel-${kernel}
        kernel-headers-${kernel}
        kernel-devel-${kernel}
        mpfr
        libmpc
        cpp
        libgomp
        glibc
        gcc
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

