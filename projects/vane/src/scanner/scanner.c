#include "vane/scanner/scanner.h"

#include "vane/utils/string_utils.h"

#pragma region UTILITIES

#define EOF_CHAR '\0'

static inline char scanner_curr_char(const Scanner* scanner) {
    if (scanner->pos >= scanner->content.len) {
        return EOF_CHAR;
    }
    return scanner->content.text[scanner->pos];
}

static inline void scanner_advance_char(Scanner* scanner) {
    char prev_ch = scanner_curr_char(scanner);

    if (prev_ch == '\r' || prev_ch == '\n') {
        scanner->loc.end.column = SCANNER_DEFAULT_COLUMN_POS;
        scanner->loc.end.line++;
    }
    else {
        scanner->loc.end.column++;
    }

    scanner->pos++;

    char ch = scanner_curr_char(scanner);
    if (prev_ch == '\r' && ch == '\n') {
        scanner->pos++;
    }
}

#pragma endregion

#pragma region DIAGNOSTIC

#define REPORT_APPEND_TRACE(FORMAT, ...) do { \
    if (scanner->rc != NULL) { \
        String msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
        report_collector_append_report_trace(scanner->rc, msg, scanner->loc); \
    } \
} while (false)

#define REPORT(SEVERITY, FORMAT, ...) do { \
    if (scanner->rc != NULL) { \
        String msg = string_from_fmt(FORMAT, ##__VA_ARGS__); \
        report_collector_append_report(scanner->rc, SEVERITY, msg, scanner->loc); \
    } \
} while (false)

#define REPORT_INFO(FORMAT, ...)  REPORT(DIAG_SEVERITY_INFO, FORMAT, ##__VA_ARGS__)
#define REPORT_WARN(FORMAT, ...)  REPORT(DIAG_SEVERITY_WARN, FORMAT, ##__VA_ARGS__)
#define REPORT_ERROR(FORMAT, ...) REPORT(DIAG_SEVERITY_ERROR, FORMAT, ##__VA_ARGS__)

#pragma endregion

#pragma region SCANNER_PARSE

static inline void scanner_skip_whitespaces(Scanner* scanner) {
    char ch = EOF_CHAR;
    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (!is_space(ch)) {
            break;
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
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch == '/' && ch == '*') {
            // nested comment block
            scanner_advance_char(scanner);
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
        REPORT_APPEND_TRACE("unterminated comment block");
    }

    return good;
}

static inline Token scanner_parse_string_literal(Scanner* scanner) {
    // '"' - basic
    // '`' - raw
    const char enclosing_char = scanner_curr_char(scanner);
    scanner_advance_char(scanner);

    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    const u64 prev_pos = scanner->pos;
    bool good = false;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch != '\\' && ch == enclosing_char) {
            good = true;
            scanner_advance_char(scanner);
            break;
        }
        else if (prev_ch != '\\' && is_vspace(ch) && enclosing_char == '"') {
            break;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    if (!good) {
        REPORT_APPEND_TRACE("unterminated string literal");
        return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
    }

    String value = string_substr(&scanner->content, prev_pos, scanner->pos - prev_pos - 1);
    return token_create(TOKEN_LITERAL_STRING, value, scanner->loc);
}

