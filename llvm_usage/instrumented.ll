; ModuleID = 'temp_target.ll'
source_filename = "target_program.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [18 x i8] c"  Inside bar(%d)\0A\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"  Inside foo()\0A\00", align 1
@.str.2 = private unnamed_addr constant [17 x i8] c"Starting main()\0A\00", align 1
@.str.3 = private unnamed_addr constant [18 x i8] c"Finishing main()\0A\00", align 1
@0 = private unnamed_addr constant [4 x i8] c"bar\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"foo\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"main\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @bar(i32 %0) #0 {
  call void @log_function_entry(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0))
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str, i64 0, i64 0), i32 %3)
  ret void
}

declare dso_local i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @foo() #0 {
  call void @log_function_entry(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1, i64 0, i64 0))
  call void @bar(i32 10)
  call void @bar(i32 20)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  call void @log_function_entry(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @2, i32 0, i32 0))
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.2, i64 0, i64 0))
  call void @foo()
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str.3, i64 0, i64 0))
  ret i32 0
}

declare void @log_function_entry(i8*)

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
