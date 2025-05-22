// test_llvm_instrument.cpp
#include <string>
#include <vector>
#include <iostream>
#include <fstream> // For std::ofstream
#include <memory>  // For std::unique_ptr
#include <system_error> // For std::error_code

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IRReader/IRReader.h"   // For parseIRFile
#include "llvm/Support/SourceMgr.h" // For SMDiagnostic
#include "llvm/Support/raw_ostream.h" // For raw_fd_ostream
#include "llvm/Support/FileSystem.h"  // For sys::fs::OF_None

using namespace llvm;
using namespace std;

bool instrumentFunction(Function &F) {
    StringRef funcName = F.getName();
    if (F.isDeclaration() || funcName.empty() || funcName == "log_function_entry") {
        return false;
    }

    LLVMContext &Context = F.getContext();
    Module *M = F.getParent();

    FunctionType *logFuncType = FunctionType::get(
        Type::getVoidTy(Context),
        {Type::getInt8PtrTy(Context)},
        false);
    FunctionCallee logFunc = M->getOrInsertFunction("log_function_entry", logFuncType);
    if (!logFunc) {
        errs() << "Could not declare log_function_entry in module for function: " << funcName << "\n";
        return false;
    }
    BasicBlock &entryBlock = F.getEntryBlock();
    if (entryBlock.empty()) { 
        // 或者，如果允许，可以创建一个新的基本块并将其设为入口
        // 但对于已编译的C代码，函数总是有入口块的第一个指令
        // 如果没有指令，IRBuilder会插入到块的末尾（此时也是开头）
         IRBuilder<> builder(&entryBlock); // 插入到块的末尾（即开头）
         Value *funcNameGlobalStr = builder.CreateGlobalStringPtr(funcName);
         builder.CreateCall(logFunc, {funcNameGlobalStr});
    } else {
        IRBuilder<> builder(&entryBlock, entryBlock.getFirstInsertionPt());
        Value *funcNameGlobalStr = builder.CreateGlobalStringPtr(funcName);
        builder.CreateCall(logFunc, {funcNameGlobalStr});
    }
    errs() << "Instrumented function (in-tool): " << funcName << "\n";
    return true;
}
int main(int argc, char** argv){
    if(argc != 3){
        errs() << "Usage: " << argv[0] << " <input_c_file> <output_ll_file>\n";
        return 1;
    }
    string cInputFile = argv[1];
    string llOutputFile = argv[2];
    string tempLLFile = "temp_target.ll"; // 临时LLVM IR文件

    string compileCommand = "clang-10 -S -emit-llvm " + cInputFile + " -o " + tempLLFile + " -Wno-deprecated";
    errs() << "Executing: " << compileCommand << "\n";
    int compileResult = system(compileCommand.c_str());
    if (compileResult != 0) {
        errs() << "Error compiling C file to LLVM IR. Exit code: " << compileResult << "\n";
        return 1;
    }
    // 步骤 2: 解析临时 .ll 文件
    LLVMContext context;
    SMDiagnostic errorDiagnostic;
    std::unique_ptr<Module> M = parseIRFile(tempLLFile, errorDiagnostic, context);
    
    if (!M) {
        errorDiagnostic.print(argv[0], errs());
        errs() << "Error parsing LLVM IR file: " << tempLLFile << "\n";
        remove(tempLLFile.c_str()); // 清理临时文件
        return 1;
    }

    // 步骤 3: 对模块中的每个函数进行插桩
    bool modified = false;
    for (Function &F : *M){
        if (instrumentFunction(F)){
            modified = true;
        }
    }
    // 步骤 4: 如果模块被修改了，则写出到输出文件
    if (modified) {
        std::error_code EC;
        raw_fd_ostream OS(llOutputFile, EC, sys::fs::OF_None); // OF_None for binary, OF_Text explicitly for text .ll
        // raw_fd_ostream OS(llOutputFile, EC); // simpler alternative

        if (EC) {
            errs() << "Error opening output file '" << llOutputFile << "': " << EC.message() << "\n";
            remove(tempLLFile.c_str()); // 清理临时文件
            return 1;
        }

        M->print(OS, nullptr); // 将模块以人类可读的格式打印到输出流
        // 如果想输出bitcode (.bc) 文件:
        // WriteBitcodeToFile(*M, OS);
        OS.close(); // 确保所有内容都已写入
        if (OS.has_error()) {
             errs() << "Error writing to output file '" << llOutputFile << "'\n";
        }

        errs() << "Successfully instrumented IR written to: " << llOutputFile << "\n";
    } else {
        errs() << "No functions were instrumented or module was not modified.\n";
        // 如果没有修改，可以选择复制临时文件到输出文件，或不生成输出
        // 为简单起见，这里即使没有修改也可能因为前面的system调用创建了tempLLFile
        // 最好是如果没有修改，就不要创建 output llOutputFile，或者复制tempLLFile
        // 但由于我们打印了instrumented function的日志，所以modified通常为true
    }

    // 步骤 5: 清理临时文件
    remove(tempLLFile.c_str());

    return 0;
}