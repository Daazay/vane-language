#include "test_ast_parser_fixture.h"

// BINARY

UTEST_F(TestASTParser, expr_binary1) {
    AST_TEST_INIT("", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_NONE);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_binary2) {
    AST_TEST_INIT("a", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_NONE);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_binary3) {
    AST_TEST_INIT("+", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_ADDITIVE);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_binary4) {
    AST_TEST_INIT("+ +", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_ADDITIVE);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_binary5) {
    AST_TEST_INIT("+ 1", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_ADDITIVE);
    ASSERT_AST_EXPR_BINARY(node, TOKEN_PLUS, AST_PLUG_KIND, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_binary.rhs, "1");
}

UTEST_F(TestASTParser, expr_binary6) {
    AST_TEST_INIT("+ 1 + 2", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_ADDITIVE);
    ASSERT_AST_EXPR_BINARY(node, TOKEN_PLUS, AST_PLUG_KIND, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_binary.rhs, "1");
}

UTEST_F(TestASTParser, expr_binary7) {
    AST_TEST_INIT("+ 1 * 2", ast_parser_parse_expr_binary, AST_PLUG(), OP_PREC_ADDITIVE);
    ASSERT_AST_EXPR_BINARY(node, TOKEN_PLUS, AST_PLUG_KIND, AST_NODE_EXPR_BINARY);
    ASSERT_AST_EXPR_BINARY(node->as.expr_binary.rhs, TOKEN_STAR, AST_NODE_EXPR_LITERAL, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_binary.rhs->as.expr_binary.rhs, "2");
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_binary.rhs->as.expr_binary.lhs, "1");
}

// PREFIX UNARY

UTEST_F(TestASTParser, expr_prefix_unary1) {
    AST_TEST_INIT("", ast_parser_parse_expr_prefix_unary);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_prefix_unary2) {
    AST_TEST_INIT("a", ast_parser_parse_expr_prefix_unary);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, exprprefix_unary3) {
    AST_TEST_INIT("+", ast_parser_parse_expr_prefix_unary);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_prefix_unary4) {
    AST_TEST_INIT("++1", ast_parser_parse_expr_prefix_unary);
    ASSERT_AST_EXPR_PREFIX_UNARY(node, TOKEN_PLUS_PLUS, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_prefix_unary.rhs, "1");
}

UTEST_F(TestASTParser, expr_prefix_unary5) {
    AST_TEST_INIT("+ - 1", ast_parser_parse_expr_prefix_unary);
    ASSERT_AST_EXPR_PREFIX_UNARY(node, TOKEN_PLUS, AST_NODE_EXPR_PREFIX_UNARY);
    ASSERT_AST_EXPR_PREFIX_UNARY(node->as.expr_prefix_unary.rhs, TOKEN_MINUS, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.expr_prefix_unary.rhs->as.expr_prefix_unary.rhs, "1");
}

// POSTFIX UNARY

UTEST_F(TestASTParser, expr_postfix_unary1) {
    AST_TEST_INIT("", ast_parser_parse_expr_postfix_unary, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_postfix_unary2) {
    AST_TEST_INIT("a", ast_parser_parse_expr_postfix_unary, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_postfix_unary3) {
    AST_TEST_INIT("++", ast_parser_parse_expr_postfix_unary, AST_PLUG());
    ASSERT_AST_EXPR_POSTFIX_UNARY(node, TOKEN_PLUS_PLUS, AST_PLUG_KIND);
}

UTEST_F(TestASTParser, expr_postfix_unary4) {
    AST_TEST_INIT("++ -", ast_parser_parse_expr_postfix_unary, AST_PLUG());
    ASSERT_AST_EXPR_POSTFIX_UNARY(node, TOKEN_PLUS_PLUS, AST_PLUG_KIND);
}

// BRACES

UTEST_F(TestASTParser, expr_braces1) {
    AST_TEST_INIT("", ast_parser_parse_expr_braces);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_braces2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_braces);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_braces3) {
    AST_TEST_INIT("(", ast_parser_parse_expr_braces);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_braces4) {
    AST_TEST_INIT("()", ast_parser_parse_expr_braces);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_braces5) {
    AST_TEST_INIT("(true)", ast_parser_parse_expr_braces);
    ASSERT_AST_EXPR_BRACES(node, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.expr_braces.expr, "true");
}

// PLACE

UTEST_F(TestASTParser, expr_place1) {
    AST_TEST_INIT("", ast_parser_parse_expr_place);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_place2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_place);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_place3) {
    AST_TEST_INIT("V", ast_parser_parse_expr_place);
    ASSERT_AST_EXPR_PLACE(node, "V");
}

