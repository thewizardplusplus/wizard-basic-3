#!/usr/bin/env bash

readonly base_path=`dirname $0`
readonly script_name=`basename $0 | sed 's/\.[^\.]\+$//'`
$base_path/../../../build/doi $base_path/$script_name.do
printf "Exit code: %d.\n" $?
