node := /Users/tim/Code/node
c := clang
cflags := -g
cxx := clang++
cxxflags := -stdlib=libc++ -std=gnu++11 -g

all: demo embed node addon.node

#-L$(node)/out/Release
#install_name_tool -change @rpath/libnode.51.dylib ./libnode.51.dylib embed
# -I$(node)/target/include/node \


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

libembed.o: embed_node.cc
	$(cxx) $(cxxflags) \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 -shared \
		embed_node.cc -o libembed.o

demo: libembed.o demo.c
	clang -g libembed.o demo.c -o demo

