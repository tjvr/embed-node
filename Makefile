node := /Users/tim/Code/node
cxx := clang++
cxxflags := -stdlib=libc++ -std=gnu++11 -g

all: embed node addon.node hello

#-L$(node)/out/Release
#install_name_tool -change @rpath/libnode.51.dylib ./libnode.51.dylib embed

embed: embed.cc
	$(cxx) $(cxxflags) \
		-I$(node)/target/include/node \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		embed.cc -o embed

node: node.cc
	$(cxx) $(cxxflags) \
		-I$(node)/target/include/node \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		node.cc -o node

addon.node: addon.cc
	$(cxx) $(cxxflags) \
		-I$(node)/target/include/node \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		-shared \
		addon.cc -o addon.node

hello: hello.cc
	$(cxx) $(cxxflags) \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		hello.cc -o hello
