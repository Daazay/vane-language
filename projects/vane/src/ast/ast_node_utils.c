#include "vane/ast/ast_node_utils.h"

static u64 gen_ast_node_id(const ASTNode* node) {
    return (u64)node;
}

static void emit_ast_node(StringBuilder* sb, const ASTNode* node) {
    if (node == NULL) {
        return;
    }

    StringBuilder label_builder = sb_create(32);

    switch (node->kind) {
    case AST_NODE_ERROR:
        break;
    case AST_NODE_IDENTIFIER:
        sb_append_str(&label_builder, &node->as.identifier.value);
        break;
    case AST_NODE_PACKAGE_DECL:
        break;
    case AST_NODE_IMPORT_DECL:
        break;
    case AST_NODE_TYPE_U8:
    case AST_NODE_TYPE_I8:
    case AST_NODE_TYPE_U16:
    case AST_NODE_TYPE_I16:
    case AST_NODE_TYPE_U32:
    case AST_NODE_TYPE_I32:
    case AST_NODE_TYPE_U64:
    case AST_NODE_TYPE_I64:
        break;
    case AST_NODE_TYPE_CUSTOM:
        break;
    case AST_NODE_TYPE_PTR:
        break;
    case AST_NODE_TYPE_ARR:
        break;
    case AST_NODE_FUNC_PARAM:
        break;
    case AST_NODE_FUNC_SIGN:
        break;
    case AST_NODE_FUNC_DECL:
        break;
    case AST_NODE_STMT_EMPTY:
        break;
    case AST_NODE_STMT_BLOCK:
        break;
    case AST_NODE_CONST_ITEM:
        break;
    case AST_NODE_STMT_CONST_DECL:
        break;
    case AST_NODE_VAR_ITEM:
        break;
    case AST_NODE_STMT_VAR_DECL:
        break;
    case AST_NODE_BRANCH:
        break;
    case AST_NODE_STMT_CONDITION:
        break;
    case AST_NODE_STMT_WHILE:
        break;
    case AST_NODE_STMT_DO:
        break;
    case AST_NODE_STMT_BREAK:
        break;
    case AST_NODE_STMT_CONTINUE:
        break;
    case AST_NODE_STMT_DEFER:
        break;
    case AST_NODE_STMT_EXPR:
        break;
    case AST_NODE_STMT_RETURN:
        break;
    case AST_NODE_EXPR_BINARY:
        sb_append_cstr(&label_builder, get_token_kind_value(node->as.expr_binary.op));
        break;
    case AST_NODE_EXPR_PREFIX_UNARY:
        sb_append_cstr(&label_builder, get_token_kind_value(node->as.expr_prefix_unary.op));
        break;
    case AST_NODE_EXPR_POSTFIX_UNARY:
        sb_append_cstr(&label_builder, get_token_kind_value(node->as.expr_postfix_unary.op));
        break;
    case AST_NODE_EXPR_BRACES:
        break;
    case AST_NODE_EXPR_CALL:
        break;
    case AST_NODE_EXPR_INDEX:
        break;
    case AST_NODE_EXPR_CAST:
        break;
    case AST_NODE_EXPR_PLACE:
        sb_append_str(&label_builder, &node->as.expr_place.value);
        break;
    case AST_NODE_EXPR_MEMBER:
        break;
    case AST_NODE_EXPR_INIT_LIST:
        break;
    case AST_NODE_EXPR_NIL:
        break;
    case AST_NODE_LITERAL_STRING:
        sb_append_format(&label_builder, "\"%.*s\"", (i32)node->as.literal.value.len, node->as.literal.value.text);
        break;
    case AST_NODE_LITERAL_CHAR:
        sb_append_format(&label_builder, "\'%.*s\'", (i32)node->as.literal.value.len, node->as.literal.value.text);
        break;
    case AST_NODE_LITERAL_DEC:
        sb_append_str(&label_builder, &node->as.literal.value);
        break;
    case AST_NODE_LITERAL_HEX:
        sb_append_str(&label_builder, &node->as.literal.value);
        break;
    case AST_NODE_LITERAL_BIN:
        sb_append_str(&label_builder, &node->as.literal.value);
        break;
    case AST_NODE_LITERAL_BOOL:
        sb_append_str(&label_builder, &node->as.literal.value);
        break;
    default:
        unreachable();
        break;
    }

    sb_append_format(sb, "  node%llu [label=\"<node_name>%s",
        gen_ast_node_id(node),
        get_ast_node_kind_name(node->kind)
    );

    if (label_builder.len > 0) {
        sb_append_format(sb, "|%.*s", (i32)label_builder.len, label_builder.buf);
    }
    sb_append_format(sb, "|%u:%u\"];\n", node->loc.begin.line, node->loc.begin.column);

    sb_destroy(&label_builder);
}

