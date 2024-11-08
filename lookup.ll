target triple = "x86_64-unknown-linux-gnu"

; https://llvm.org/docs/LangRef.html#t-vector
declare void @printf(ptr, ...)

@zeros = private unnamed_addr constant <16 x i8> <i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0 >

; First byte high nibble
@table_1 = private unnamed_addr constant <16 x i8> <
    i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1,
    i8 128, i8 128, i8 128, i8 128, i8 6, i8 2, i8 26, i8 98
>
; First byte low nibble
@table_2 = private unnamed_addr constant <16 x i8> <
    i8 183, i8 135, i8 131, i8 131, i8 195, i8 227, i8 227, i8 227,
    i8 227, i8 227, i8 227, i8 227, i8 227, i8 235, i8 227, i8 227
>
; Second byte high nibble
@table_3 = private unnamed_addr constant <16 x i8> <
    i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2,
    i8 181, i8 213, i8 205, i8 205, i8 2, i8 2, i8 2, i8 2
>

@debug.num = private unnamed_addr constant [4 x i8] c"%d \00"
@debug.nl = private unnamed_addr constant [2 x i8] c"\0a\00"

define <16 x i8> @load_vec (ptr %bytes) {
    %1 = load i128, i128* %bytes
    %2 = bitcast i28 %1 to <16 x i8>
    ret %2
}

define i1 @lookup_validate (ptr %bytes, i64 %len) {
  %vec = load <16 x i8>, <16 x i8>* @zeros
  call void @debug_vec(<16 x i8> %vec)
  ret i1 false
}

define void @debug_vec (<16 x i8> %vec) {
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
