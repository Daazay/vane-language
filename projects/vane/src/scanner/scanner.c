#include "vane/scanner/scanner.h"

#include "vane/utils/string_utils.h"
#include "vane/utils/string_builder.h"

#pragma region UTILITIES

#define EOF_CHAR '\0'
#define SCANNER_DEFAULT_COLUMN_POS 1
#define SCANNER_DEFAULT_LINE_POS   1

static inline char scanner_curr_char(const Scanner* scanner) {
    if (scanner->pos >= scanner->content->len) {
        return EOF_CHAR;
    }
    return scanner->content->text[scanner->pos];
}

static inline void scanner_advance_char(Scanner* scanner) {
    char prev_ch = scanner_curr_char(scanner);

    if (prev_ch == '\r' || prev_ch == '\n') {
        scanner->loc.range.end.column = SCANNER_DEFAULT_COLUMN_POS;
        scanner->loc.range.end.line++;
        scanner->first_in_line = true;
    }
    else {
        scanner->loc.range.end.column++;
    }

    scanner->pos++;

    char ch = scanner_curr_char(scanner);
    if (prev_ch == '\r' && ch == '\n') {
        scanner->pos++;
    }
}

#define RET_TOKEN(KIND, VALUE) \
scanner->first_in_line = false; \
return (Token) { \
    .kind = KIND, \
    .value = VALUE, \
    .loc = scanner->loc, \
}

#pragma endregion

#pragma region DIAGNOSTIC

#define TRACE(FMT, ...) RC_TRACE(scanner->rc, scanner->loc, FMT, ##__VA_ARGS__)

#pragma endregion

#pragma region SCANNER_PARSE

static inline void scanner_skip_whitespaces(Scanner* scanner) {
    char ch = EOF_CHAR;
    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (!is_space(ch)) {
            return;
        }
        scanner_advance_char(scanner);
    }
}

static inline void scanner_skip_comment_line(Scanner* scanner) {
    char ch = EOF_CHAR;
    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_vspace(ch)) {
            break;
        }
        scanner_advance_char(scanner);
    }
}

static inline bool scanner_skip_comment_block(Scanner* scanner) {
    scanner_advance_char(scanner);

    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch == '/' && ch == '*') {
            // nested comment block
            if (!scanner_skip_comment_block(scanner)) {
                good = false;
                break;
            }
            continue;
        }
        else if (prev_ch == '*' && ch == '/') {
            scanner_advance_char(scanner);
            break;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    if (!good) {
        TRACE("Unterminated comment block");
    }

    return good;
}

