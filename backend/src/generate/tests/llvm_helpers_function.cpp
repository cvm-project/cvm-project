#define CATCH_CONFIG_MAIN

#include <string>

#include <catch2/catch.hpp>

#include "llvm_helpers/function.hpp"

TEST_CASE("Read false, primitive return type", "") {
    std::string ir =
            "define i64 "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"(i64 %.1, "
            "i64 %.2) local_unnamed_addr #1 {\n"
            "entry:\n"
            "  ret i64 %.1\n"
            "}";
    llvm_helpers::Function parser(ir);
    REQUIRE(!parser.ComputeIsArgumentRead(0));
}

TEST_CASE("Read true, primitive return type", "") {
    std::string ir =
            "define i64 "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"(i64 %.1, "
            "i64 %.2) local_unnamed_addr #1 {\n"
            "entry:\n"
            "  %.27.i = mul nsw i64 %.1, 10\n"
            "  ret i64 %.27.i\n"
            "}";
    llvm_helpers::Function parser(ir);
    REQUIRE(parser.ComputeIsArgumentRead(0));
}

TEST_CASE("input arg 1 used in output, primitive return type", "") {
    std::string ir =
            "define i64 "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"(i64 %.1, "
            "i64 %.2) local_unnamed_addr #1 {\n"
            "entry:\n"
            "  ret i64 %.1\n"
            "}";
    llvm_helpers::Function parser(ir);
    REQUIRE(parser.ComputeOutputPositions(0)[0] == 0);
    REQUIRE(parser.ComputeOutputPositions(0).size() == 1);
    REQUIRE(parser.ComputeOutputPositions(1).empty());
}

TEST_CASE("input arg 2 used in output, primitive return type", "") {
    std::string ir =
            "define i64 "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"(i64 %.1, "
            "i64 %.2) local_unnamed_addr #1 {\n"
            "entry:\n"
            "  ret i64 %.2\n"
            "}";
    llvm_helpers::Function parser(ir);
    REQUIRE(parser.ComputeOutputPositions(1)[0] == 0);
    REQUIRE(parser.ComputeOutputPositions(1).size() == 1);
    REQUIRE(parser.ComputeOutputPositions(0).empty());
}

TEST_CASE("Read false, struct return type", "") {
    std::string ir =
            "define { i64, i64 } "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"(i64 %.1, "
            "i64 %.2) {\n"
            "entry:"
            "  %.16.fca.0.insert = insertvalue { i64, i64 } undef, i64 %.1, 0"
            "  %.16.fca.1.insert = insertvalue { i64, i64 } %.16.fca.0.insert, "
            "i64 3, 1"
            "  ret { i64, i64 } %.16.fca.1.insert"
            "}";
    llvm_helpers::Function parser(ir);
    REQUIRE(!parser.ComputeIsArgumentRead(0));
    REQUIRE(!parser.ComputeIsArgumentRead(1));
}

TEST_CASE("Read true, caller pointer return type", "") {
    std::string ir =
            "define void "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"({ i64, "
            "i64, i64 }* nocapture %.1, i64 %.2, i64 %.3) local_unnamed_addr "
            "#0 {\n"
            "entry:\n"
            "  %.27.i = mul nsw i64 %.2, 10\n"
            "  %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*\n"
            "  store i64 %.27.i, i64* %.1.repack5, align 8\n"
            "  %.1.repack1 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 1\n"
            "  store i64 2, i64* %.1.repack1, align 8\n"
            "  %.1.repack3 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 2\n"
            "  store i64 3, i64* %.1.repack3, align 8\n"
            "  ret void\n"
            "}\n"
            "\n"
            "";
    llvm_helpers::Function parser(ir);
    REQUIRE(parser.ComputeIsArgumentRead(0));
    REQUIRE(!parser.ComputeIsArgumentRead(1));
}

TEST_CASE("Read false, caller pointer return type", "") {
    std::string ir =
            "define void "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"({ i64, "
            "i64, i64 }* nocapture %.1, i64 %.2, i64 %.3) local_unnamed_addr "
            "#0 {\n"
            "entry:\n"
            "  %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*\n"
            "  store i64 %.2, i64* %.1.repack5, align 8\n"
            "  %.1.repack1 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 1\n"
            "  store i64 %.2, i64* %.1.repack1, align 8\n"
            "  %.1.repack3 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 2\n"
            "  store i64 %.3, i64* %.1.repack3, align 8\n"
            "  ret void\n"
            "}\n"
            "\n"
            "";
    llvm_helpers::Function parser(ir);
    REQUIRE(!parser.ComputeIsArgumentRead(0));
    REQUIRE(!parser.ComputeIsArgumentRead(1));
}

TEST_CASE("input arg 1 used in output, pointer return type", "") {
    std::string ir =
            "define void "
            "@\"cfunc.notuniquename218303dba31a092a63fd8a50e54f2c15\"({ i64, "
            "i64, i64 }* nocapture %.1, i64 %.2, i64 %.3) local_unnamed_addr "
            "#0 {\n"
            "entry:\n"
            "  %.1.repack5 = bitcast { i64, i64, i64 }* %.1 to i64*\n"
            "  store i64 %.2, i64* %.1.repack5, align 8\n"
            "  %.1.repack1 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 1\n"
            "  store i64 %.2, i64* %.1.repack1, align 8\n"
            "  %.1.repack3 = getelementptr inbounds { i64, i64, i64 }, { i64, "
            "i64, i64 }* %.1, i64 0, i32 2\n"
            "  store i64 %.3, i64* %.1.repack3, align 8\n"
            "  ret void\n"
            "}";
    llvm_helpers::Function parser(ir);
    size_t pos = parser.ComputeOutputPositions(0)[0] +
                 parser.ComputeOutputPositions(0)[1];
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    REQUIRE(pos == 1);  // false positive
    REQUIRE(parser.ComputeOutputPositions(0).size() == 2);
    REQUIRE(parser.ComputeOutputPositions(1).size() == 1);
    REQUIRE(parser.ComputeOutputPositions(1)[0] == 2);
}
