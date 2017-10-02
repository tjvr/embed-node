#include <node.h>
#include "uv.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);                 \
void operator=(const TypeName&)
#include <node_platform.h>


using v8::V8;
using v8::Local;
using v8::Isolate;
using v8::Context;
using v8::HandleScope;
using node::NodePlatform;
using node::IsolateData;
using node::Environment;



int main(int argc, char *argv[]) {

    argv = uv_setup_args(argc, argv);
    uv_loop_t* loop = uv_default_loop();

    int exec_argc;
    const char** exec_argv;
    node::Init(&argc, const_cast<const char**>(argv), &exec_argc, &exec_argv);

    int v8_thread_pool_size = 1;
    static node::NodePlatform* platform_;
    platform_ = new node::NodePlatform(v8_thread_pool_size, loop, nullptr);
    V8::InitializePlatform(platform_);
    V8::Initialize();



    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    Isolate::CreateParams params;
    // TODO consider using node::ArrayBufferAllocator ?
    params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate* const isolate = Isolate::New(params);
    if (isolate == nullptr)
        return 12;  // Signal internal error.
    
    v8::Locker locker(isolate);

    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    node::IsolateData* isolate_data = node::CreateIsolateData(isolate, loop);

    Local<Context> context = Context::New(isolate);
    assert(context->GetIsolate() != nullptr);

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



    node::Environment *env = node::CreateEnvironment(isolate_data, context, argc, argv, exec_argc, exec_argv);
    assert(env != nullptr);

    node::LoadEnvironment(env);
    

    //return node::Start(argc, argv);
}

