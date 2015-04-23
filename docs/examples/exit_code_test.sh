#!/usr/bin/env bash

readonly base_path=`dirname $0`

script_name=`basename $0`
script_name=`echo "$script_name" | sed 's/\.[^\.]\+$//'`

$base_path/../../build/wb3i $base_path/$script_name.do
printf "Exit code: %d.\n" $?
