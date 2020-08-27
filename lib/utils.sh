#!/bin/bash
shopt -s extglob #set extended globbing
set -e
#trap 'error "$BASH_SOURCE \"$BASH_COMMAND\"";exit' ERR

#Globals:
  KERNEL=$(uname -r)

#Functions:
  #Logging:
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

