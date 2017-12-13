extern "C" int generate_dag_plan(char *);

#include <fstream>
#include <streambuf>
#include <string>

int main(int argc, char **argv) {
    std::ifstream t("../dag.json");
    std::string test((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    //    std::string test = "{\"action\": \"collect\", \"dag\": [{\"id\": 0,
    //    \"predecessors\": [], \"op\": \"collection_source\", \"output_type\":
    //    \"(long, long)\", \"data_ptr\": 39287328, \"data_size\": 2}, {\"id\":
    //    1, \"predecessors\": [0], \"op\": \"map\", \"func\": \"define void
    //    @\\\"cfunc._ZN8__main__8func$242E5TupleIxxE\\\"({ i64, i64, i64 }*
    //    nocapture %.1, i64 %.2, i64 %.3) local_unnamed_addr #0 {\\nentry:\\n
    //    %.35.i = add nsw i64 %.2, 3\\n  %.60.i = mul nsw i64 %.2, 10\\n
    //    %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*\\n  store i64
    //    %.2, i64* %.1.repack5, align 8\\n  %.1.repack1 = getelementptr
    //    inbounds { i64, i64, i64 }, { i64, i64, i64 }* %.1, i64 0, i32 1\\n
    //    store i64 %.35.i, i64* %.1.repack1, align 8\\n  %.1.repack3 =
    //    getelementptr inbounds { i64, i64, i64 }, { i64, i64, i64 }* %.1, i64
    //    0, i32 2\\n  store i64 %.60.i, i64* %.1.repack3, align 8\\n  ret
    //    void\\n}\\n\\n\\n\", \"output_type\": \"(long, long, long)\"},
    //    {\"id\": 2, \"predecessors\": [1], \"op\": \"filter\", \"func\":
    //    \"define i1
    //    @\\\"cfunc._ZN8__main__16$3clambda$3e$244E5TupleIxxxE\\\"(i64 %.1, i64
    //    %.2, i64 %.3) local_unnamed_addr #1 {\\nentry:\\n  %.42.i = srem i64
    //    %.1, 2\\n  %.44.i = icmp slt i64 %.42.i, 0\\n  %.53.i = add nsw i64
    //    %.42.i, 2\\n  %.34.0.i = select i1 %.44.i, i64 %.53.i, i64 %.42.i\\n
    //    %.75.i = icmp eq i64 %.34.0.i, 0\\n  ret i1 %.75.i\\n}\\n\\n\\n\\n\",
    //    \"output_type\": \"(long, long, long)\"}]}";
    generate_dag_plan(const_cast<char *>(test.c_str()));
    return 0;
}
