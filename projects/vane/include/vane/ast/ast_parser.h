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

ASTNode* ast_parser_parse_identifier(ASTParser* ast_parser);

ASTNode* ast_parser_parse_typeref(ASTParser* ast_parser);

ASTNode* ast_parser_parse_typeref_basic(ASTParser* ast_parser);

ASTNode* ast_parser_parse_typeref_ptr(ASTParser* ast_parser);

ASTNode* ast_parser_parse_typeref_arr(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_with_prec(ASTParser* ast_parser, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_nud(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_prefix_unary(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_place(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_braces(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_init_list(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_nil(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_literal(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_cast(ASTParser* ast_parser);

ASTNode* ast_parser_parse_expr_led(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_binary(ASTParser* ast_parser, ASTNode* lhs, OpPrecedence prec);

ASTNode* ast_parser_parse_expr_postfix_unary(ASTParser* ast_parser, ASTNode* lhs);

ASTNode* ast_parser_parse_expr_call(ASTParser* ast_parser, ASTNode* lhs);

ASTNode* ast_parser_parse_expr_index(ASTParser* ast_parser, ASTNode* lhs);

ASTNode* ast_parser_parse_expr_member(ASTParser* ast_parser, ASTNode* lhs);