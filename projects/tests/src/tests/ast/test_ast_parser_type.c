#include "test_ast_parser_fixture.h"

// BUILTIN

UTEST_F(TestASTParser, type_builtin1) {
    AST_TEST_INIT("", ast_parser_parse_type_builtin);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_builtin2) {
    AST_TEST_INIT("+", ast_parser_parse_type_builtin);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_builtin3) {
    AST_TEST_INIT("^u8", ast_parser_parse_type_builtin);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_builtin4) {
    AST_TEST_INIT("[]u8", ast_parser_parse_type_builtin);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_builtin5) {
    AST_TEST_INIT("u8", ast_parser_parse_type_builtin);
    ASSERT_AST_TYPE_BUILTIN(node, TOKEN_KEYWORD_U8);
}

// CUSTOM

UTEST_F(TestASTParser, type_custom1) {
    AST_TEST_INIT("", ast_parser_parse_type_custom);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_custom2) {
    AST_TEST_INIT("+", ast_parser_parse_type_custom);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_custom3) {
    AST_TEST_INIT("^arr", ast_parser_parse_type_custom);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_custom4) {
    AST_TEST_INIT("[]arr", ast_parser_parse_type_custom);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_custom5) {
    AST_TEST_INIT("arr", ast_parser_parse_type_custom);
    ASSERT_AST_TYPE_CUSTOM(node, "arr");
}

// PTR

UTEST_F(TestASTParser, type_ptr1) {
    AST_TEST_INIT("", ast_parser_parse_type_ptr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_ptr2) {
    AST_TEST_INIT("+", ast_parser_parse_type_ptr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_ptr3) {
    AST_TEST_INIT("[]u8", ast_parser_parse_type_ptr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_ptr4) {
    AST_TEST_INIT("^", ast_parser_parse_type_ptr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_ptr5) {
    AST_TEST_INIT("^u8", ast_parser_parse_type_ptr);
    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(node->as.type_ptr.type, TOKEN_KEYWORD_U8);
}

UTEST_F(TestASTParser, type_ptr6) {
    AST_TEST_INIT("^^u8", ast_parser_parse_type_ptr);
    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_PTR);
    ASSERT_AST_TYPE_PTR(node->as.type_ptr.type, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(node->as.type_ptr.type->as.type_ptr.type, TOKEN_KEYWORD_U8);
}

// ARR

UTEST_F(TestASTParser, type_arr1) {
    AST_TEST_INIT("", ast_parser_parse_type_arr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_arr2) {
    AST_TEST_INIT("+", ast_parser_parse_type_arr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_arr3) {
    AST_TEST_INIT("^u8", ast_parser_parse_type_arr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_arr4) {
    AST_TEST_INIT("[", ast_parser_parse_type_arr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_arr5) {
    AST_TEST_INIT("[]", ast_parser_parse_type_arr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, type_arr6) {
    AST_TEST_INIT("[]u8", ast_parser_parse_type_arr);
    ASSERT_AST_TYPE_ARR(node, AST_NODE_UNKNOWN, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(node->as.type_arr.type, TOKEN_KEYWORD_U8);
}

UTEST_F(TestASTParser, type_arr7) {
    AST_TEST_INIT("[][]u8", ast_parser_parse_type_arr);
    ASSERT_AST_TYPE_ARR(node, AST_NODE_UNKNOWN, AST_NODE_TYPE_ARR);
    ASSERT_AST_TYPE_ARR(node->as.type_arr.type, AST_NODE_UNKNOWN, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(node->as.type_arr.type->as.type_arr.type, TOKEN_KEYWORD_U8);
}

// COMPLEX

UTEST_F(TestASTParser, type_complex1) {
    AST_TEST_INIT("^[10]u8", ast_parser_parse_type);
    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_ARR);
    ASSERT_AST_TYPE_ARR(node->as.type_ptr.type, AST_NODE_EXPR_LITERAL, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(node->as.type_ptr.type->as.type_arr.type, TOKEN_KEYWORD_U8);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.type_ptr.type->as.type_arr.size_expr, "10");
}

UTEST_F(TestASTParser, type_complex2) {
    AST_TEST_INIT("[]^Person", ast_parser_parse_type);
    ASSERT_AST_TYPE_ARR(node, AST_NODE_UNKNOWN, AST_NODE_TYPE_PTR);
    ASSERT_AST_TYPE_PTR(node->as.type_arr.type, AST_NODE_TYPE_CUSTOM);
    ASSERT_AST_TYPE_CUSTOM(node->as.type_arr.type->as.type_ptr.type, "Person");
}

UTEST_F(TestASTParser, type_complex3) {
    AST_TEST_INIT("[3][4]u64", ast_parser_parse_type);
    ASSERT_AST_TYPE_ARR(node, AST_NODE_EXPR_LITERAL, AST_NODE_TYPE_ARR);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.type_arr.size_expr, "3");
    ASSERT_AST_TYPE_ARR(node->as.type_arr.type, AST_NODE_EXPR_LITERAL, AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.type_arr.type->as.type_arr.size_expr, "4");
    ASSERT_AST_TYPE_BUILTIN(node->as.type_arr.type->as.type_arr.type, TOKEN_KEYWORD_U64);
}