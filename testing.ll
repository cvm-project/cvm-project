define void @"cfunc._ZN2ml6kmeans6KMeans3fit12$3clocals$3e12reduce_1$244E5TupleI5TupleIddE5TupleIxddExE5TupleI5TupleIddE5TupleIxddExE"({ { double, double }, { i64, double, double }, double }* nocapture %.1, double %.2, double %.3, i64 %.4, double %.5, double %.6, i64 %.7, double %.8, double %.9, i64 %.10, double %.11, double %.12, i64 %.13) local_unnamed_addr #0 {
entry:
  %.396.i = fsub fast double %.2, %.11
  %.409.i = fmul fast double %.396.i, %.396.i
  %.396.1.i = fsub fast double %.3, %.12
  %.409.1.i = fmul fast double %.396.1.i, %.396.1.i
  %.421.1.i = fadd fast double %.409.1.i, %.409.i
  %.442.i = sitofp i64 %.7 to double
  %.443.i = fcmp fast olt double %.421.1.i, %.442.i
  br i1 %.443.i, label %B110.i, label %"_ZN2ml6kmeans6KMeans3fit12$3clocals$3e12reduce_1$244E5TupleI5TupleIddE5TupleIxddExE5TupleI5TupleIddE5TupleIxddExE.exit"

B110.i:                                           ; preds = %entry
  br label %"_ZN2ml6kmeans6KMeans3fit12$3clocals$3e12reduce_1$244E5TupleI5TupleIddE5TupleIxddExE5TupleI5TupleIddE5TupleIxddExE.exit"

"_ZN2ml6kmeans6KMeans3fit12$3clocals$3e12reduce_1$244E5TupleI5TupleIddE5TupleIxddExE5TupleI5TupleIddE5TupleIxddExE.exit": ; preds = %entry, %B110.i
  %.5.sink = phi double [ %.11, %B110.i ], [ %.5, %entry ]
  %.6.sink = phi double [ %.12, %B110.i ], [ %.6, %entry ]
  %.442.i.sink = phi double [ %.421.1.i, %B110.i ], [ %.442.i, %entry ]
  %.15.sroa.6.0 = phi i64 [ %.10, %B110.i ], [ %.4, %entry ]
  %0 = bitcast { { double, double }, { i64, double, double }, double }* %.1 to double*
  store double %.2, double* %0, align 8
  %.1.repack.repack9 = getelementptr inbounds { { double, double }, { i64, double, double }, double }, { { double, double }, { i64, double, double }, double }* %.1, i64 0, i32 0, i32 1
  store double %.3, double* %.1.repack.repack9, align 8
  %.1.repack1.repack = getelementptr inbounds { { double, double }, { i64, double, double }, double }, { { double, double }, { i64, double, double }, double }* %.1, i64 0, i32 1, i32 0
  store i64 %.15.sroa.6.0, i64* %.1.repack1.repack, align 8
  %.1.repack1.repack5 = getelementptr inbounds { { double, double }, { i64, double, double }, double }, { { double, double }, { i64, double, double }, double }* %.1, i64 0, i32 1, i32 1
  store double %.5.sink, double* %.1.repack1.repack5, align 8
  %.1.repack1.repack7 = getelementptr inbounds { { double, double }, { i64, double, double }, double }, { { double, double }, { i64, double, double }, double }* %.1, i64 0, i32 1, i32 2
  store double %.6.sink, double* %.1.repack1.repack7, align 8
  %.1.repack3 = getelementptr inbounds { { double, double }, { i64, double, double }, double }, { { double, double }, { i64, double, double }, double }* %.1, i64 0, i32 2
  store double %.442.i.sink, double* %.1.repack3, align 8
  ret void
}


