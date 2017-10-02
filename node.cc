#include <node.h>

int main(int argc, char *argv[]) {
    int exec_argc;
    const char** exec_argv;
    node::Init(&argc, const_cast<const char**>(argv), &exec_argc, &exec_argv);

    return node::Start(argc, argv);
}

