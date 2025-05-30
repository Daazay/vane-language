#include <utest/utest.h>

#include "test_ast_parser.h"

// MISC

// ::ID

UTEST_F(TestASTParser, identifier_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_identifier);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, identifier_invalid) {
    AST_TEST_INIT("+", ast_parser_parse_identifier);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, identifier_valid) {
    AST_TEST_INIT("id", ast_parser_parse_identifier);

    ASSERT_AST_ID(node, "id");
}

// TYPE

UTEST_F(TestASTParser, type_basic_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_typeref_basic);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_basic_invalid) {
    AST_TEST_INIT("+", ast_parser_parse_typeref_basic);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_basic_valid1) {
    AST_TEST_INIT("i32", ast_parser_parse_typeref_basic);

    ASSERT_AST_TYPE_I32(node);
}

UTEST_F(TestASTParser, type_basic_valid2) {
    AST_TEST_INIT("arr", ast_parser_parse_typeref_basic);

    ASSERT_AST_TYPE_CUSTOM(node, "arr");
}

// ::PTR

UTEST_F(TestASTParser, type_ptr_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_typeref_ptr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_ptr_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_typeref_ptr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_ptr_invalid2) {
    AST_TEST_INIT("^", ast_parser_parse_typeref_ptr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_ptr_valid1) {
    AST_TEST_INIT("^u8", ast_parser_parse_typeref_ptr);

    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_U8);
}

UTEST_F(TestASTParser, type_ptr_valid2) {
    AST_TEST_INIT("^^u8", ast_parser_parse_typeref_ptr);

    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_PTR);
    ASSERT_AST_TYPE_PTR(node->as.type_ptr.type, AST_NODE_TYPE_U8);
}

// ::ARR

UTEST_F(TestASTParser, type_arr_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_typeref_arr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_arr_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_typeref_arr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_arr_invalid2) {
    AST_TEST_INIT("[]", ast_parser_parse_typeref_arr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_arr_invalid3) {
    AST_TEST_INIT("[3]", ast_parser_parse_typeref_arr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_arr_invalid4) {
    AST_TEST_INIT("[+]u8", ast_parser_parse_typeref_arr);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_arr_valid1) {
    AST_TEST_INIT("[]u8", ast_parser_parse_typeref_arr);

    ASSERT_AST_TYPE_ARR(node, AST_NODE_TYPE_U8, AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, type_arr_valid2) {
    AST_TEST_INIT("[14]u16", ast_parser_parse_typeref_arr);

    ASSERT_AST_TYPE_ARR(node, AST_NODE_TYPE_U16, AST_NODE_LITERAL_DEC);
    ASSERT_AST_EXPR_DEC(node->as.type_arr.expr, "14");
}

// ::COMPLEX

UTEST_F(TestASTParser, type_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_typeref);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_typeref);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, type_valid1) {
    AST_TEST_INIT("u8", ast_parser_parse_typeref);

    ASSERT_AST_TYPE_U8(node);
}

UTEST_F(TestASTParser, type_valid2) {
    AST_TEST_INIT("v", ast_parser_parse_typeref);

    ASSERT_AST_TYPE_CUSTOM(node, "v");
}

UTEST_F(TestASTParser, type_valid3) {
    AST_TEST_INIT("^Item", ast_parser_parse_typeref);

    ASSERT_AST_TYPE_PTR(node, AST_NODE_TYPE_CUSTOM);
    ASSERT_AST_TYPE_CUSTOM(node->as.type_ptr.type, "Item");
}

UTEST_F(TestASTParser, type_valid4) {
    AST_TEST_INIT("[]Item", ast_parser_parse_typeref);

    ASSERT_AST_TYPE_ARR(node, AST_NODE_TYPE_CUSTOM, AST_NODE_UNKNOWN);
    ASSERT_AST_TYPE_CUSTOM(node->as.type_arr.type, "Item");
}

UTEST_F(TestASTParser, type_valid5) {
    AST_TEST_INIT("[5]^Item", ast_parser_parse_typeref);

    ASSERT_AST_TYPE_ARR(node, AST_NODE_TYPE_PTR, AST_NODE_LITERAL_DEC);
    ASSERT_AST_EXPR_DEC(node->as.type_arr.expr, "5");

    ASSERT_AST_TYPE_PTR(node->as.type_arr.type, AST_NODE_TYPE_CUSTOM);
    ASSERT_AST_TYPE_CUSTOM(node->as.type_arr.type->as.type_ptr.type, "Item");
}

// EXPR

// ::LITERAL

UTEST_F(TestASTParser, expr_literal_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_literal_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_literal_invalid2) {
    AST_TEST_INIT("\"", ast_parser_parse_expr_literal);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_literal_valid1) {
    AST_TEST_INIT("1235", ast_parser_parse_expr_literal);

    ASSERT_AST_EXPR_DEC(node, "1235");
}

UTEST_F(TestASTParser, expr_literal_valid2) {
    AST_TEST_INIT("\"test\"", ast_parser_parse_expr_literal);

    ASSERT_AST_EXPR_STRING(node, "test");
}

// ::NUD

// ::PLACE

UTEST_F(TestASTParser, expr_place_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_place);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_place_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_expr_place);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_place_valid1) {
    AST_TEST_INIT("id", ast_parser_parse_expr_place);

    ASSERT_AST_EXPR_PLACE(node, "id");
}