static inline Token scanner_parse_string_literal(Scanner* scanner) {
    scanner_advance_char(scanner);

    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    u64 prev_pos = scanner->pos;
    bool good = false;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch != '\\' && ch == '\"') {
            good = true;
            scanner_advance_char(scanner);
            break;
        }
        else if (is_vspace(ch)) {
            good = false;
            break;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    if (!good) {
        TRACE("Unterminated string literal");
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    String value = string_substr(scanner->content, prev_pos, scanner->pos - prev_pos - 1);

    RET_TOKEN(TOKEN_LITERAL_STRING, value);
}

static inline Token scanner_parse_char_literal(Scanner* scanner) {
    scanner_advance_char(scanner);

    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    u64 prev_pos = scanner->pos;
    bool good = true;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch != '\\' && ch == '\'') {
            scanner_advance_char(scanner);
            break;
        }
        else if (is_vspace(ch)) {
            good = false;
            break;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    if (!good) {
        TRACE("Unterminated char literal");
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    const u64 len = scanner->pos - prev_pos - 1;
    if (len < 1) {
        TRACE("Char literal is empty");
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }
    else if ((scanner->content->text[prev_pos] == '\\' && len > 2) || (scanner->content->text[prev_pos] != '\\' && len > 1)) {
        TRACE("Char literal `%.*s` contains too many characters",
            (i32)len,
            scanner->content->text + prev_pos
        );
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    String value = string_substr(scanner->content, prev_pos, len);
    RET_TOKEN(TOKEN_LITERAL_CHAR, value);
}

static inline Token scanner_parse_hex_literal(Scanner* scanner) {
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;
    const u64 prev_pos = scanner->pos - 2; // include prefix '0x' or '0X'

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || (is_punc(ch) && ch != '_')) {
            break;
        }
        else if (ch == '_') {
            if (prev_ch == '_') {
                good = false;
            }
        }
        else if (!is_xdigit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    const u64 len = scanner->pos - prev_pos;

    if (len == 2) {
        TRACE("Hex literal has no body");
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }
    else if (!good || prev_ch == '_') {
        TRACE("Invalid hex literal `%.*s`",
            (i32)len,
            scanner->content->text + prev_pos
        );
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    String value = string_substr(scanner->content, prev_pos, len);
    RET_TOKEN(TOKEN_LITERAL_HEX, value);
}

static inline Token scanner_parse_bin_literal(Scanner* scanner) {
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;
    const u64 prev_pos = scanner->pos - 2; // include prefix '0b' or '0B'

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || (is_punc(ch) && ch != '_')) {
            break;
        }
        else if (ch == '_') {
            if (prev_ch == '_') {
                good = false;
            }
        }
        else if (!is_bdigit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    const u64 len = scanner->pos - prev_pos;

    if (len == 2) {
        TRACE("Bin literal has no body");
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }
    else if (!good || prev_ch == '_') {
        TRACE("Invalid bin literal `%.*s`",
            (i32)len,
            scanner->content->text + prev_pos
        );
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    String value = string_substr(scanner->content, prev_pos, len);
    RET_TOKEN(TOKEN_LITERAL_BIN, value);
}

static inline Token scanner_parse_dec_literal(Scanner* scanner) {
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;
    const u64 prev_pos = scanner->pos - 1; // include char before

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || (is_punc(ch) && ch != '_')) {
            break;
        }
        else if (ch == '_') {
            if (prev_ch == '_') {
                good = false;
            }
        }
        else if (!is_digit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    const u64 len = scanner->pos - prev_pos;

    if (!good || prev_ch == '_') {
        TRACE("Invalid dec literal `%.*s`",
            (i32)len,
            scanner->content->text + prev_pos
        );
        RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
    }

    String value = string_substr(scanner->content, prev_pos, len);
    RET_TOKEN(TOKEN_LITERAL_DEC, value);
}

static inline Token scanner_parse_number_literal(Scanner* scanner) {
    char ch = scanner_curr_char(scanner);
    scanner_advance_char(scanner);

    if (ch == '0') {
        ch = scanner_curr_char(scanner);

        // HEX LITERAL
        if (ch == 'x' || ch == 'X') {
            scanner_advance_char(scanner);
            return scanner_parse_hex_literal(scanner);
        }
        // BIN LITERAL
        else if (ch == 'b' || ch == 'B') {
            scanner_advance_char(scanner);
            return scanner_parse_bin_literal(scanner);
        }
    }

    return scanner_parse_dec_literal(scanner);
}

static inline Token scanner_parse_identifier_or_keyword(Scanner* scanner) {
    char ch = EOF_CHAR;
    const u64 prev_pos = scanner->pos;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (!is_alnum_(ch)) {
            break;
        }
        scanner_advance_char(scanner);
    }

    String value = string_substr(scanner->content, prev_pos, scanner->pos - prev_pos);

    // BOOL LITERAL
    if (string_eq_cstr(&value, "true") || string_eq_cstr(&value, "false")) {
        RET_TOKEN(TOKEN_LITERAL_BOOL, value);
    }
#define TOKEN_KEYWORD(KIND, NAME) \
else if (string_eq_cstr(&value, NAME)) { \
    string_destroy(&value); \
    RET_TOKEN(TOKEN_KEYWORD_##KIND, STRING_EMPTY); \
}
#include "vane/scanner/token_kind.def"

    RET_TOKEN(TOKEN_IDENTIFIER, value);
}

#pragma endregion

#pragma region SCANNER_CASES

#define CASE_C1(CHAR, KIND) case CHAR:\
scanner_advance_char(scanner); \
RET_TOKEN(KIND, STRING_EMPTY)

#define CASE_C1C1(CHAR1, CHAR2, KIND1, KIND2) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR2) { \
    scanner_advance_char(scanner); \
    RET_TOKEN(KIND2, STRING_EMPTY); \
} \
RET_TOKEN(KIND1, STRING_EMPTY)

#define CASE_C1C2(CHAR1, CHAR21, CHAR22, KIND1, KIND21, KIND22) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR21) { \
    scanner_advance_char(scanner); \
    RET_TOKEN(KIND21, STRING_EMPTY); \
} else if (scanner_curr_char(scanner) == CHAR22) { \
    scanner_advance_char(scanner); \
    RET_TOKEN(KIND22, STRING_EMPTY); \
} \
RET_TOKEN(KIND1, STRING_EMPTY)

#define CASE_C1C2C1(CHAR1, CHAR21, CHAR22, CHAR3, KIND1, KIND21, KIND22, KIND3) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR21) { \
    scanner_advance_char(scanner); \
    RET_TOKEN(KIND21, STRING_EMPTY); \
} else if (scanner_curr_char(scanner) == CHAR22) { \
    scanner_advance_char(scanner); \
    if (scanner_curr_char(scanner) == CHAR3) { \
        scanner_advance_char(scanner); \
        RET_TOKEN(KIND3, STRING_EMPTY); \
    } \
    RET_TOKEN(KIND22, STRING_EMPTY); \
} \
RET_TOKEN(KIND1, STRING_EMPTY)

#pragma endregion

Scanner scanner_create(const String* path, const String* content, ReportCollector* rc) {
    assert(path != NULL && content != NULL && rc != NULL);

    return (Scanner) {
        .content = content,
        .pos = 0,
        .loc = {
            .path = path,
            .range.begin.line   = SCANNER_DEFAULT_LINE_POS,
            .range.begin.column = SCANNER_DEFAULT_COLUMN_POS,
            .range.end.line     = SCANNER_DEFAULT_LINE_POS,
            .range.end.column   = SCANNER_DEFAULT_COLUMN_POS,
        },
        .rc = rc,
        .first_in_line = true,
    };
}

void scanner_destroy(Scanner* scanner) {
    (void)scanner;
    /* DO NOTHING */
}

Token scanner_scan_next(Scanner* scanner) {
    assert(scanner != NULL);

    scanner_skip_whitespaces(scanner);
    scanner->loc.range.begin = scanner->loc.range.end;

    char ch = scanner_curr_char(scanner);
    switch (ch) {
        CASE_C1('\0', TOKEN_EOF_);
        CASE_C1('(', TOKEN_L_BRACE);
        CASE_C1(')', TOKEN_R_BRACE);
        CASE_C1('[', TOKEN_L_BRACKET);
        CASE_C1(']', TOKEN_R_BRACKET);
        CASE_C1('.', TOKEN_DOT);
        CASE_C1(',', TOKEN_COMMA);
        CASE_C1(':', TOKEN_COLON);
        CASE_C1(';', TOKEN_SEMICOLON);
        CASE_C1C2('+', '=', '+', TOKEN_PLUS, TOKEN_PLUS_EQUAL, TOKEN_PLUS_PLUS);
        CASE_C1C2('-', '=', '-', TOKEN_MINUS, TOKEN_MINUS_EQUAL, TOKEN_MINUS_MINUS);
        CASE_C1C1('*', '=', TOKEN_STAR, TOKEN_STAR_EQUAL);
    case '/':
        scanner_advance_char(scanner);
        ch = scanner_curr_char(scanner);

        switch (ch) {
        case '/':
            scanner_skip_comment_line(scanner);
            return scanner_scan_next(scanner);
        case '*':
            if (!scanner_skip_comment_block(scanner)) {
                RET_TOKEN(TOKEN_INVALID, STRING_EMPTY);
            }
            return scanner_scan_next(scanner);
        case '=':
            scanner_advance_char(scanner);
            RET_TOKEN(TOKEN_SLASH_EQUAL, STRING_EMPTY);
        default:
            RET_TOKEN(TOKEN_SLASH, STRING_EMPTY);
        }
        CASE_C1C1('%', '=', TOKEN_PERCENT, TOKEN_PERCENT_EQUAL);
        CASE_C1('^', TOKEN_CARET);
        CASE_C1C2('&', '=', '&', TOKEN_AMP, TOKEN_AMP_EQUAL, TOKEN_AMP_AMP);
        CASE_C1C2('|', '=', '|', TOKEN_PIPE, TOKEN_PIPE_EQUAL, TOKEN_PIPE_PIPE);
        CASE_C1('~', TOKEN_TILDE);
        CASE_C1C1('=', '=', TOKEN_EQUAL, TOKEN_EQUAL_EQUAL);
        CASE_C1C1('!', '=', TOKEN_EXCLAIM, TOKEN_EXCLAIM_EQUAL);
        CASE_C1C2C1('>', '=', '>', '=', TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_GREATER_GREATER, TOKEN_GREATER_GREATER_EQUAL);
        CASE_C1C2C1('<', '=', '<', '=', TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_LESS_LESS, TOKEN_LESS_LESS_EQUAL);
    case '"':  return scanner_parse_string_literal(scanner);
    case '\'': return scanner_parse_char_literal(scanner);
    default:
        // NUMBER LITERALS
        if (is_digit(ch)) {
            return scanner_parse_number_literal(scanner);
        }
        // IDENTIFIER OR KEYWORD
        else if (is_alnum_(ch)) {
            return scanner_parse_identifier_or_keyword(scanner);
        }

        // UNKNOWN
        const u64 prev_pos = scanner->pos;
        while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
            if (is_space(ch) || is_punc(ch)) {
                break;
            }
            scanner_advance_char(scanner);
        }

        const u64 len = scanner->pos - prev_pos;

        TRACE("Unknown token `%.*s`",
            (i32)len,
            scanner->content->text + prev_pos
        );
        RET_TOKEN(TOKEN_UNKNOWN, STRING_EMPTY);
    }
}