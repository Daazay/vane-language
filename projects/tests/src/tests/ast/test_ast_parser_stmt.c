#include "test_ast_parser_fixture.h"

// BLOCK

UTEST_F(TestASTParser, stmt_block1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_block);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_block2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_block);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_block3) {
    AST_TEST_INIT("begin", ast_parser_parse_stmt_block);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_block4) {
    AST_TEST_INIT("begin end", ast_parser_parse_stmt_block);
    ASSERT_AST_STMT_BLOCK(node, 0);
}

UTEST_F(TestASTParser, stmt_block5) {
    AST_TEST_INIT(
        "begin\n"
        "   ;"
        "end",
        ast_parser_parse_stmt_block
    );
    ASSERT_AST_STMT_BLOCK(node, 1);
    const ASTNode* stmt = vector_at(&node->as.stmt_block.block, 0);
    ASSERT_AST_STMT_EMPTY(stmt);
}

UTEST_F(TestASTParser, stmt_block6) {
    AST_TEST_INIT(
        "begin\n"
        "   begin\n"
        "       a + b\n"
        "   end\n"
        "end",
        ast_parser_parse_stmt_block
    );
    ASSERT_AST_STMT_BLOCK(node, 1);
    const ASTNode* inner_block = vector_at(&node->as.stmt_block.block, 0);
    //
    ASSERT_AST_STMT_BLOCK(inner_block, 1);
    const ASTNode* stmt = vector_at(&inner_block->as.stmt_block.block, 0);
    //
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_BINARY);
    ASSERT_AST_EXPR_BINARY(stmt->as.stmt_expr.expr, TOKEN_PLUS, AST_NODE_EXPR_PLACE, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr->as.expr_binary.lhs, "a");
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr->as.expr_binary.rhs, "b");
}

UTEST_F(TestASTParser, stmt_block7) {
    AST_TEST_INIT(
        "begin\n"
        "   begin\n"
        "       a\n"
        "       b\n"
        "   end\n"
        "end",
        ast_parser_parse_stmt_block
    );
    ASSERT_AST_STMT_BLOCK(node, 1);
    const ASTNode* inner_block = vector_at(&node->as.stmt_block.block, 0);
    //
    ASSERT_AST_STMT_BLOCK(inner_block, 2);
    //
    const ASTNode* stmt1 = vector_at(&inner_block->as.stmt_block.block, 0);
    ASSERT_AST_STMT_EXPR(stmt1, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt1->as.stmt_expr.expr, "a");
    //
    const ASTNode* stmt2 = vector_at(&inner_block->as.stmt_block.block, 1);
    ASSERT_AST_STMT_EXPR(stmt2, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt2->as.stmt_expr.expr, "b");
}

// VAR ITEM

