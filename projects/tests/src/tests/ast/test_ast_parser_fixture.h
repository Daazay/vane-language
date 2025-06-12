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

/*...............................MISC...............................*/

#define ASSERT_AST_ID(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_IDENTIFIER); \
ASSERT_STREQ_MSG(NODE->as.id.value.text, VALUE, "invalid id value")

/*...............................TYPE...............................*/

#define ASSERT_AST_TYPE_BUILTIN(NODE, TYPE_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_BUILTIN); \
ASSERT_EQ_MSG(NODE->as.type_builtin.kind, TYPE_KIND, "invalid type builtin kind")

#define ASSERT_AST_TYPE_CUSTOM(NODE, VALUE) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_CUSTOM); \
ASSERT_STREQ_MSG(NODE->as.type_custom.value.text, VALUE, "invalid type custom value")

#define ASSERT_AST_TYPE_PTR(NODE, TYPE_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_PTR); \
ASSERT_AST_NODE(NODE->as.type_ptr.type, TYPE_KIND)

#define ASSERT_AST_TYPE_ARR(NODE, SIZE_EXPR_KIND, TYPE_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_TYPE_ARR); \
if (SIZE_EXPR_KIND != AST_NODE_UNKNOWN) { \
    ASSERT_AST_NODE(NODE->as.type_arr.size_expr, SIZE_EXPR_KIND); \
} else { \
    ASSERT_EQ_MSG(NODE->as.type_arr.size_expr, NULL, "invalid size_expr in type arr. Must be NULL"); \
} \
ASSERT_AST_NODE(NODE->as.type_arr.type, TYPE_KIND)

/*...............................STMT...............................*/

#define ASSERT_AST_FUN_PARAM(NODE, ID_VALUE, TYPE_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_FUN_PARAM); \
if (TYPE_KIND != AST_NODE_UNKNOWN) { \
    ASSERT_AST_NODE(NODE->as.fun_param.type, TYPE_KIND); \
} else { \
    ASSERT_EQ_MSG(NODE->as.fun_param.type, NULL, "invalid type in fun param. Must be NULL"); \
} \
ASSERT_AST_ID(NODE->as.fun_param.id, ID_VALUE)

#define ASSERT_AST_FUN_SIGN(NODE, ID_VALUE, TYPE_KIND, PARAM_COUNT) \
ASSERT_AST_NODE(NODE, AST_NODE_FUN_SIGN); \
if (TYPE_KIND != AST_NODE_UNKNOWN) { \
    ASSERT_AST_NODE(NODE->as.fun_sign.type, TYPE_KIND); \
} else { \
    ASSERT_EQ_MSG(NODE->as.fun_sign.type, NULL, "invalid type in fun sign. Must be NULL"); \
} \
ASSERT_AST_ID(NODE->as.fun_sign.id, ID_VALUE); \
ASSERT_EQ_MSG(NODE->as.fun_sign.params.size, PARAM_COUNT, "invalid fun sign params size")

#define ASSERT_AST_FUN_DECL(NODE, SIGN_ID_VALUE, SIGN_TYPE_KIND, SIGN_PARAM_COUNT, BLOCK_SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_FUN_DECL); \
ASSERT_AST_FUN_SIGN(NODE->as.fun_decl.sign, SIGN_ID_VALUE, SIGN_TYPE_KIND, SIGN_PARAM_COUNT); \
ASSERT_EQ_MSG(NODE->as.fun_decl.block.size, BLOCK_SIZE, "invalid fun decl block size")


/*...............................STMT...............................*/

#define ASSERT_AST_STMT_EMPTY(NODE) ASSERT_AST_NODE(NODE, AST_NODE_STMT_EMPTY)

#define ASSERT_AST_STMT_BLOCK(NODE, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_BLOCK); \
ASSERT_EQ_MSG(NODE->as.stmt_block.block.size, SIZE, "invalid stmt block size")

