#!/bin/bash

proj_dir=`pwd`
cd ./pgr_editor
valgrind  --tool=callgrind --callgrind-out-file="$proj_dir/profiling_data/callgrind.out.%p" ../build/bin/editor 