UTEST_F(TestASTParser, stmt_var_item1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_var_item);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_item2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_var_item);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_item3) {
    AST_TEST_INIT("a", ast_parser_parse_stmt_var_item);
    ASSERT_AST_STMT_VAR_ITEM(node, "a", AST_NODE_UNKNOWN, AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, stmt_var_item4) {
    AST_TEST_INIT("a:", ast_parser_parse_stmt_var_item);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_item5) {
    AST_TEST_INIT("a: u8", ast_parser_parse_stmt_var_item);
    ASSERT_AST_STMT_VAR_ITEM(node, "a", AST_NODE_TYPE_BUILTIN, AST_NODE_UNKNOWN);
    ASSERT_AST_TYPE_BUILTIN(node->as.stmt_var_item.type, TOKEN_KEYWORD_U8);
}

UTEST_F(TestASTParser, stmt_var_item6) {
    AST_TEST_INIT("a = ", ast_parser_parse_stmt_var_item);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_item7) {
    AST_TEST_INIT("a = 1", ast_parser_parse_stmt_var_item);
    ASSERT_AST_STMT_VAR_ITEM(node, "a", AST_NODE_UNKNOWN, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_var_item.expr, "1");
}

UTEST_F(TestASTParser, stmt_var_item8) {
    AST_TEST_INIT("a: u8 = 1", ast_parser_parse_stmt_var_item);
    ASSERT_AST_STMT_VAR_ITEM(node, "a", AST_NODE_TYPE_BUILTIN, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_TYPE_BUILTIN(node->as.stmt_var_item.type, TOKEN_KEYWORD_U8);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_var_item.expr, "1");
}

// VAR DECL

UTEST_F(TestASTParser, stmt_var_decl1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_decl2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_decl3) {
    AST_TEST_INIT("var", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_decl4) {
    AST_TEST_INIT("var a", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_STMT_VAR_DECL(node, 1);
    //
    const ASTNode* var = vector_at(&node->as.stmt_var_decl.items, 0);
    ASSERT_AST_STMT_VAR_ITEM(var, "a", AST_NODE_UNKNOWN, AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, stmt_var_decl5) {
    AST_TEST_INIT("var a:", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_decl6) {
    AST_TEST_INIT("var a: u8", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_STMT_VAR_DECL(node, 1);
    //
    const ASTNode* var = vector_at(&node->as.stmt_var_decl.items, 0);
    ASSERT_AST_STMT_VAR_ITEM(var, "a", AST_NODE_TYPE_BUILTIN, AST_NODE_UNKNOWN);
    ASSERT_AST_TYPE_BUILTIN(var->as.stmt_var_item.type, TOKEN_KEYWORD_U8);
}

UTEST_F(TestASTParser, stmt_var_decl7) {
    AST_TEST_INIT("var a = ", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_var_decl8) {
    AST_TEST_INIT("var a = 1", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_STMT_VAR_DECL(node, 1);
    //
    const ASTNode* var = vector_at(&node->as.stmt_var_decl.items, 0);
    ASSERT_AST_STMT_VAR_ITEM(var, "a", AST_NODE_UNKNOWN, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(var->as.stmt_var_item.expr, "1");
}

UTEST_F(TestASTParser, stmt_var_decl9) {
    AST_TEST_INIT("var a: u8 = 1", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_STMT_VAR_DECL(node, 1);
    //
    const ASTNode* var = vector_at(&node->as.stmt_var_decl.items, 0);
    ASSERT_AST_STMT_VAR_ITEM(var, "a", AST_NODE_TYPE_BUILTIN, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_TYPE_BUILTIN(var->as.stmt_var_item.type, TOKEN_KEYWORD_U8);
    ASSERT_AST_EXPR_LITERAL_DEC(var->as.stmt_var_item.expr, "1");
}

UTEST_F(TestASTParser, stmt_var_decl10) {
    AST_TEST_INIT("var a, b", ast_parser_parse_stmt_var_decl);
    ASSERT_AST_STMT_VAR_DECL(node, 2);
    //
    const ASTNode* var1 = vector_at(&node->as.stmt_var_decl.items, 0);
    ASSERT_AST_STMT_VAR_ITEM(var1, "a", AST_NODE_UNKNOWN, AST_NODE_UNKNOWN);
    //
    const ASTNode* var2 = vector_at(&node->as.stmt_var_decl.items, 1);
    ASSERT_AST_STMT_VAR_ITEM(var2, "b", AST_NODE_UNKNOWN, AST_NODE_UNKNOWN);
}

// BRANCH

UTEST_F(TestASTParser, stmt_branch1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_branch, false);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_branch, false);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch3) {
    AST_TEST_INIT("if", ast_parser_parse_stmt_branch, false);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch4) {
    AST_TEST_INIT("if true", ast_parser_parse_stmt_branch, false);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch5) {
    AST_TEST_INIT("if true then", ast_parser_parse_stmt_branch, false);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch6) {
    AST_TEST_INIT(
        "if true then\n"
        "end",
        ast_parser_parse_stmt_branch, false
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 0);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
}

UTEST_F(TestASTParser, stmt_branch7) {
    AST_TEST_INIT(
        "if true then\n"
        "else",
        ast_parser_parse_stmt_branch, false
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 0);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
}

UTEST_F(TestASTParser, stmt_branch8) {
    AST_TEST_INIT(
        "if true then\n"
        "   a\n"
        "end",
        ast_parser_parse_stmt_branch, false
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 1);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

UTEST_F(TestASTParser, stmt_branch9) {
    AST_TEST_INIT(
        "else if true then\n"
        "end",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 0);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
}

UTEST_F(TestASTParser, stmt_branch10) {
    AST_TEST_INIT(
        "else if true then\n"
        "   a\n"
        "end",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 1);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

UTEST_F(TestASTParser, stmt_branch11) {
    AST_TEST_INIT(
        "else if true then\n"
        "else",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 0);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
}

UTEST_F(TestASTParser, stmt_branch12) {
    AST_TEST_INIT(
        "else if true then\n"
        "   a\n"
        "else",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_EXPR_LITERAL, 1);
    //
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

UTEST_F(TestASTParser, stmt_branch13) {
    AST_TEST_INIT("else",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_branch14) {
    AST_TEST_INIT(
        "else\n"
        "end",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_UNKNOWN, 0);
}

UTEST_F(TestASTParser, stmt_branch15) {
    AST_TEST_INIT(
        "else\n"
        "   a\n"
        "end",
        ast_parser_parse_stmt_branch, true
    );
    ASSERT_AST_STMT_BRANCH(node, AST_NODE_UNKNOWN, 1);
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

//CONDITION

UTEST_F(TestASTParser, stmt_condition1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_condition);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_condition2) {
    AST_TEST_INIT(
        "if true then\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_STMT_CONDITION(node, 1);
    //
    const ASTNode* br = vector_at(&node->as.stmt_condition.branches, 0);
    //
    ASSERT_AST_STMT_BRANCH(br, AST_NODE_EXPR_LITERAL, 0);
    ASSERT_AST_EXPR_LITERAL_BOOL(br->as.stmt_branch.expr, "true");
}

UTEST_F(TestASTParser, stmt_condition3) {
    AST_TEST_INIT(
        "if true then\n"
        "   a\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_STMT_CONDITION(node, 1);
    //
    const ASTNode* br = vector_at(&node->as.stmt_condition.branches, 0);
    //
    ASSERT_AST_STMT_BRANCH(br, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(br->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&br->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

UTEST_F(TestASTParser, stmt_condition4) {
    AST_TEST_INIT(
        "if true then\n"
        "   a\n"
        "else\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_STMT_CONDITION(node, 2);
    //
    const ASTNode* br1 = vector_at(&node->as.stmt_condition.branches, 0);
    //
    ASSERT_AST_STMT_BRANCH(br1, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(br1->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&br1->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
    //
    const ASTNode* br2 = vector_at(&node->as.stmt_condition.branches, 1);
    //
    ASSERT_AST_STMT_BRANCH(br2, AST_NODE_UNKNOWN, 0);
}

UTEST_F(TestASTParser, stmt_condition5) {
    AST_TEST_INIT(
        "if true then\n"
        "   a\n"
        "else\n"
        "   b\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_STMT_CONDITION(node, 2);
    //
    const ASTNode* br1 = vector_at(&node->as.stmt_condition.branches, 0);
    //
    ASSERT_AST_STMT_BRANCH(br1, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(br1->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&br1->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
    //
    const ASTNode* br2 = vector_at(&node->as.stmt_condition.branches, 1);
    //
    ASSERT_AST_STMT_BRANCH(br2, AST_NODE_UNKNOWN, 1);
    //
    const ASTNode* stmt2 = vector_at(&br2->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt2, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt2->as.stmt_expr.expr, "b");
}

UTEST_F(TestASTParser, stmt_condition6) {
    AST_TEST_INIT(
        "if true then\n"
        "   a\n"
        "else if false then\n"
        "   b\n"
        "else\n"
        "   c\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_STMT_CONDITION(node, 3);
    //
    const ASTNode* br1 = vector_at(&node->as.stmt_condition.branches, 0);
    //
    ASSERT_AST_STMT_BRANCH(br1, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(br1->as.stmt_branch.expr, "true");
    //
    const ASTNode* stmt = vector_at(&br1->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
    //
    const ASTNode* br2 = vector_at(&node->as.stmt_condition.branches, 1);
    //
    ASSERT_AST_STMT_BRANCH(br2, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(br2->as.stmt_branch.expr, "false");
    //
    const ASTNode* stmt2 = vector_at(&br2->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt2, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt2->as.stmt_expr.expr, "b");
    //
    const ASTNode* br3 = vector_at(&node->as.stmt_condition.branches, 2);
    //
    ASSERT_AST_STMT_BRANCH(br3, AST_NODE_UNKNOWN, 1);
    //
    const ASTNode* stmt3 = vector_at(&br3->as.stmt_branch.block, 0);
    ASSERT_AST_STMT_EXPR(stmt3, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt3->as.stmt_expr.expr, "c");
}

UTEST_F(TestASTParser, stmt_condition7) {
    AST_TEST_INIT(
        "if true then\n"
        "else\n"
        "else if false\n"
        "end\n"
        , ast_parser_parse_stmt_condition
    );
    ASSERT_AST_ERROR(node);
}

// WHILE

UTEST_F(TestASTParser, stmt_while1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_while);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_while2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_while);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_while3) {
    AST_TEST_INIT("while", ast_parser_parse_stmt_while);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_while4) {
    AST_TEST_INIT("while true", ast_parser_parse_stmt_while);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_while5) {
    AST_TEST_INIT("while true do", ast_parser_parse_stmt_while);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_while6) {
    AST_TEST_INIT(
        "while true do\n"
        "end"
    , ast_parser_parse_stmt_while);
    ASSERT_AST_STMT_WHILE(node, AST_NODE_EXPR_LITERAL, 0);
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_while.expr, "true");
}

UTEST_F(TestASTParser, stmt_while7) {
    AST_TEST_INIT(
        "while true do\n"
        "   a\n"
        "end"
        , ast_parser_parse_stmt_while);
    ASSERT_AST_STMT_WHILE(node, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_while.expr, "true");
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_while.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}


// DO

UTEST_F(TestASTParser, stmt_do1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_do);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_do2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_do);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_do3) {
    AST_TEST_INIT("do", ast_parser_parse_stmt_do);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_do4) {
    AST_TEST_INIT("do loop", ast_parser_parse_stmt_do);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_do5) {
    AST_TEST_INIT("do loop while", ast_parser_parse_stmt_do);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_do6) {
    AST_TEST_INIT(
        "do\n"
        "loop while true"
        , ast_parser_parse_stmt_do);
    ASSERT_AST_STMT_DO(node, AST_NODE_EXPR_LITERAL, 0);
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_do.expr, "true");
}

UTEST_F(TestASTParser, stmt_do7) {
    AST_TEST_INIT(
        "do\n"
        "   a\n"
        "loop while true"
        , ast_parser_parse_stmt_do);
    ASSERT_AST_STMT_DO(node, AST_NODE_EXPR_LITERAL, 1);
    ASSERT_AST_EXPR_LITERAL_BOOL(node->as.stmt_do.expr, "true");
    //
    const ASTNode* stmt = vector_at(&node->as.stmt_do.block, 0);
    ASSERT_AST_STMT_EXPR(stmt, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(stmt->as.stmt_expr.expr, "a");
}

// BREAK

UTEST_F(TestASTParser, stmt_break1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_break);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_break2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_break);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_break3) {
    AST_TEST_INIT("break", ast_parser_parse_stmt_break);
    ASSERT_AST_STMT_BREAK(node);
}

// CONTINUE

UTEST_F(TestASTParser, stmt_continue1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_continue);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_continue2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_continue);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_continue3) {
    AST_TEST_INIT("continue", ast_parser_parse_stmt_continue);
    ASSERT_AST_STMT_CONTINUE(node);
}

// RETURN

UTEST_F(TestASTParser, stmt_return1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_return);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_return2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_return);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_return3) {
    AST_TEST_INIT("return", ast_parser_parse_stmt_return);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_return4) {
    AST_TEST_INIT("return;", ast_parser_parse_stmt_return);
    ASSERT_AST_STMT_RETURN(node, AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, stmt_return5) {
    AST_TEST_INIT("return \n6", ast_parser_parse_stmt_return);
    ASSERT_AST_STMT_RETURN(node, AST_NODE_UNKNOWN);
}

UTEST_F(TestASTParser, stmt_return6) {
    AST_TEST_INIT("return 0", ast_parser_parse_stmt_return);
    ASSERT_AST_STMT_RETURN(node, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_return.expr, "0");
}

UTEST_F(TestASTParser, stmt_return7) {
    AST_TEST_INIT("return 1 + 2", ast_parser_parse_stmt_return);
    ASSERT_AST_STMT_RETURN(node, AST_NODE_EXPR_BINARY);
    ASSERT_AST_EXPR_BINARY(node->as.stmt_return.expr, TOKEN_PLUS, AST_NODE_EXPR_LITERAL, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_return.expr->as.expr_binary.lhs, "1");
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_return.expr->as.expr_binary.rhs, "2");
}

// RETURN

UTEST_F(TestASTParser, stmt_expr1) {
    AST_TEST_INIT("", ast_parser_parse_stmt_expr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_expr2) {
    AST_TEST_INIT("+", ast_parser_parse_stmt_expr);
    ASSERT_AST_ERROR(node);
}

UTEST_F(TestASTParser, stmt_expr3) {
    AST_TEST_INIT("1", ast_parser_parse_stmt_expr);
    ASSERT_AST_STMT_EXPR(node, AST_NODE_EXPR_LITERAL);
    ASSERT_AST_EXPR_LITERAL_DEC(node->as.stmt_expr.expr, "1");
}

UTEST_F(TestASTParser, stmt_expr4) {
    AST_TEST_INIT("a + b", ast_parser_parse_stmt_expr);
    ASSERT_AST_STMT_EXPR(node, AST_NODE_EXPR_BINARY);
    ASSERT_AST_EXPR_BINARY(node->as.stmt_expr.expr, TOKEN_PLUS, AST_NODE_EXPR_PLACE, AST_NODE_EXPR_PLACE);
    ASSERT_AST_EXPR_PLACE(node->as.stmt_expr.expr->as.expr_binary.lhs, "a");
    ASSERT_AST_EXPR_PLACE(node->as.stmt_expr.expr->as.expr_binary.rhs, "b");
}