#define ASSERT_AST_STMT_VAR_ITEM(NODE, ID_VALUE, TYPE_KIND, EXPR_KIND) do {\
ASSERT_AST_NODE(NODE, AST_NODE_STMT_VAR_ITEM); \
ASSERT_AST_ID(NODE->as.stmt_var_item.id, ID_VALUE); \
    if (TYPE_KIND != AST_NODE_UNKNOWN) { \
        ASSERT_AST_NODE(NODE->as.stmt_var_item.type, TYPE_KIND); \
    } else { \
        ASSERT_EQ_MSG(NODE->as.stmt_var_item.type, NULL, "invalid type in stmt var item. Must be NULL"); \
    } \
    if (EXPR_KIND != AST_NODE_UNKNOWN) { \
        ASSERT_AST_NODE(NODE->as.stmt_var_item.expr, EXPR_KIND); \
    } else { \
        ASSERT_EQ_MSG(NODE->as.stmt_var_item.expr, NULL, "invalid expr in stmt var item. Must be NULL"); \
    } \
} while (false)

#define ASSERT_AST_STMT_VAR_DECL(NODE, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_VAR_DECL); \
ASSERT_EQ_MSG(NODE->as.stmt_var_decl.items.size, SIZE, "invalid stmt var decl items size")

#define ASSERT_AST_STMT_BRANCH(NODE, EXPR_KIND, BLOCK_SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_BRANCH); \
if (EXPR_KIND != AST_NODE_UNKNOWN) { \
    ASSERT_AST_NODE(NODE->as.stmt_branch.expr, EXPR_KIND); \
} else { \
    ASSERT_EQ_MSG(NODE->as.stmt_branch.expr, NULL, "invalid expr in stmt branch. Must be NULL"); \
} \
ASSERT_EQ_MSG(NODE->as.stmt_branch.block.size, BLOCK_SIZE, "invalid stmt branch block size")

#define ASSERT_AST_STMT_CONDITION(NODE, SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_CONDITION); \
ASSERT_EQ_MSG(NODE->as.stmt_condition.branches.size, SIZE, "invalid stmt condition items size")

#define ASSERT_AST_STMT_WHILE(NODE, EXPR_KIND, BLOCK_SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_WHILE); \
 ASSERT_AST_NODE(NODE->as.stmt_while.expr, EXPR_KIND); \
ASSERT_EQ_MSG(NODE->as.stmt_while.block.size, BLOCK_SIZE, "invalid stmt while block size")

#define ASSERT_AST_STMT_DO(NODE, EXPR_KIND, BLOCK_SIZE) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_DO); \
ASSERT_AST_NODE(NODE->as.stmt_do.expr, EXPR_KIND); \
ASSERT_EQ_MSG(NODE->as.stmt_do.block.size, BLOCK_SIZE, "invalid stmt do block size")

#define ASSERT_AST_STMT_BREAK(NODE) ASSERT_AST_NODE(NODE, AST_NODE_STMT_BREAK)

#define ASSERT_AST_STMT_CONTINUE(NODE) ASSERT_AST_NODE(NODE, AST_NODE_STMT_CONTINUE)

#define ASSERT_AST_STMT_RETURN(NODE, EXPR_KIND) do { \
    ASSERT_AST_NODE(NODE, AST_NODE_STMT_RETURN); \
    if (EXPR_KIND != AST_NODE_UNKNOWN) { \
        ASSERT_AST_NODE(NODE->as.stmt_return.expr, EXPR_KIND); \
    } else { \
        ASSERT_EQ_MSG(NODE->as.stmt_return.expr, NULL, "invalid expr in stmt return. Must be NULL"); \
    } \
} while (false)

#define ASSERT_AST_STMT_EXPR(NODE, EXPR_KIND) \
ASSERT_AST_NODE(NODE, AST_NODE_STMT_EXPR); \
ASSERT_AST_NODE(NODE->as.stmt_expr.expr, EXPR_KIND)

/*...............................EXPR...............................*/

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