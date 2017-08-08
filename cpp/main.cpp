extern "C" int generate_dag_plan(char*);

#include <string>

int main(int argc, char **argv) {
    std::string test = "{\n"
            "  \"action\": \"collect\",\n"
            "  \"dag\": [\n"
            "    {\n"
            "      \"id\": 0,\n"
            "      \"predecessors\": [],\n"
            "      \"op\": \"collection_source\",\n"
            "      \"output_type\": \"long\",\n"
            "      \"data_ptr\": 16252192,\n"
            "      \"data_size\": 10\n"
            "    },\n"
            "    {\n"
            "      \"id\": 1,\n"
            "      \"predecessors\": [\n"
            "        0\n"
            "      ],\n"
            "      \"op\": \"reduce\",\n"
            "      \"func\": \"define i64 @\\\"cfunc._ZN5blaze5tests14test_operators10TestReduce15test_reduce_sum12$3clocals$3e16$3clambda$3e$242Exx\\\"(i64 %.1, i64 %.2) local_unnamed_addr #1 {\\nentry:\\n  %.15.i = add nsw i64 %.2, %.1\\n  ret i64 %.15.i\\n}\\n\\n\",\n"
            "      \"output_type\": \"long\"\n"
            "    }\n"
            "  ]\n"
            "}";
    generate_dag_plan((char *) test.c_str());
    return 0;
}
