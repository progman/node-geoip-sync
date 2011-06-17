#!/bin/bash

cd ..;
rm -rf build &> /dev/null;

node-waf configure build;

export NODE_PATH="${NODE_PATH}:./build/default/";
node example/test.js;