static inline Token scanner_parse_char_literal(Scanner* scanner) {
    scanner_advance_char(scanner);

    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    const u64 prev_pos = scanner->pos;
    bool good = false;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (prev_ch != '\\' && ch == '\'') {
            good = true;
            scanner_advance_char(scanner);
            break;
        }
        else if (prev_ch != '\\' && is_vspace(ch)) {
            break;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    if (!good) {
        REPORT_APPEND_TRACE("unterminated char literal");
        return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
    }

    const u64 lit_len = scanner->pos - prev_pos - 1;

    if (lit_len < 1) {
        REPORT_APPEND_TRACE("char literal has no body");
        return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
    }

    String value = string_substr(&scanner->content, prev_pos, lit_len);

    if (lit_len > 1) {
        REPORT_APPEND_TRACE("char literal `%.*s` contains too many characters", (i32)value.len, value.text);
        return token_create(TOKEN_INVALID, value, scanner->loc);
    }

    return token_create(TOKEN_LITERAL_CHAR, value, scanner->loc);
}

static inline Token scanner_parse_hex_literal(Scanner* scanner) {
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;
    const u64 prev_pos = scanner->pos - 2; // include '0x'

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || is_punc(ch) && ch != '_') {
            break;
        }
        else if (ch == '_' && prev_ch) {
            good = false;
        }
        else if (!is_xdigit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    const u64 lit_len = scanner->pos - prev_pos;

    if (lit_len == 2) {
        REPORT_APPEND_TRACE("hex literal has no body");
        return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
    }

    String value = string_substr(&scanner->content, prev_pos, lit_len);

    if (!good || prev_ch == '_') {
        REPORT_APPEND_TRACE("invalid hex literal `%.*s`", (i32)value.len, value.text);
        return token_create(TOKEN_INVALID, value, scanner->loc);
    }

    return token_create(TOKEN_LITERAL_HEX, value, scanner->loc);
}

static inline Token scanner_parse_bin_literal(Scanner* scanner) {
    char ch = EOF_CHAR;
    char prev_ch = EOF_CHAR;

    bool good = true;
    const u64 prev_pos = scanner->pos - 2; // include '0b'

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || is_punc(ch) && ch != '_') {
            break;
        }
        else if (ch == '_' && prev_ch) {
            good = false;
        }
        else if (!is_bdigit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    const u64 lit_len = scanner->pos - prev_pos;

    if (lit_len == 2) {
        REPORT_APPEND_TRACE("bin literal has no body");
        return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
    }

    String value = string_substr(&scanner->content, prev_pos, lit_len);

    if (!good || prev_ch == '_') {
        REPORT_APPEND_TRACE("invalid bin literal `%.*s`", (i32)value.len, value.text);
        return token_create(TOKEN_INVALID, value, scanner->loc);
    }

    return token_create(TOKEN_LITERAL_BIN, value, scanner->loc);
}

static inline Token scanner_parse_number_literal(Scanner* scanner) {
    char ch = scanner_curr_char(scanner);

    const u64 prev_pos = scanner->pos;

    if (ch == '0') {
        scanner_advance_char(scanner);
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

    bool good = true;
    char prev_ch = EOF_CHAR;

    while ((ch = scanner_curr_char(scanner)) != EOF_CHAR) {
        if (is_space(ch) || is_punc(ch) && ch != '_') {
            break;
        }
        else if (ch == '_' && prev_ch == '_') {
            good = false;
        }
        else if (!is_digit(ch)) {
            good = false;
        }
        scanner_advance_char(scanner);
        prev_ch = ch;
    }

    String value = string_substr(&scanner->content, prev_pos, scanner->pos - prev_pos);

    if (!good || prev_ch == '_') {
        REPORT_APPEND_TRACE("invalid dec literal `%.*s`", (i32)value.len, value.text);
        return token_create(TOKEN_INVALID, value, scanner->loc);
    }

    return token_create(TOKEN_LITERAL_DEC, value, scanner->loc);
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

    String value = string_substr(&scanner->content, prev_pos, scanner->pos - prev_pos);

    // BOOL LITERAL
    if (string_eq_cstr(&value, "true") || string_eq_cstr(&value, "false")) {
        return token_create(TOKEN_LITERAL_BOOL, value, scanner->loc);
    }
#define TOKEN_KEYWORD(KIND, NAME) \
else if (string_eq_cstr(&value, NAME)) { \
    string_destroy(&value); \
    return token_create(TOKEN_KEYWORD_##KIND, STRING_EMPTY, scanner->loc); \
}
#include "vane/scanner/token_kind.def"

    return token_create(TOKEN_IDENTIFIER, value, scanner->loc);
}

#pragma endregion

#pragma region SCANNER_CASES

#define CASE_C1(CHAR, KIND) case CHAR:\
scanner_advance_char(scanner); \
return token_create(KIND, STRING_EMPTY, scanner->loc)

#define CASE_C1C1(CHAR1, CHAR2, KIND1, KIND2) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR2) { \
    scanner_advance_char(scanner); \
    return token_create(KIND2, STRING_EMPTY, scanner->loc); \
} \
return token_create(KIND1, STRING_EMPTY, scanner->loc)

#define CASE_C1C2(CHAR1, CHAR21, CHAR22, KIND1, KIND21, KIND22) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR21) { \
    scanner_advance_char(scanner); \
    return token_create(KIND21, STRING_EMPTY, scanner->loc); \
} else if (scanner_curr_char(scanner) == CHAR22) { \
    scanner_advance_char(scanner); \
    return token_create(KIND22, STRING_EMPTY, scanner->loc); \
} \
return token_create(KIND1, STRING_EMPTY, scanner->loc)

#define CASE_C1C2C1(CHAR1, CHAR21, CHAR22, CHAR3, KIND1, KIND21, KIND22, KIND3) case CHAR1: \
scanner_advance_char(scanner); \
if (scanner_curr_char(scanner) == CHAR21) { \
    scanner_advance_char(scanner); \
    return token_create(KIND21, STRING_EMPTY, scanner->loc); \
} else if (scanner_curr_char(scanner) == CHAR22) { \
    scanner_advance_char(scanner); \
    if (scanner_curr_char(scanner) == CHAR3) { \
        scanner_advance_char(scanner); \
        return token_create(KIND3, STRING_EMPTY, scanner->loc); \
    } \
    return token_create(KIND22, STRING_EMPTY, scanner->loc); \
} \
return token_create(KIND1, STRING_EMPTY, scanner->loc)

#pragma endregion

Scanner scanner_create(const String* path, byte* data, u64 size, ReportCollector* rc) {
    return (Scanner) {
        .content = (String){
            .text = (char*)data,
            .len = size,
        },
        .pos = 0,
        .loc = {
            .path = path,
            .begin = {
                .line = SCANNER_DEFAULT_LINE_POS,
                .column = SCANNER_DEFAULT_COLUMN_POS,
            },
            .end = {
                .line = SCANNER_DEFAULT_LINE_POS,
                .column = SCANNER_DEFAULT_COLUMN_POS,
            }
        },
        .rc = rc,
    };
}

void scanner_destroy(Scanner* scanner) {
    (void)scanner;
    // NOTHING
}

Token scanner_scan_next(Scanner* scanner) {
    assert(scanner != NULL);

    scanner_skip_whitespaces(scanner);
    scanner->loc.begin = scanner->loc.end;

    char ch = scanner_curr_char(scanner);

    switch (ch) {
        CASE_C1('\0', TOKEN_EOF_);
        CASE_C1('(', TOKEN_L_BRACE);
        CASE_C1(')', TOKEN_R_BRACE);
        CASE_C1('{', TOKEN_L_CURLY);
        CASE_C1('}', TOKEN_R_CURLY);
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
            scanner_advance_char(scanner);
            if (!scanner_skip_comment_block(scanner)) {
                return token_create(TOKEN_INVALID, STRING_EMPTY, scanner->loc);
            }
            return scanner_scan_next(scanner);
        case '=':
            scanner_advance_char(scanner);
            return token_create(TOKEN_SLASH_EQUAL, STRING_EMPTY, scanner->loc);
        default:
            return token_create(TOKEN_SLASH, STRING_EMPTY, scanner->loc);
        }
        CASE_C1C1('%', '=', TOKEN_PERCENT, TOKEN_PERCENT_EQUAL);
        CASE_C1C1('^', '=', TOKEN_CARET, TOKEN_CARET_EQUAL);
        CASE_C1C2('&', '=', '&', TOKEN_AMP, TOKEN_AMP_EQUAL, TOKEN_AMP_AMP);
        CASE_C1C2('|', '=', '|', TOKEN_PIPE, TOKEN_PIPE_EQUAL, TOKEN_PIPE_PIPE);
        CASE_C1('~', TOKEN_TILDE);
        CASE_C1C1('=', '=', TOKEN_EQUAL, TOKEN_EQUAL_EQUAL);
        CASE_C1C1('!', '=', TOKEN_EXCLAIM, TOKEN_EXCLAIM_EQUAL);
        CASE_C1C2C1('>', '=', '>', '=', TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_GREATER_GREATER, TOKEN_GREATER_GREATER_EQUAL);
        CASE_C1C2C1('<', '=', '<', '=', TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_LESS_LESS, TOKEN_LESS_LESS_EQUAL);
    case '`':
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

        String value = string_substr(&scanner->content, prev_pos, scanner->pos - prev_pos);

        REPORT_APPEND_TRACE("unknown token `%.*s`", (i32)value.len, value.text);
        return token_create(TOKEN_UNKNOWN, value, scanner->loc);
    }
}