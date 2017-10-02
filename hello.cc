// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "v8.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);                                 \
void operator=(const TypeName&)
#include <node_platform.h>



using namespace v8;

int main(int argc, char* argv[]) {
    // Initialize V8.
    V8::InitializeICUDefaultLocation(argv[0]);
    V8::InitializeExternalStartupData(argv[0]);

    // make a platform--use Node,
    // because libnode doesn't export the
    // symbol: v8::platform::CreateDefaultPlatform
    int v8_thread_pool_size = 1;
    argv = uv_setup_args(argc, argv);
    uv_loop_t* loop = uv_default_loop();
    static node::NodePlatform* platform_ = new node::NodePlatform(v8_thread_pool_size, loop, nullptr);

    V8::InitializePlatform(platform_);
    V8::Initialize();

    // Create a new Isolate and make it the current one.
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
            v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate* isolate = Isolate::New(create_params);
    {
        Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        // Create a new context.
        Local<Context> context = Context::New(isolate);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        Local<String> source =
                String::NewFromUtf8(isolate, "'Hello' + ', World!'",
                                                        NewStringType::kNormal).ToLocalChecked();

        // Compile the source code.
        Local<Script> script = Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        Local<Value> result = script->Run(context).ToLocalChecked();

        // Convert the result to an UTF8 string and print it.
        String::Utf8Value utf8(result);
        printf("%s\n", *utf8);
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    // TODO delete NodePlatform?
    delete create_params.array_buffer_allocator;
    return 0;
}
