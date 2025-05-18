#pragma once

#include <utest/utest.h>

#include <vane/ast/ast_parser.h>

struct TestASTParser {
    String path;
    String content;
    TokenStream ts;
    ASTParser ast_parser;
    ASTNode* ast_node;
};

UTEST_F_SETUP(TestASTParser) {
    utest_fixture->path = STRING_EMPTY;
    utest_fixture->content = STRING_EMPTY;
    utest_fixture->ts = (TokenStream){ 0 };
    utest_fixture->ast_parser = (ASTParser){ 0 };
    utest_fixture->ast_node = NULL;
}

UTEST_F_TEARDOWN(TestASTParser) {
    string_destroy(&utest_fixture->path);
    string_destroy(&utest_fixture->content);
    ts_destroy(&utest_fixture->ts);
    ast_parser_destroy(&utest_fixture->ast_parser);
    ast_node_destroy(utest_fixture->ast_node);
}

#define AST_TEST_INIT(CONTENT, METHOD, ...) \
utest_fixture->content = string_from_cstr(CONTENT); \
utest_fixture->ts = ts_create(32, &utest_fixture->path, (byte*)utest_fixture->content.text, utest_fixture->content.len); \
utest_fixture->ast_parser = ast_parser_create(&utest_fixture->ts); \
utest_fixture->ast_node = METHOD(&utest_fixture->ast_parser, ##__VA_ARGS__); \
const ASTNode* node = utest_fixture->ast_node

//

#define ASSERT_AST_NODE(NODE, KIND) \
ASSERT_NE_MSG(NODE, NULL, #KIND " is equal to NULL"); \
ASSERT_EQ_MSG(NODE->kind, KIND, "invalid node kind.")

// MISC

#define ASSERT_AST_ID(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_IDENTIFIER); \
ASSERT_STREQ_MSG(NODE->as.identifier.value.text, VALUE, "invalid id value")

// TYPE

// ::BASIC

#define ASSERT_AST_TYPE_VOID(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_VOID)
#define ASSERT_AST_TYPE_ANY(NODE)  ASSERT_AST_NODE(NODE, AST_NODE_TYPE_ANY)


#define ASSERT_AST_TYPE_U8(NODE)  ASSERT_AST_NODE(NODE, AST_NODE_TYPE_U8)
#define ASSERT_AST_TYPE_I8(NODE)  ASSERT_AST_NODE(NODE, AST_NODE_TYPE_I8)
#define ASSERT_AST_TYPE_U16(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_U16)
#define ASSERT_AST_TYPE_I16(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_I16)
#define ASSERT_AST_TYPE_U32(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_U32)
#define ASSERT_AST_TYPE_I32(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_I32)
#define ASSERT_AST_TYPE_U64(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_U64)
#define ASSERT_AST_TYPE_I64(NODE) ASSERT_AST_NODE(NODE, AST_NODE_TYPE_I64)

#define ASSERT_AST_TYPE_CUSTOM(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_CUSTOM); \
ASSERT_STREQ_MSG(NODE->as.type_custom.value.text, VALUE, "invalid type custom value")

// ::PTR

#define ASSERT_AST_TYPE_PTR(NODE, PTR_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_PTR); \
ASSERT_AST_NODE(NODE->as.type_ptr.type, PTR_KIND)

// ::ARR

#define ASSERT_AST_TYPE_ARR(NODE, ARR_KIND, EXPR_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_ARR); \
ASSERT_AST_NODE(NODE->as.type_arr.type, ARR_KIND); \
do { \
    if (EXPR_KIND != AST_NODE_UNKNOWN) { \
        ASSERT_AST_NODE(NODE->as.type_arr.expr, EXPR_KIND); \
    } \
} while(false)

// EXPR

// ::LITERAL

#define ASSERT_AST_EXPR_STRING(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_STRING); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal string value")

#define ASSERT_AST_EXPR_CHAR(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_CHAR); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal char value")

#define ASSERT_AST_EXPR_DEC(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_DEC); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal dec value")

#define ASSERT_AST_EXPR_HEX(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_HEX); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal hex value")

#define ASSERT_AST_EXPR_BIN(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_BIN); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal bin value")

#define ASSERT_AST_EXPR_BOOL(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_LITERAL_BOOL); \
ASSERT_STREQ_MSG(NODE->as.literal.value.text, VALUE, "invalid literal bool value")

// ::NUD

#define ASSERT_AST_EXPR_PLACE(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_PLACE); \
ASSERT_STREQ_MSG(NODE->as.expr_place.value.text, VALUE, "invalid expr place value")

#define ASSERT_AST_EXPR_NIL(NODE) ASSERT_AST_NODE(NODE, AST_NODE_EXPR_NIL)

#define ASSERT_AST_EXPR_PREFIX_UNARY(NODE, OP, KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_UNARY); \
ASSERT_EQ_MSG(NODE->as.expr_unary.op, OP, "invalid expr unary on"); \
ASSERT_AST_NODE(NODE->as.expr_unary.rhs, KIND); \
ASSERT_EQ_MSG(NODE->as.expr_unary.lhs, NULL, "expr unary lhs must be NULL")

#define ASSERT_AST_EXPR_BRACES(NODE, KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_BRACES); \
ASSERT_AST_NODE(NODE->as.expr_braces.expr, KIND)

#define ASSERT_AST_EXPR_INIT_LIST(NODE, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_INIT_LIST); \
ASSERT_EQ_MSG(NODE->as.expr_init_list.items.size, SIZE, "invalid expr init list size")

#define ASSERT_AST_EXPR_CAST(NODE, TYPE_KIND, EXPR_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_CAST); \
ASSERT_AST_NODE(NODE->as.expr_cast.type, TYPE_KIND); \
ASSERT_AST_NODE(NODE->as.expr_cast.expr, EXPR_KIND)

// ::LED

#define ASSERT_AST_EXPR_CALL(NODE, CALLEE_KIND, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_CALL); \
ASSERT_AST_NODE(NODE->as.expr_call.callee, CALLEE_KIND); \
ASSERT_EQ_MSG(NODE->as.expr_call.args.size, SIZE, "invalid expr call args size")

#define ASSERT_AST_EXPR_INDEX(NODE, CALLEE_KIND, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_INDEX); \
ASSERT_AST_NODE(NODE->as.expr_index.callee, CALLEE_KIND); \
ASSERT_EQ_MSG(NODE->as.expr_index.args.size, SIZE, "invalid expr index args size")

#define ASSERT_AST_EXPR_MEMBER(NODE, OBJ_KIND, MEM_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_MEMBER); \
ASSERT_AST_NODE(NODE->as.expr_member.object, OBJ_KIND); \
ASSERT_AST_NODE(NODE->as.expr_member.member, MEM_KIND)

#define ASSERT_AST_EXPR_INFIX_UNARY(NODE, OP, KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_UNARY); \
ASSERT_EQ_MSG(NODE->as.expr_unary.op, OP); \
ASSERT_AST_NODE(NODE->as.expr_unary.lhs, KIND); \
ASSERT_EQ_MSG(NODE->as.expr_unary.rhs, NULL, "expr unary rhs must be NULL")

#define ASSERT_AST_EXPR_BINARY(NODE, OP, LHS_KIND, RHS_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_EXPR_BINARY); \
ASSERT_EQ_MSG(NODE->as.expr_binary.op, OP); \
ASSERT_AST_NODE(NODE->as.expr_binary.lhs, LHS_KIND); \
ASSERT_AST_NODE(NODE->as.expr_binary.rhs, RHS_KIND)