static void emit_ast_edge(StringBuilder* sb, const ASTNode* from, const ASTNode* to) {
    if (from == NULL || to == NULL) {
        return;
    }

    sb_append_format(sb, "  node%llu -> node%llu;\n",
        gen_ast_node_id(from),
        gen_ast_node_id(to)
    );
}

static void emit_ast_node_dot(StringBuilder* sb, const ASTNode* node) {
    if (node == NULL) {
        return;
    }

    emit_ast_node(sb, node);

#define EMIT_EDGE_AND_NODE(NODE) do { \
    emit_ast_edge(sb, node, NODE); \
    emit_ast_node_dot(sb, NODE); \
} while (false)

//#define EMIT_EDGE_AND_NODE_VEC(VEC) do { \
//    for (u32 i = 0; i < VEC.size; ++i) { \
//        const ASTNode* n = vector_at(&VEC, i); \
//        EMIT_EDGE_AND_NODE(n); \
//    } \
//} while (false)

#define EMIT_EDGE_AND_NODE_VEC(VEC) do { \
    sb_append_format(sb, "  node_args%llu [label=\"args\"];\n", gen_ast_node_id(node)); \
    sb_append_format(sb, "  node%llu -> node_args%llu;\n", \
        gen_ast_node_id(node), \
        gen_ast_node_id(node) \
    ); \
    for (u32 i = 0; i < VEC.size; ++i) { \
        const ASTNode* n = vector_at(&VEC, i); \
        sb_append_format(sb, "  node_args%llu -> node%llu;\n", \
            gen_ast_node_id(node), \
            gen_ast_node_id(n) \
        ); \
        emit_ast_node_dot(sb, n); \
    } \
} while (false)

    switch (node->kind) {
    case AST_NODE_ERROR:
        EMIT_EDGE_AND_NODE(node->as.error.next);
        break;
    case AST_NODE_IDENTIFIER:
        sb_append_cstr(sb, "  //id");
        break;
    case AST_NODE_PACKAGE_DECL:
        EMIT_EDGE_AND_NODE(node->as.package_decl.id);
        break;
    case AST_NODE_IMPORT_DECL:
        EMIT_EDGE_AND_NODE(node->as.import_decl.path);
        EMIT_EDGE_AND_NODE(node->as.import_decl.alias);
        break;
    case AST_NODE_TYPE_U8:
    case AST_NODE_TYPE_I8:
    case AST_NODE_TYPE_U16:
    case AST_NODE_TYPE_I16:
    case AST_NODE_TYPE_U32:
    case AST_NODE_TYPE_I32:
    case AST_NODE_TYPE_U64:
    case AST_NODE_TYPE_I64:
        break;
    case AST_NODE_TYPE_CUSTOM:
        break;
    case AST_NODE_TYPE_PTR:
        EMIT_EDGE_AND_NODE(node->as.type_ptr.type);
        break;
    case AST_NODE_TYPE_ARR:
        EMIT_EDGE_AND_NODE(node->as.type_arr.type);
        EMIT_EDGE_AND_NODE(node->as.type_arr.expr);
        break;
    case AST_NODE_FUNC_PARAM:
        EMIT_EDGE_AND_NODE(node->as.func_param.id);
        EMIT_EDGE_AND_NODE(node->as.func_param.type);
        break;
    case AST_NODE_FUNC_SIGN:
        EMIT_EDGE_AND_NODE(node->as.func_sign.id);
        EMIT_EDGE_AND_NODE_VEC(node->as.func_sign.params);
        EMIT_EDGE_AND_NODE(node->as.func_sign.type);
        break;
    case AST_NODE_FUNC_DECL:
        EMIT_EDGE_AND_NODE(node->as.func_decl.sign);
        EMIT_EDGE_AND_NODE_VEC(node->as.func_decl.body);
        break;
    case AST_NODE_STMT_EMPTY:
        break;
    case AST_NODE_STMT_BLOCK:
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_block.stmts);
        break;
    case AST_NODE_CONST_ITEM:
        EMIT_EDGE_AND_NODE(node->as.const_item.id);
        EMIT_EDGE_AND_NODE(node->as.const_item.type);
        EMIT_EDGE_AND_NODE(node->as.const_item.expr);
        break;
    case AST_NODE_STMT_CONST_DECL:
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_const_decl.items);
        break;
    case AST_NODE_VAR_ITEM:
        EMIT_EDGE_AND_NODE(node->as.var_item.id);
        EMIT_EDGE_AND_NODE(node->as.var_item.type);
        EMIT_EDGE_AND_NODE(node->as.var_item.expr);
        break;
    case AST_NODE_STMT_VAR_DECL:
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_var_decl.items);
        break;
    case AST_NODE_BRANCH:
        EMIT_EDGE_AND_NODE(node->as.branch.expr);
        EMIT_EDGE_AND_NODE_VEC(node->as.branch.block);
        break;
    case AST_NODE_STMT_CONDITION:
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_condition.branches);
        break;
    case AST_NODE_STMT_WHILE:
        EMIT_EDGE_AND_NODE(node->as.stmt_while.expr);
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_while.block);
        break;
    case AST_NODE_STMT_DO:
        EMIT_EDGE_AND_NODE(node->as.stmt_do.expr);
        EMIT_EDGE_AND_NODE_VEC(node->as.stmt_do.block);
        break;
    case AST_NODE_STMT_BREAK:
        break;
    case AST_NODE_STMT_CONTINUE:
        break;
    case AST_NODE_STMT_DEFER:
        EMIT_EDGE_AND_NODE(node->as.stmt_defer.defer);
        break;
    case AST_NODE_STMT_EXPR:
        EMIT_EDGE_AND_NODE(node->as.stmt_expr.expr);
        break;
    case AST_NODE_STMT_RETURN:
        EMIT_EDGE_AND_NODE(node->as.stmt_return.expr);
        break;
    case AST_NODE_EXPR_BINARY:
        EMIT_EDGE_AND_NODE(node->as.expr_binary.lhs);
        EMIT_EDGE_AND_NODE(node->as.expr_binary.rhs);
        break;
    case AST_NODE_EXPR_PREFIX_UNARY:
        EMIT_EDGE_AND_NODE(node->as.expr_prefix_unary.rhs);
        break;
    case AST_NODE_EXPR_POSTFIX_UNARY:
        EMIT_EDGE_AND_NODE(node->as.expr_postfix_unary.lhs);
        break;
    case AST_NODE_EXPR_BRACES:
        EMIT_EDGE_AND_NODE(node->as.expr_braces.expr);
        break;
    case AST_NODE_EXPR_CALL:
        EMIT_EDGE_AND_NODE(node->as.expr_call.callee);
        EMIT_EDGE_AND_NODE_VEC(node->as.expr_call.args);
        break;
    case AST_NODE_EXPR_INDEX:
        EMIT_EDGE_AND_NODE(node->as.expr_index.callee);
        EMIT_EDGE_AND_NODE_VEC(node->as.expr_index.args);
        break;
    case AST_NODE_EXPR_CAST:
        EMIT_EDGE_AND_NODE(node->as.expr_cast.type);
        EMIT_EDGE_AND_NODE(node->as.expr_cast.expr);
        break;
    case AST_NODE_EXPR_PLACE:
        break;
    case AST_NODE_EXPR_MEMBER:
        EMIT_EDGE_AND_NODE(node->as.expr_member.object);
        EMIT_EDGE_AND_NODE(node->as.expr_member.member);
        break;
    case AST_NODE_EXPR_INIT_LIST:
        EMIT_EDGE_AND_NODE_VEC(node->as.expr_init_list.items);
        break;
    case AST_NODE_EXPR_NIL:
        break;
    case AST_NODE_LITERAL_STRING:
        break;
    case AST_NODE_LITERAL_CHAR:
        break;
    case AST_NODE_LITERAL_DEC:
        break;
    case AST_NODE_LITERAL_HEX:
        break;
    case AST_NODE_LITERAL_BIN:
        break;
    case AST_NODE_LITERAL_BOOL:
        break;
    default:
        unreachable();
        break;
    }
}

String ast_node_to_dot(const ASTNode* node) {
    StringBuilder sb = sb_create(256);
    sb_append_cstr(&sb,
        "digraph {\n"
        "  ranksep = 0.35;\n"
        "  node [\n"
        "    shape = \"record\",\n"
        "    style = \"solid, filled\",\n"
        "    fontcolor = \"dark\",\n"
        "    fontsize = 12,\n"
        "    width = 0.5,\n"
        "    height = 0.25\n"
        "  ];\n\n"
        "  edge [\n"
        "    arrowsize = 0.6,\n"
        "    color = \"black\",\n"
        "    style = \"light\"\n"
        "  ];\n\n"
    );

    emit_ast_node_dot(&sb, node);
    sb_append_cstr(&sb, "}\n");

    String s = sb_get_str(&sb);
    sb_destroy(&sb);

    return s;
}