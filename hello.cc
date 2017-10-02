// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "v8.h"

#include <node.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);                                 \
void operator=(const TypeName&)
#include <node_platform.h>



using namespace v8;

int main(int argc, char* argv[]) {

    // init UV
    argv = uv_setup_args(argc, argv);
    uv_loop_t* loop = uv_default_loop();

    // This needs to run *before* V8::Initialize().
    int exec_argc;
    const char** exec_argv;
    node::Init(&argc, const_cast<const char**>(argv), &exec_argc, &exec_argv);

    // Initialize V8.
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);

    // make a platform--use Node,
    // because libnode doesn't export the
    // symbol: v8::platform::CreateDefaultPlatform
    int v8_thread_pool_size = 1;
    static node::NodePlatform* platform_ = new node::NodePlatform(v8_thread_pool_size, loop, nullptr);

    V8::InitializePlatform(platform_);
    V8::Initialize();

    // Create a new Isolate and make it the current one.
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate* isolate = Isolate::New(create_params);

    int exit_code;
    {
        Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        // Create a new context.
        Local<Context> context = Context::New(isolate);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

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
            node::FreeIsolateData(isolate_data); // ???
        }
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    // TODO delete NodePlatform?
    delete create_params.array_buffer_allocator;
    return 0;
}
