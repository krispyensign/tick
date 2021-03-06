#!/bin/bash
rm -fr third_party/
mkdir -p third_party/
cd third_party/
svn export https://github.com/Tencent/rapidjson/trunk/include/rapidjson/
wget https://raw.githubusercontent.com/bombela/backward-cpp/master/backward.hpp -O backward.hpp
svn export https://github.com/gabime/spdlog/trunk/include/spdlog/
wget https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq.hpp -O zmq.hpp
svn export https://github.com/msgpack/msgpack-c/branches/cpp_master/include/msgpack/
wget https://raw.githubusercontent.com/msgpack/msgpack-c/cpp_master/include/msgpack.hpp -O msgpack.hpp
svn export https://github.com/fmtlib/fmt/trunk/include/fmt/
svn export https://github.com/ericniebler/range-v3/trunk/include/concepts/
svn export https://github.com/ericniebler/range-v3/trunk/include/meta/
svn export https://github.com/ericniebler/range-v3/trunk/include/range/
svn export https://github.com/ericniebler/range-v3/trunk/include/std/
wget https://raw.githubusercontent.com/Taywee/args/master/args.hxx -O args.hxx
