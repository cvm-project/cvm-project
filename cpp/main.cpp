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
            "      \"output_type\": \"(long, long)\",\n"
            "      \"data_ptr\": 24490816,\n"
            "      \"data_size\": 10\n"
            "    },\n"
            "    {\n"
            "      \"id\": 1,\n"
            "      \"predecessors\": [\n"
            "        0\n"
            "      ],\n"
            "      \"op\": \"map\",\n"
            "      \"func\": \"define { i64, i64 } @\\\"cfunc._ZN5blaze5tests14test_operators8TestJoin12test_overlap12$3clocals$3e16$3clambda$3e$242E5TupleIxxE\\\"(i64 %.1, i64 %.2) local_unnamed_addr #1 {\\nentry:\\n  %.16.fca.0.insert = insertvalue { i64, i64 } undef, i64 %.1, 0\\n  %.16.fca.1.insert = insertvalue { i64, i64 } %.16.fca.0.insert, i64 %.2, 1\\n  ret { i64, i64 } %.16.fca.1.insert\\n}\\n\\n\",\n"
            "      \"output_type\": \"(long, long)\"\n"
            "    },\n"
            "    {\n"
            "      \"id\": 2,\n"
            "      \"predecessors\": [],\n"
            "      \"op\": \"collection_source\",\n"
            "      \"output_type\": \"(long, long, long)\",\n"
            "      \"data_ptr\": 24708656,\n"
            "      \"data_size\": 10\n"
            "    },\n"
            "    {\n"
            "      \"id\": 3,\n"
            "      \"predecessors\": [\n"
            "        1,\n"
            "        2\n"
            "      ],\n"
            "      \"op\": \"join\",\n"
            "      \"output_type\": \"(long, (long, long, long))\"\n"
            "    }\n"
            "  ]\n"
            "}";
    generate_dag_plan((char *) test.c_str());
    return 0;
}
