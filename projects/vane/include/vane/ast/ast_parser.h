#pragma once

#include "vane/utils/defines.h"
#include "vane/utils/string.h"
#include "vane/utils/vector.h"

#include "vane/scanner/token_stream.h"

#include "vane/diagnostic/report_collector.h"

#include "vane/ast/ast_node.h"

typedef struct ASTParser ASTParser;

struct ASTParser {
    TokenStream* ts;
    ReportCollector* rc;
};

ASTParser ast_parser_create(TokenStream* ts, ReportCollector* rc);

void ast_parser_destroy(ASTParser* ast_parser);


/*...............................MISC...............................*/

ASTNode* ast_parser_parse_identifier(ASTParser* ast_parser);

/*...............................TYPE...............................*/

ASTNode* ast_parser_parse_type(ASTParser* ast_parser);

ASTNode* ast_parser_parse_type_builtin(ASTParser* ast_parser);

ASTNode* ast_parser_parse_type_custom(ASTParser* ast_parser);

ASTNode* ast_parser_parse_type_ptr(ASTParser* ast_parser);

ASTNode* ast_parser_parse_type_arr(ASTParser* ast_parser);

/*...............................PACKAGE............................*/

ASTNode* ast_parser_parse_package_decl(ASTParser* ast_parser);

ASTNode* ast_parser_parse_import_decl(ASTParser* ast_parser);

ASTNode* ast_parser_parse_typealias_decl(ASTParser* ast_parser);

/*...............................FUN................................*/

ASTNode* ast_parser_parse_fun_param(ASTParser* ast_parser);

ASTNode* ast_parser_parse_fun_sign(ASTParser* ast_parser);

ASTNode* ast_parser_parse_fun_decl(ASTParser* ast_parser);

/*...............................STMT...............................*/

ASTNode* ast_parser_parse_stmt(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_block(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_var_item(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_var_decl(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_branch(ASTParser* ast_parser, bool start_with_else);

ASTNode* ast_parser_parse_stmt_condition(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_while(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_do(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_break(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_continue(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_return(ASTParser* ast_parser);

ASTNode* ast_parser_parse_stmt_expr(ASTParser* ast_parser);

/*...............................EXPR...............................*/

ASTNode* ast_parser_parse_expr(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_with_prec(ASTParser* ast_parser, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_nud(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_led(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_binary(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_prefix_unary(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_postfix_unary(ASTParser* ast_parser, ASTNode* lhs);

ASTNode* ast_parser_parse_expr_braces(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_place(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_call(ASTParser* ast_parser, ASTNode* callee);

ASTNode* ast_parser_parse_expr_index(ASTParser* ast_parser, ASTNode* callee);

ASTNode* ast_parser_parse_expr_member(ASTParser* ast_parser, ASTNode* object);

ASTNode* ast_parser_parse_expr_literal(ASTParser* ast_parser);