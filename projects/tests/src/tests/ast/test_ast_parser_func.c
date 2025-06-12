#include "test_ast_parser_fixture.h"

// FUN PARAM

UTEST_F(TestASTParser, fun_param1) {
    AST_TEST_INIT("", ast_parser_parse_fun_param);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_param2) {
    AST_TEST_INIT("+", ast_parser_parse_fun_param);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_param3) {
    AST_TEST_INIT("a", ast_parser_parse_fun_param);
    ASSERT_AST_FUN_PARAM(node, "a", AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, fun_param4) {
    AST_TEST_INIT("a:", ast_parser_parse_fun_param);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_param5) {
    AST_TEST_INIT("a: uint", ast_parser_parse_fun_param);
    ASSERT_AST_FUN_PARAM(node, "a", AST_NODE_TYPE_CUSTOM);
    ASSERT_AST_TYPE_CUSTOM(node->as.fun_param.type, "uint");
}

// FUN SIGN

UTEST_F(TestASTParser, fun_sign1) {
    AST_TEST_INIT("", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign2) {
    AST_TEST_INIT("+", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign3) {
    AST_TEST_INIT("main", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign4) {
    AST_TEST_INIT("main(", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign5) {
    AST_TEST_INIT("main()", ast_parser_parse_fun_sign);
    ASSERT_AST_FUN_SIGN(node, "main", AST_NODE_UNKNOWN, 0);
}

UTEST_F(TestASTParser, fun_sign6) {
    AST_TEST_INIT("main():", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign7) {
    AST_TEST_INIT("main(): uint", ast_parser_parse_fun_sign);
    ASSERT_AST_FUN_SIGN(node, "main", AST_NODE_TYPE_CUSTOM, 0);
    ASSERT_AST_TYPE_CUSTOM(node->as.fun_sign.type, "uint");
}

UTEST_F(TestASTParser, fun_sign8) {
    AST_TEST_INIT("main(a): uint", ast_parser_parse_fun_sign);
    ASSERT_AST_FUN_SIGN(node, "main", AST_NODE_TYPE_CUSTOM, 1);
    ASSERT_AST_TYPE_CUSTOM(node->as.fun_sign.type, "uint");
    //
    const ASTNode* param = vector_at(&node->as.fun_sign.params, 0);
    ASSERT_AST_FUN_PARAM(param, "a", AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, fun_sign9) {
    AST_TEST_INIT("main(a, b): uint", ast_parser_parse_fun_sign);
    ASSERT_AST_FUN_SIGN(node, "main", AST_NODE_TYPE_CUSTOM, 2);
    ASSERT_AST_TYPE_CUSTOM(node->as.fun_sign.type, "uint");
    //
    const ASTNode* param = vector_at(&node->as.fun_sign.params, 0);
    ASSERT_AST_FUN_PARAM(param, "a", AST_NODE_UNKNOWN);
    //
    const ASTNode* param2 = vector_at(&node->as.fun_sign.params, 1);
    ASSERT_AST_FUN_PARAM(param2, "b", AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, fun_sign10) {
    AST_TEST_INIT("main(a, b,): uint", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign11) {
    AST_TEST_INIT("main(a:): uint", ast_parser_parse_fun_sign);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_sign12) {
    AST_TEST_INIT("main(a: u8)", ast_parser_parse_fun_sign);
    ASSERT_AST_FUN_SIGN(node, "main", AST_NODE_UNKNOWN, 1);
    //
    const ASTNode* param = vector_at(&node->as.fun_sign.params, 0);
    ASSERT_AST_FUN_PARAM(param, "a", AST_NODE_TYPE_BUILTIN);
    ASSERT_AST_TYPE_BUILTIN(param->as.fun_param.type, TOKEN_KEYWORD_U8);
}

// FUN DECL

UTEST_F(TestASTParser, fun_decl1) {
    AST_TEST_INIT("", ast_parser_parse_fun_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_decl2) {
    AST_TEST_INIT("+", ast_parser_parse_fun_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_decl3) {
    AST_TEST_INIT("fun", ast_parser_parse_fun_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, fun_decl4) {
    AST_TEST_INIT("fun main() end", ast_parser_parse_fun_decl);
    ASSERT_AST_FUN_DECL(node, "main", AST_NODE_UNKNOWN, 0, 0);
}
