// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "include/libplatform/libplatform.h"
#include "include/v8-context.h"
#include "include/v8-exception.h"
#include "include/v8-function.h"
#include "include/v8-initialization.h"
#include "include/v8-isolate.h"
#include "include/v8-local-handle.h"
#include "include/v8-primitive.h"
#include "include/v8-script.h"

std::string FormatValue(v8::Isolate* isolate,
                        v8::Local<v8::Value> val) {
  if (val.IsEmpty()) {
    return "\"\"";
  } else {
    v8::String::Utf8Value val_utf8(isolate, val);
    if (*val_utf8 == nullptr)
      return std::string();
    return std::string(*val_utf8, val_utf8.length());
  }
}

std::string FormatExceptionMessage(
    v8::Local<v8::Context> context,
    v8::Local<v8::Message> message) {
  if (message.IsEmpty()) {
    return "Unknown exception.";
  } else {
    std::stringstream ss;
    v8::Isolate* isolate = message->GetIsolate();
    int line_num;
    ss << FormatValue(isolate, message->GetScriptResourceName());
    if (!context.IsEmpty() && message->GetLineNumber(context).To(&line_num)) {
      ss << ":" << line_num;
    }
    ss << " ";
    ss << FormatValue(isolate, message->Get()) << ".";
    return ss.str();
  }
}

v8::Local<v8::Value> CompileAndRunChecked(v8::Isolate* isolate,
                                          v8::Local<v8::Context> context,
                                          std::string script_str) {
  v8::Context::Scope context_scope(context);
  v8::TryCatch tc(isolate);
  v8::Local<v8::String> v8_script_str;
  if (!v8::String::NewFromUtf8(isolate, script_str.data(),
                               v8::NewStringType::kNormal,
                               script_str.length()).ToLocal(&v8_script_str)) {
    std::cerr << "Could not convert `" << script_str << "` to v8::String";
    if (tc.HasCaught()) {
      std::cerr << FormatExceptionMessage(context, tc.Message()) << std::endl;
    }
    abort();
  }
  v8::Local<v8::Script> script;
  if (!v8::Script::Compile(context, v8_script_str).ToLocal(&script)) {
    std::cerr << "Could not compile `" << script_str << "`";
    if (tc.HasCaught()) {
      std::cerr << FormatExceptionMessage(context, tc.Message()) << std::endl;
    }
    abort();
  }
  v8::Local<v8::Value> result;
  if (!script->Run(context).ToLocal(&result)) {
    std::cerr << "Could not run `" << script_str << "`";
    if (tc.HasCaught()) {
      std::cerr << FormatExceptionMessage(context, tc.Message()) << std::endl;
    }
    abort();
  }
  return result;
}

void AddBindingsToContext(v8::Isolate* isolate, v8::Local<v8::Context> context) {
  // TODO: Have fake bindings check args and log errors.
  CompileAndRunChecked(isolate, context,
  R"(
    globalThis.forDebuggingOnly = {
      reportAdAuctionLoss: function(url) {},
      reportAdAuctionWin: function(url) {},
    };
    globalThis.privateAggregation = {
      sendHistogramReport: function(obj) {},
      reportContributionForEvent: function(event_type, obj) {},
      enableDebugMode: function(id = null) {},
    };
    globalThis.registerAdBeacon = function(obj) {};
    globalThis.sendReportTo = function(url) {};
    globalThis.setBid = function(obj) {};
    globalThis.setPriority = function(number) {};
    globalThis.setPrioritySignalsOverride = function(key, number) {};
    globalThis.sharedStorage = {
      set: function(key, value, options = null){},
      append: function(key, value){},
      delete: function(key){},
      clear: function(){},
    };
  )");

}

