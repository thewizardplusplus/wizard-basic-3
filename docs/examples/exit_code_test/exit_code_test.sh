#!/usr/bin/env bash

readonly base_path=`dirname $0`
readonly script_name=`basename $0 | sed 's/\.[^\.]\+$//'`
$base_path/../../../build/wb3 $base_path/$script_name.wb3
printf "Exit code: %d.\n" $?
