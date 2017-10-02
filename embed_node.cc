// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "v8.h"

#include <node.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)
#include <node_platform.h>

#include "embed_node.h"

using namespace v8;

extern "C" {

int exec_argc;
const char** exec_argv;
uv_loop_t* loop;
static node::NodePlatform* platform_;
Isolate* isolate;
Isolate::CreateParams create_params;


int node_init(int argc, char* argv[]) {
    // init UV
    argv = uv_setup_args(argc, argv);
    loop = uv_default_loop();

    // This needs to run *before* V8::Initialize().
    node::Init(&argc, const_cast<const char**>(argv), &(exec_argc), &(exec_argv));

    // Initialize V8.
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);

    // make a platform--use Node,
    // because libnode doesn't export the
    // symbol: platform::CreateDefaultPlatform
    int v8_thread_pool_size = 1;
    platform_ = new node::NodePlatform(v8_thread_pool_size, loop, nullptr);

    V8::InitializePlatform(platform_);
    V8::Initialize();

    // Create a new Isolate and make it the current one.
    create_params.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = Isolate::New(create_params);

    return 0;
}

NodeString node_string(char* utf8String) {
    printf("can we make a string?\n");
    return (NodeString)String::NewFromUtf8(isolate, utf8String);
}

NodeObjectTemplate* node_object_template() {
    printf("can we make a template?\n");
    return ObjectTemplate::New(isolate);
    //return (NodeObjectTemplate)ObjectTemplate::New(isolate);
}

void node_object_template_set(NodeObjectTemplate obj, NodeString key, void (*callback)(const NodeCallbackInfo& args)) {
    obj->Set(key, FunctionTemplate::New(isolate, callback));
}

int node_run(int argc, char* argv[], NodeObjectTemplate* (*makeGlobal)()) {
    Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    //NodeObjectTemplate global = makeGlobal();
    Local<ObjectTemplate> global = makeGlobal();
    
    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    int exit_code;
    {
        node::IsolateData* isolate_data = node::CreateIsolateData(isolate, loop);
        node::Environment *env = node::CreateEnvironment(isolate_data, context, argc, argv, exec_argc, exec_argv);
        node::LoadEnvironment(env);
         
        bool more;
        do {
            more = uv_run(loop, UV_RUN_ONCE);
            if (more == false) {
                node::EmitBeforeExit(env);
         
                //plat.DrainVMTasks();
                
                // Emit `beforeExit` if the loop became alive either after emitting
                // event, or after running some callbacks.
                more = uv_loop_alive(loop);

                if (uv_run(loop, UV_RUN_NOWAIT) != 0)
                    more = true;
            }
        } while (more == true);
        
        exit_code = node::EmitExit(env);
        node::RunAtExit(env);
        
        node::FreeEnvironment(env);
        node::FreeIsolateData(isolate_data);
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    platform_->Shutdown(); // can't hurt
    delete create_params.array_buffer_allocator;
    return exit_code;
}

} // extern "C"

