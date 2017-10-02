node := /Users/tim/Code/node
cxx := clang++
cxxflags := -stdlib=libc++ -std=gnu++11

all: embed node addon.node

#-L$(node)/out/Release
embed: embed.cc
	$(cxx) $(cxxflags) \
		-I$(node)/target/include/node \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		embed.cc -o embed
	#install_name_tool -change @rpath/libnode.51.dylib ./libnode.51.dylib embed

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