// CALL

UTEST_F(TestASTParser, expr_call1) {
    AST_TEST_INIT("", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_call2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_call3) {
    AST_TEST_INIT("(", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_call4) {
    AST_TEST_INIT("(,)", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_call5) {
    AST_TEST_INIT("(arg,)", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_call6) {
    AST_TEST_INIT("()", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_EXPR_CALL(node, AST_PLUG_KIND, 0);
}

UTEST_F(TestASTParser, expr_call7) {
    AST_TEST_INIT("(a)", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_EXPR_CALL(node, AST_PLUG_KIND, 1);
    const ASTNode* arg1 = vector_at(&node->as.expr_call.args, 0);
    ASSERT_AST_EXPR_PLACE(arg1, "a");
}

UTEST_F(TestASTParser, expr_call8) {
    AST_TEST_INIT("(a, b)", ast_parser_parse_expr_call, AST_PLUG());
    ASSERT_AST_EXPR_CALL(node, AST_PLUG_KIND, 2);
    const ASTNode* arg1 = vector_at(&node->as.expr_call.args, 0);
    ASSERT_AST_EXPR_PLACE(arg1, "a");
    const ASTNode* arg2 = vector_at(&node->as.expr_call.args, 1);
    ASSERT_AST_EXPR_PLACE(arg2, "b");
}

// INDEX

UTEST_F(TestASTParser, expr_index1) {
    AST_TEST_INIT("", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index3) {
    AST_TEST_INIT("[", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index4) {
    AST_TEST_INIT("[,]", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index5) {
    AST_TEST_INIT("[]", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index6) {
    AST_TEST_INIT("[arg,]", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_index7) {
    AST_TEST_INIT("[a]", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_EXPR_INDEX(node, AST_PLUG_KIND, 1);
    const ASTNode* arg1 = vector_at(&node->as.expr_call.args, 0);
    ASSERT_AST_EXPR_PLACE(arg1, "a");
}

UTEST_F(TestASTParser, expr_index8) {
    AST_TEST_INIT("[a, b]", ast_parser_parse_expr_index, AST_PLUG());
    ASSERT_AST_EXPR_INDEX(node, AST_PLUG_KIND, 2);
    const ASTNode* arg1 = vector_at(&node->as.expr_call.args, 0);
    ASSERT_AST_EXPR_PLACE(arg1, "a");
    const ASTNode* arg2 = vector_at(&node->as.expr_call.args, 1);
    ASSERT_AST_EXPR_PLACE(arg2, "b");
}

// MEMBER

UTEST_F(TestASTParser, expr_member1) {
    AST_TEST_INIT("", ast_parser_parse_expr_member, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_member2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_member, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_member3) {
    AST_TEST_INIT(".", ast_parser_parse_expr_member, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_member4) {
    AST_TEST_INIT(".+", ast_parser_parse_expr_member, AST_PLUG());
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_member5) {
    AST_TEST_INIT("._", ast_parser_parse_expr_member, AST_PLUG());
    ASSERT_AST_EXPR_MEMBER(node, AST_PLUG_KIND, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(node->as.expr_member.member, "_");
}

// LITERAL DEC

UTEST_F(TestASTParser, expr_literal_dec1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_dec2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_dec3) {
    AST_TEST_INIT("10_", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_dec4) {
    AST_TEST_INIT("1__000", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_dec5) {
    AST_TEST_INIT("10", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_DEC(node, "10");
}

UTEST_F(TestASTParser, expr_literal_dec6) {
    AST_TEST_INIT("1_000_000", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_DEC(node, "1_000_000");
}

// LITERAL HEX

UTEST_F(TestASTParser, expr_literal_hex1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex3) {
    AST_TEST_INIT("0x", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex4) {
    AST_TEST_INIT("0x10_", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex5) {
    AST_TEST_INIT("0x10__00", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex6) {
    AST_TEST_INIT("0xXYZ", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_hex7) {
    AST_TEST_INIT("0x1AFC", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_HEX(node, "0x1AFC");
}

UTEST_F(TestASTParser, expr_literal_hex8) {
    AST_TEST_INIT("0x10_FF", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_HEX(node, "0x10_FF");
}

// LITERAL BIN

UTEST_F(TestASTParser, expr_literal_bin1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin3) {
    AST_TEST_INIT("0b", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin4) {
    AST_TEST_INIT("0b10_", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin5) {
    AST_TEST_INIT("0b10__00", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin6) {
    AST_TEST_INIT("0bXYZ", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bin7) {
    AST_TEST_INIT("0b1001", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_BIN(node, "0b1001");
}

UTEST_F(TestASTParser, expr_literal_bin8) {
    AST_TEST_INIT("0b1000_0000", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_BIN(node, "0b1000_0000");
}

// LITERAL BOOL

UTEST_F(TestASTParser, expr_literal_bool1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bool2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bool3) {
    AST_TEST_INIT("True", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bool4) {
    AST_TEST_INIT("False", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_bool5) {
    AST_TEST_INIT("true", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_BOOL(node, "true");
}

UTEST_F(TestASTParser, expr_literal_bool6) {
    AST_TEST_INIT("false", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_BOOL(node, "false");
}

// LITERAL CHAR

UTEST_F(TestASTParser, expr_literal_char1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char3) {
    AST_TEST_INIT("'", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char4) {
    AST_TEST_INIT("'\n'", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char5) {
    AST_TEST_INIT("''", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char6) {
    AST_TEST_INIT("'too many'", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_char7) {
    AST_TEST_INIT("'a'", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_CHAR(node, "a");
}

UTEST_F(TestASTParser, expr_literal_char8) {
    AST_TEST_INIT("'\\n'", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_CHAR(node, "\\n");
}

// LITERAL STRING

UTEST_F(TestASTParser, expr_literal_string1) {
    AST_TEST_INIT("", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_string2) {
    AST_TEST_INIT("+", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_string3) {
    AST_TEST_INIT("\"", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_string4) {
    AST_TEST_INIT("\"\n\"", ast_parser_parse_expr_literal);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, expr_literal_string5) {
    AST_TEST_INIT("\"\"", ast_parser_parse_expr_literal);
    ASSERT_AST_NODE(node, AST_NODE_EXPR_LITERAL);
    ASSERT_EQ_MSG(node->as.expr_literal.kind, TOKEN_LITERAL_STRING, "invalid expr literal kind");
    ASSERT_TRUE_MSG(string_is_empty(&node->as.expr_literal.value), "expr literal value must be equal to STRING_EMPTY in this specific case");
}

UTEST_F(TestASTParser, expr_literal_string6) {
    AST_TEST_INIT("\"literal\"", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_STRING(node, "literal");
}

UTEST_F(TestASTParser, expr_literal_string7) {
    AST_TEST_INIT("\"\\n\"", ast_parser_parse_expr_literal);
    ASSERT_AST_EXPR_LITERAL_STRING(node, "\\n");
}
