#!/bin/bash

if command -v wget > /dev/null; then
    cmd="wget -O"
elif command -v curl > /dev/null; then
    cmd="curl -o"
else
    echo "Both \033[4mwget\033[0m and \033[4mcurl\033[0m are not available, impossible to update."
    exit 1
fi

repo="https://raw.githubusercontent.com/fcpp/fcpp/master"

$cmd .gitignore $repo/.gitignore
for file in plot.asy plot_builder.py; do
    $cmd plotter/$file $repo/src/extras/plotter/$file
done
for file in .bazelrc gtest.BUILD make.sh Dockerfile Vagrantfile; do
    $cmd $file $repo/src/$file
done
chmod a+x make.sh
