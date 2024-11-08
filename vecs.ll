; https://llvm.org/docs/LangRef.html#t-vector
declare void @printf(ptr, ...)

@debug.num = private unnamed_addr constant [4 x i8] c"%d \00"
@debug.nl = private unnamed_addr constant [2 x i8] c"\0a\00"

define <16 x i8> @vec_shift_into (<16 x i8> %prev, <16 x i8> %input) {
    %palignr_ed = shufflevector <16 x i8> %prev, <16 x i8> %input, <16 x i32> <i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30>
    ret <16 x i8> %palignr_ed
}

define <16 x i8> @vec_shr (<16 x i8> %v, <16 x i8> %n) {
    %shifted = lshr <16 x i8> %v, %n
    ret <16 x i8> %shifted
}

define <16 x i8> @vec_bitand (<16 x i8> %v, <16 x i8> %n) {
    %masked = and <16 x i8> %v, %n
    ret <16 x i8> %masked
}

declare i8 @llvm.vector.reduce.or.v4i8(<16 x i8> %vec)
define i8 @vec_reduce_or  (<16 x i8> %v) {
    %reduced = call i8 @llvm.vector.reduce.or.v4i8(<16 x i8> %v)
    ret i8 %reduced
}


define void @vec_debug (<16 x i8> %vec) {
    %1 = extractelement <16 x i8> %vec, i32 0
    call void (ptr, ...) @printf(ptr @debug.num, i8 %1)
    %2 = extractelement <16 x i8> %vec, i32 1
    call void (ptr, ...) @printf(ptr @debug.num, i8 %2)
    %3 = extractelement <16 x i8> %vec, i32 2
    call void (ptr, ...) @printf(ptr @debug.num, i8 %3)
    %4 = extractelement <16 x i8> %vec, i32 3
    call void (ptr, ...) @printf(ptr @debug.num, i8 %4)
    %5 = extractelement <16 x i8> %vec, i32 4
    call void (ptr, ...) @printf(ptr @debug.num, i8 %5)
    %6 = extractelement <16 x i8> %vec, i32 5
    call void (ptr, ...) @printf(ptr @debug.num, i8 %6)
    %7 = extractelement <16 x i8> %vec, i32 6
    call void (ptr, ...) @printf(ptr @debug.num, i8 %7)
    %8 = extractelement <16 x i8> %vec, i32 7
    call void (ptr, ...) @printf(ptr @debug.num, i8 %8)
    %9 = extractelement <16 x i8> %vec, i32 8
    call void (ptr, ...) @printf(ptr @debug.num, i8 %9)
    %10 = extractelement <16 x i8> %vec, i32 9
    call void (ptr, ...) @printf(ptr @debug.num, i8 %10)
    %11 = extractelement <16 x i8> %vec, i32 10
    call void (ptr, ...) @printf(ptr @debug.num, i8 %11)
    %12 = extractelement <16 x i8> %vec, i32 11
    call void (ptr, ...) @printf(ptr @debug.num, i8 %12)
    %13 = extractelement <16 x i8> %vec, i32 12
    call void (ptr, ...) @printf(ptr @debug.num, i8 %13)
    %14 = extractelement <16 x i8> %vec, i32 13
    call void (ptr, ...) @printf(ptr @debug.num, i8 %14)
    %15 = extractelement <16 x i8> %vec, i32 14
    call void (ptr, ...) @printf(ptr @debug.num, i8 %15)
    %16 = extractelement <16 x i8> %vec, i32 15
    call void (ptr, ...) @printf(ptr @debug.num, i8 %16)
    call void (ptr, ...) @printf(ptr @debug.nl)
    ret void
}
