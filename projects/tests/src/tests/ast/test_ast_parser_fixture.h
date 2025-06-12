#pragma once

#include <utest/utest.h>

#include <vane/ast/ast_parser.h>

struct TestASTParser {
    ReportCollector rc;
    FileContent fc;
    TokenStream ts;
    ASTParser ast_parser;
    ASTNode* ast_node;
};

UTEST_F_SETUP(TestASTParser) {
    utest_fixture->rc = report_collector_create(true);
    utest_fixture->fc = (FileContent){ 0 };
    utest_fixture->ts = (TokenStream){ 0 };
    utest_fixture->ast_parser = ast_parser_create(&utest_fixture->ts, &utest_fixture->rc);
    utest_fixture->ast_node = NULL;
}

UTEST_F_TEARDOWN(TestASTParser) {
    ast_node_destroy(utest_fixture->ast_node);
    ast_parser_destroy(&utest_fixture->ast_parser);
    token_stream_destroy(&utest_fixture->ts);
    //file_content_destroy(&utest_fixture->fc);

    report_collector_print(&utest_fixture->rc);
    report_collector_destroy(&utest_fixture->rc);
}

#define AST_PLUG_KIND AST_NODE_GROUP_MISC
#define AST_PLUG() ast_node_create(AST_PLUG_KIND, (SourceLoc){ 0 })

#define AST_TEST_INIT(CONTENT, METHOD, ...) \
utest_fixture->fc.content = (byte*)CONTENT; \
utest_fixture->fc.size = strlen((const char*)utest_fixture->fc.content); \
utest_fixture->ts = token_stream_create(16, &utest_fixture->fc, &utest_fixture->rc); \
utest_fixture->ast_parser = ast_parser_create(&utest_fixture->ts, &utest_fixture->rc); \
utest_fixture->ast_node = METHOD(&utest_fixture->ast_parser, ##__VA_ARGS__); \
const ASTNode* node = utest_fixture->ast_node

#define ASSERT_AST_NODE(NODE, KIND) \
ASSERT_NE_MSG(NODE, NULL, "not is null"); \
ASSERT_EQ_MSG(NODE->kind, KIND, "invalid node kind")

#define ASSERT_AST_ERROR(NODE) ASSERT_AST_NODE(NODE, AST_NODE_ERROR)

#define ASSERT_AST_EXPR_BINARY(NODE, OP, LHS_KIND, RHS_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_BINARY); \
ASSERT_EQ_MSG(NODE->as.expr_binary.op, OP, "invalid binary op"); \
ASSERT_AST_NODE(NODE->as.expr_binary.lhs, LHS_KIND); \
ASSERT_AST_NODE(NODE->as.expr_binary.rhs, RHS_KIND)

#define ASSERT_AST_EXPR_PREFIX_UNARY(NODE, OP, RHS_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_PREFIX_UNARY); \
ASSERT_EQ_MSG(NODE->as.expr_prefix_unary.op, OP, "invalid prefix unary op"); \
ASSERT_AST_NODE(NODE->as.expr_prefix_unary.rhs, RHS_KIND)

#define ASSERT_AST_EXPR_POSTFIX_UNARY(NODE, OP, LHS_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_POSTFIX_UNARY); \
ASSERT_EQ_MSG(NODE->as.expr_postfix_unary.op, OP, "invalid postfix unary op"); \
ASSERT_AST_NODE(NODE->as.expr_postfix_unary.lhs, LHS_KIND)

#define ASSERT_AST_EXPR_BRACES(NODE, EXPR_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_BRACES); \
ASSERT_AST_NODE(NODE->as.expr_braces.expr, EXPR_KIND)

#define ASSERT_AST_EXPR_PLACE(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_PLACE); \
ASSERT_STREQ_MSG(NODE->as.expr_place.value.text, VALUE, "invalid expr place value")

#define ASSERT_AST_EXPR_CALL(NODE, CALLEE_KIND, ARGS_COUNT) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_CALL); \
ASSERT_AST_NODE(NODE->as.expr_call.callee, CALLEE_KIND); \
ASSERT_EQ_MSG(NODE->as.expr_call.args.size, ARGS_COUNT, "invalid call args count");

#define ASSERT_AST_EXPR_INDEX(NODE, CALLEE_KIND, ARGS_COUNT) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_INDEX); \
ASSERT_AST_NODE(NODE->as.expr_index.callee, CALLEE_KIND); \
ASSERT_EQ_MSG(NODE->as.expr_index.args.size, ARGS_COUNT, "invalid index args count");

#define ASSERT_AST_EXPR_MEMBER(NODE, OBJECT_KIND, MEMBER_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_MEMBER); \
ASSERT_AST_NODE(NODE->as.expr_member.object, OBJECT_KIND); \
ASSERT_AST_NODE(NODE->as.expr_member.member, MEMBER_KIND)

#define ASSERT_AST_EXPR_LITERAL(NODE, KIND, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_LITERAL); \
ASSERT_EQ_MSG(NODE->as.expr_literal.kind, KIND, "invalid expr literal kind"); \
ASSERT_STREQ_MSG(NODE->as.expr_literal.value.text, VALUE, "invalid expr literal value")

#define ASSERT_AST_EXPR_LITERAL_DEC(NODE, VALUE)    ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_DEC, VALUE)
#define ASSERT_AST_EXPR_LITERAL_HEX(NODE, VALUE)    ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_HEX, VALUE)
#define ASSERT_AST_EXPR_LITERAL_BIN(NODE, VALUE)    ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_BIN, VALUE)
#define ASSERT_AST_EXPR_LITERAL_BOOL(NODE, VALUE)   ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_BOOL, VALUE)
#define ASSERT_AST_EXPR_LITERAL_CHAR(NODE, VALUE)   ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_CHAR, VALUE)
#define ASSERT_AST_EXPR_LITERAL_STRING(NODE, VALUE) ASSERT_AST_EXPR_LITERAL(NODE, TOKEN_LITERAL_STRING, VALUE)