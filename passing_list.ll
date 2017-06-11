; ModuleID = 'func'
source_filename = "<string>"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@numba.dynamic.globals.7f54ba9e4f50 = linkonce local_unnamed_addr global i8* inttoptr (i64 140001884917584 to i8*)
@".const.%s" = internal constant [3 x i8] c"%s\00"
@".const.the print() function" = internal constant [21 x i8] c"the print() function\00"
@".const.\0A" = internal constant [2 x i8] c"\0A\00"
@.const.func = internal constant [5 x i8] c"func\00"
@".const.Fatal error: missing _dynfunc.Closure" = internal constant [38 x i8] c"Fatal error: missing _dynfunc.Closure\00"
@PyExc_RuntimeError = external global i8
@".const.missing Environment" = internal constant [20 x i8] c"missing Environment\00"
@_Py_NoneStruct = external global i8

define i32 @"_ZN8__main__8func$241E"(i8** noalias nocapture %retptr, { i8*, i32 }** noalias nocapture readnone %excinfo, i8* noalias nocapture readnone %env) local_unnamed_addr {
entry:
  %.53 = alloca i32, align 4
  %.231 = load i32 (i32, i32)*, i32 (i32, i32)** bitcast (i8** @numba.dynamic.globals.7f54ba9e4f50 to i32 (i32, i32)**), align 8
  %.33 = tail call i32 %.231(i32 1120, i32 27)
  call void @numba_gil_ensure(i32* nonnull %.53)
  %.55 = sext i32 %.33 to i64
  %.57 = call i8* @PyLong_FromLongLong(i64 %.55)
  %.60 = icmp eq i8* %.57, null
  br i1 %.60, label %B0.else, label %B0.if, !prof !0

B0.if:                                            ; preds = %entry
  %.62 = call i8* @PyObject_Str(i8* nonnull %.57)
  %.63 = call i8* @PyUnicode_AsUTF8(i8* %.62)
  call void (i8*, ...) @PySys_WriteStdout(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @".const.%s", i64 0, i64 0), i8* %.63)
  call void @Py_DecRef(i8* %.62)
  call void @Py_DecRef(i8* nonnull %.57)
  br label %B0.endif

B0.else:                                          ; preds = %entry
  %.68 = call i8* @PyUnicode_FromString(i8* getelementptr inbounds ([21 x i8], [21 x i8]* @".const.the print() function", i64 0, i64 0))
  call void @PyErr_WriteUnraisable(i8* %.68)
  call void @Py_DecRef(i8* %.68)
  br label %B0.endif

B0.endif:                                         ; preds = %B0.else, %B0.if
  call void (i8*, ...) @PySys_WriteStdout(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @".const.\0A", i64 0, i64 0))
  call void @numba_gil_release(i32* nonnull %.53)
  store i8* null, i8** %retptr, align 8
  ret i32 0
}

declare void @numba_gil_ensure(i32*) local_unnamed_addr

declare i8* @PyLong_FromLongLong(i64) local_unnamed_addr

declare i8* @PyObject_Str(i8*) local_unnamed_addr

declare i8* @PyUnicode_AsUTF8(i8*) local_unnamed_addr

declare void @PySys_WriteStdout(i8*, ...) local_unnamed_addr

declare void @Py_DecRef(i8*) local_unnamed_addr

declare i8* @PyUnicode_FromString(i8*) local_unnamed_addr

declare void @PyErr_WriteUnraisable(i8*) local_unnamed_addr

declare void @numba_gil_release(i32*) local_unnamed_addr

define i8* @"_ZN7cpython8__main__8func$241E"(i8* %py_closure, i8* %py_args, i8* nocapture readnone %py_kws) local_unnamed_addr {
entry:
  %.5 = tail call i32 (i8*, i8*, i64, i64, ...) @PyArg_UnpackTuple(i8* %py_args, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.const.func, i64 0, i64 0), i64 0, i64 0)
  %.6 = icmp eq i32 %.5, 0
  %.28 = alloca i8*, align 8
  br i1 %.6, label %entry.if, label %entry.endif, !prof !0

entry.if:                                         ; preds = %entry
  ret i8* null

entry.endif:                                      ; preds = %entry
  %.10 = icmp eq i8* %py_closure, null
  br i1 %.10, label %entry.endif.if, label %entry.endif.endif, !prof !0

entry.endif.if:                                   ; preds = %entry.endif
  %.12 = tail call i32 @puts(i8* getelementptr inbounds ([38 x i8], [38 x i8]* @".const.Fatal error: missing _dynfunc.Closure", i64 0, i64 0))
  unreachable

entry.endif.endif:                                ; preds = %entry.endif
  %.14 = ptrtoint i8* %py_closure to i64
  %.15 = add i64 %.14, 24
  %.17 = inttoptr i64 %.15 to { i8* }*
  %.181 = bitcast { i8* }* %.17 to i8**
  %.19 = load i8*, i8** %.181, align 8
  %.24 = icmp eq i8* %.19, null
  br i1 %.24, label %entry.endif.endif.if, label %entry.endif.endif.endif, !prof !0

entry.endif.endif.if:                             ; preds = %entry.endif.endif
  tail call void @PyErr_SetString(i8* nonnull @PyExc_RuntimeError, i8* getelementptr inbounds ([20 x i8], [20 x i8]* @".const.missing Environment", i64 0, i64 0))
  ret i8* null

entry.endif.endif.endif:                          ; preds = %entry.endif.endif
  store i8* null, i8** %.28, align 8
  %.30 = call i32 @"_ZN8__main__8func$241E"(i8** nonnull %.28, { i8*, i32 }** undef, i8* undef)
  tail call void @Py_IncRef(i8* nonnull @_Py_NoneStruct)
  ret i8* @_Py_NoneStruct
}

declare i32 @PyArg_UnpackTuple(i8*, i8*, i64, i64, ...) local_unnamed_addr

; Function Attrs: nounwind
declare i32 @puts(i8* nocapture readonly) local_unnamed_addr #0

declare void @PyErr_SetString(i8*, i8*) local_unnamed_addr

declare void @Py_IncRef(i8*) local_unnamed_addr

; Function Attrs: nounwind
declare void @llvm.stackprotector(i8*, i8**) #0

attributes #0 = { nounwind }

!0 = !{!"branch_weights", i32 1, i32 99}
