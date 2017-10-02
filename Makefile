node := /Users/tim/Code/node
cxx := clang++
cxxflags := -stdlib=libc++ -std=gnu++11

#-L$(node)/out/Release
embed: embed.cpp
	$(cxx) $(cxxflags) \
		-I$(node)/target/include/node \
		-I$(node)/deps/v8/include \
		-I$(node)/deps/uv/include \
		-I$(node)/src \
		-L. \
		-lnode.57 \
		embed.cpp -o embed
	#install_name_tool -change @rpath/libnode.51.dylib ./libnode.51.dylib embed