std::vector<v8::Local<v8::Value> > BuildFakeGenerateBidArgs(v8::Isolate* isolate,
                              v8::Local<v8::Context> context) {
  std::vector<v8::Local<v8::Value> > result;

  // TODO: Fill in missing subcomponents of args.

  // interestGroup
  result.push_back(CompileAndRunChecked(isolate, context,
    R"(
      const interestGroup = {
         owner: "https://foo.com",
         name: "bar",
         useBiddingSignalsPrioritization: false,
         biddingLogicUrl: "https://foo.com/bid.js",
         ads: [{ renderUrl: "https://foo.com", metadata: ""}],
      };
      interestGroup
    )"
  ));

  // auctionSignals
  result.push_back(CompileAndRunChecked(isolate, context,
    R"(
      const auctionSignals = {};
      auctionSignals
    )"));

  // perBuyerSignals
  result.push_back(CompileAndRunChecked(isolate, context,
    R"(
      const perBuyerSignals = {};
      perBuyerSignals
    )"));

  // trustedSignals
  result.push_back(CompileAndRunChecked(isolate, context,
    R"(
      const trustedSignals = {};
      trustedSignals
    )"));

  // browserSignals
  result.push_back(CompileAndRunChecked(isolate, context,
    R"(
      const browserSignals = {
        topWindowHostname: "https://bar.com",
        seller: "https://bar.com",
        joinCount: 5,
        bidCount: 10,
      };
      browserSignals
    )"));

  // directFromSellerSignals (optional)
  //result.push_back(isolate, context, CompileAndRunChecked("{}"));
  return result;
}

void CheckGenerateBidOutput(v8::Local<v8::Value> generate_bid_result) {
  if (generate_bid_result->IsNullOrUndefined()) {
    std::cout << "The bid is undefined" << std::endl;
    return;
  }
  if (!generate_bid_result->IsObject()) {
    std::cout << "The bid is not an object" << std::endl;
    return;
  }
  // TODO: Check that the output is reasonable.
}

void RunScriptInContext(v8::Isolate* isolate, v8::Local<v8::Context> context,
                        std::string script_path, const std::string& script_str,
                        bool freeze_context) {
  // Enter the context for compiling and running the script.
  v8::Context::Scope context_scope(context);

  v8::TryCatch try_catch(isolate);

  // Create a string containing the JavaScript source code.
  v8::Local<v8::String> script_v8_str;
  if (!v8::String::NewFromUtf8(isolate, script_str.data(),
                              v8::NewStringType::kNormal,
                              script_str.length()).ToLocal(&script_v8_str)) {
    std::cout << "Could not create script as UTF8 string" << std::endl;
    return;
  }

  v8::Local<v8::String> script_source;
  if (!v8::String::NewFromUtf8(isolate, script_path.data(),
                               v8::NewStringType::kNormal,
                               script_path.length()).ToLocal(&script_source)) {
    std::cout << "Could not create script path as UTF8 string" << std::endl;
    return;
  }

  // Compile the source code.
  v8::ScriptCompiler::Source source(
    script_v8_str,
    v8::ScriptOrigin(isolate, script_source));

  v8::Local<v8::Script> script;
  if (!v8::ScriptCompiler::Compile(context, &source).ToLocal(&script)) {
    std::cout << "Could not compile script" << std::endl;
    return;
  }
  if (try_catch.HasCaught()) {
    std::cout << FormatExceptionMessage(context, try_catch.Message()) << std::endl;
    return;
  }

  // Run the script to get the result.
  v8::MaybeLocal<v8::Value> result = script->Run(context);
  if (try_catch.HasCaught()) {
    std::cout << FormatExceptionMessage(context, try_catch.Message()) << std::endl;
    return;
  }
  if (result.IsEmpty()) {
    std::cout << "Unknown error while running the script" << std::endl;
    return;
  }

  AddBindingsToContext(isolate, context);
  if (try_catch.HasCaught()) {
    std::cout << FormatExceptionMessage(context, try_catch.Message()) << std::endl;
    return;
  }

  if (freeze_context) {
    bool success = context->DeepFreeze().IsJust();
    if (try_catch.HasCaught()) {
      std::cout << FormatExceptionMessage(context, try_catch.Message()) << std::endl;
      return;
    }
    if (!success) {
      std::cout << "Unknown error freezing the context" << std::endl;
      return;
    }
  }

  v8::Local<v8::String> v8_function_name =
      v8::String::NewFromUtf8Literal(isolate, "generateBid");

  v8::Local<v8::Value> function_value;
  if (!context->Global()->Get(context, v8_function_name).ToLocal(&function_value)) {
    std::cout << "function `generateBid` not found." << std::endl;
    return;
  }

  if (!function_value->IsFunction()) {
    std::cout << "function `generateBid` is not a function." << std::endl;
    return;
  }

  v8::Local<v8::Function> function =
      v8::Local<v8::Function>::Cast(function_value);

  std::vector<v8::Local<v8::Value>> args = BuildFakeGenerateBidArgs(isolate, context);
  result = function->Call(context, context->Global(), args.size(), args.data());
  if (try_catch.HasCaught()) {
    std::cout << FormatExceptionMessage(context, try_catch.Message()) << std::endl;
    return;
  }
  if (result.IsEmpty()) {
    std::cout << "Unknown error while calling `generateBid`" << std::endl;
    return;
  }
  CheckGenerateBidOutput(result.ToLocalChecked());
}

void ConsoleLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
  for (int idx = 0; idx < args.Length(); idx++) {
    std::cout << FormatValue(args.GetIsolate(), args[idx]) << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Initialize V8.
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  if (argc < 2 || argc > 3) {
    printf("Usage: %s <path_to_script> [true|false]\n", argv[0]);
    printf("  where <path_to_script> is the path to your script\n");
    printf("  and [true|false] indicates whether to try to freeze the context.\n");
    return 0;
  }

  bool freeze_context = false;
  if (argc >= 3 && std::string(argv[2]) == "true") {
    freeze_context = true;
  }


  // load the script file.
  std::string script_str;
  {
    std::ifstream script_file(argv[1], std::ios::in);
    if (!script_file.is_open()) {
      std::cout << "Could not open " << argv[1] << std::endl;
      return 0;
    }
    std::stringstream buf;
    buf << script_file.rdbuf();
    script_str = std::move(buf).str();
  }

  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);

    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    {
      // Enter the context for compiling and running the hello world script.
      v8::Context::Scope context_scope(context);
      auto result =
        context->Global()->Delete(context, v8::String::NewFromUtf8Literal(isolate, "Date"));
      if (result.IsNothing()) {
        std::cerr << "Could not remove `Date` from context" << std::endl;
        return 0;
      }

      // Overwrite console.log().
      v8::Local<v8::Function> v8_function = v8::Function::New(context, &ConsoleLog).ToLocalChecked();
      v8::Local<v8::Value> console = context->Global()->Get(context, v8::String::NewFromUtf8Literal(isolate, "console")).ToLocalChecked();
      v8::Local<v8::Object>::Cast(console)->Set(context, v8::String::NewFromUtf8Literal(isolate, "log"), v8_function).Check();
    }

    // Run the script.
    RunScriptInContext(isolate, context, argv[1], script_str, freeze_context);
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}