// ::NIL

UTEST_F(TestASTParser, expr_nil_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_nil);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_nil_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_expr_nil);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_nil_invalid2) {
    AST_TEST_INIT("nil", ast_parser_parse_expr_nil);

    ASSERT_AST_EXPR_NIL(node);
}

// ::PREFIX UNARY

UTEST_F(TestASTParser, expr_prefix_unary_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_prefix_unary);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_prefix_unary_invalid1) {
    AST_TEST_INIT("^", ast_parser_parse_expr_prefix_unary);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_prefix_unary_invalid2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_prefix_unary);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_prefix_unary_invalid3) {
    AST_TEST_INIT("a++", ast_parser_parse_expr_prefix_unary);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_prefix_unary_valid1) {
    AST_TEST_INIT("- 3", ast_parser_parse_expr_prefix_unary);

    ASSERT_AST_EXPR_PREFIX_UNARY(node, TOKEN_MINUS, AST_NODE_LITERAL_DEC);
    ASSERT_AST_EXPR_DEC(node->as.expr_unary.rhs, "3");
}

UTEST_F(TestASTParser, expr_prefix_unary_valid2) {
    AST_TEST_INIT("++i", ast_parser_parse_expr_prefix_unary);

    ASSERT_AST_EXPR_PREFIX_UNARY(node, TOKEN_PLUS_PLUS, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(node->as.expr_unary.rhs, "i");
}

UTEST_F(TestASTParser, expr_prefix_unary_valid3) {
    AST_TEST_INIT("- - a", ast_parser_parse_expr_prefix_unary);

    ASSERT_AST_EXPR_PREFIX_UNARY(node, TOKEN_MINUS, AST_NODE_EXPR_UNARY);
    ASSERT_AST_EXPR_PREFIX_UNARY(node->as.expr_unary.rhs, TOKEN_MINUS, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(node->as.expr_unary.rhs->as.expr_unary.rhs, "a");
}

// ::BRACES

UTEST_F(TestASTParser, expr_braces_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_braces);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_braces_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_expr_braces);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_braces_invalid2) {
    AST_TEST_INIT("()", ast_parser_parse_expr_braces);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_braces_invalid3) {
    AST_TEST_INIT("(-)", ast_parser_parse_expr_braces);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_braces_valid1) {
    AST_TEST_INIT("(a)", ast_parser_parse_expr_braces);

    ASSERT_AST_EXPR_BRACES(node, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(node->as.expr_braces.expr, "a");
}

// ::INIT LIST

UTEST_F(TestASTParser, expr_init_list_from_empty) {
    AST_TEST_INIT("", ast_parser_parse_expr_init_list);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_init_list_invalid1) {
    AST_TEST_INIT("+", ast_parser_parse_expr_init_list);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_init_list_invalid2) {
    AST_TEST_INIT("{", ast_parser_parse_expr_init_list);

    ASSERT_EQ(node, NULL);
}

UTEST_F(TestASTParser, expr_init_list_valid1) {
    AST_TEST_INIT("{ }", ast_parser_parse_expr_init_list);

    ASSERT_AST_EXPR_INIT_LIST(node, 0);
}

UTEST_F(TestASTParser, expr_init_list_valid2) {
    AST_TEST_INIT("{ 0 }", ast_parser_parse_expr_init_list);

    ASSERT_AST_EXPR_INIT_LIST(node, 1);

    const ASTNode* item1 = vector_at(&node->as.expr_init_list.items, 0);
    ASSERT_AST_EXPR_DEC(item1, "0");
}

UTEST_F(TestASTParser, expr_init_list_valid3) {
    AST_TEST_INIT("{ 0, 1, 2 }", ast_parser_parse_expr_init_list);

    ASSERT_AST_EXPR_INIT_LIST(node, 3);

    const ASTNode* item1 = vector_at(&node->as.expr_init_list.items, 0);
    ASSERT_AST_EXPR_DEC(item1, "0");
    const ASTNode* item2 = vector_at(&node->as.expr_init_list.items, 1);
    ASSERT_AST_EXPR_DEC(item2, "1");
    const ASTNode* item3 = vector_at(&node->as.expr_init_list.items, 2);
    ASSERT_AST_EXPR_DEC(item3, "2");
}

UTEST_F(TestASTParser, expr_init_list_valid4) {
    AST_TEST_INIT("{ 0, 1, 2, }", ast_parser_parse_expr_init_list);

    ASSERT_AST_EXPR_INIT_LIST(node, 3);

    const ASTNode* item1 = vector_at(&node->as.expr_init_list.items, 0);
    ASSERT_AST_EXPR_DEC(item1, "0");
    const ASTNode* item2 = vector_at(&node->as.expr_init_list.items, 1);
    ASSERT_AST_EXPR_DEC(item2, "1");
    const ASTNode* item3 = vector_at(&node->as.expr_init_list.items, 2);
    ASSERT_AST_EXPR_DEC(item3, "2");
}