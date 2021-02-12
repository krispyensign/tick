#!/bin/bash -ex
transform_file=$1
transform_target=$(echo $1 | sed 's/cts/cxx/g')
sed -f transform.sed $transform_file > $transform_target
shift
echo $@
/usr/bin/g++ $@
