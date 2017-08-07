extern "C" int generate_dag_plan(char*);

#include <string>
//
//struct small_tuple{
//    long a;
//    long b;
//};
//
//struct big_tuple {
//    long a;
//    long b;
//    long c;
//    long d;
//};
//
//small_tuple func(small_tuple t){
//    return {t.a, t.b};
//}
//
int main(int argc, char **argv) {
    //get the dag string as an argument
    std::string test = "{\n"
            "  \"action\": \"collect\",\n"
            "  \"dag\": [\n"
            "    {\n"
            "      \"id\": 0,\n"
            "      \"predecessors\": [],\n"
            "      \"op\": \"collection_source\",\n"
            "      \"size\": 10,\n"
            "      \"output_type\": \"double\"\n"
            "    },\n"
            "    {\n"
            "      \"id\": 1,\n"
            "      \"predecessors\": [\n"
            "        0\n"
            "      ],\n"
            "      \"op\": \"map\",\n"
            "      \"func\": \"define { double, double } @\\\"cfunc._ZN8__main__8func$242Ed\\\"(double %.1) local_unnamed_addr #1 {\\nentry:\\n  %.15.i = fadd double %.1, 1.000000e+00\\n  %.27.i = fmul double %.1, 1.000000e+01\\n  %.15.fca.0.insert = insertvalue { double, double } undef, double %.15.i, 0\\n  %.15.fca.1.insert = insertvalue { double, double } %.15.fca.0.insert, double %.27.i, 1\\n  ret { double, double } %.15.fca.1.insert\\n}\\n\\n\",\n"
            "      \"output_type\": \"(double, double)\"\n"
            "    }\n"
            "  ]\n"
            "}";
    generate_dag_plan((char *) test.c_str());
    //compile the code to some object file

    //return

    //in python call the execute() function

    //which should return a pointer to the result
    //which should be interpreted as specified by the user
    //e.g. list or an integer(if the action is count)
    return 0;
}
