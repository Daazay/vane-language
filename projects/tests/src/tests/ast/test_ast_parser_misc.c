#include "test_ast_parser_fixture.h"

// ID

UTEST_F(TestASTParser, misc_id1) {
    AST_TEST_INIT("", ast_parser_parse_identifier);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, misc_id2) {
    AST_TEST_INIT("+", ast_parser_parse_identifier);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, misc_id3) {
    AST_TEST_INIT("id", ast_parser_parse_identifier);
    ASSERT_AST_ID(node, "id");
}