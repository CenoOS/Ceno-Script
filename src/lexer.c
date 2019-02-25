const char *typedef_keyword;
const char *enum_keyword;
const char *struct_keyword;
const char *union_keyword;
const char *var_keyword;
const char *const_keyword;
const char *func_keyword;
const char *sizeof_keyword;
const char *alignof_keyword;
const char *typeof_keyword;
const char *offsetof_keyword;
const char *break_keyword;
const char *continue_keyword;
const char *return_keyword;
const char *if_keyword;
const char *else_keyword;
const char *while_keyword;
const char *do_keyword;
const char *for_keyword;
const char *switch_keyword;
const char *case_keyword;
const char *default_keyword;
const char *import_keyword;
const char *goto_keyword;

const char *first_keyword;
const char *last_keyword;
const char **keywords;

const char *always_name;
const char *foreign_name;
const char *complete_name;
const char *assert_name;
const char *declare_note_name;
const char *static_assert_name;

#define KEYWORD(name) name##_keyword = str_intern(#name); buf_push(keywords, name##_keyword)

void init_keywords(void) {
    static bool inited;
    if (inited) {
        return;
    }
    KEYWORD(typedef);
    KEYWORD(enum);
    KEYWORD(struct);
    KEYWORD(union);
    KEYWORD(const);
    KEYWORD(var);
    KEYWORD(func);
    KEYWORD(import);
    KEYWORD(goto);
    KEYWORD(sizeof);
    KEYWORD(alignof);
    KEYWORD(typeof);
    KEYWORD(offsetof);
    KEYWORD(break);
    KEYWORD(continue);
    KEYWORD(return);
    KEYWORD(if);
    KEYWORD(else);
    KEYWORD(while);
    KEYWORD(do);
    KEYWORD(for);
    KEYWORD(switch);
    KEYWORD(case);
    KEYWORD(default);

    first_keyword = typedef_keyword;
    last_keyword = default_keyword;

    always_name = str_intern("always");
    foreign_name = str_intern("foreign");
    complete_name = str_intern("complete");
    assert_name = str_intern("assert");
    declare_note_name = str_intern("declare_note");
    static_assert_name = str_intern("static_assert");

    inited = true;
}

#undef KEYWORD

bool is_keyword_name(const char *name) {
    for(int i = 0; i< buf_len(keywords);i++){
		if(name==keywords[i]){
			return true;
		}
	}
	return false;
}


// lexing:treanslating char stream to token stream
typedef enum TokenKind{
	TOKEN_EOF = 0,
	TOKEN_LAST_CHAR = 127,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_STR,
	TOKEN_NAME,
	TOKEN_LSHIFT, // <<
	TOKEN_RSHIFT, // >>
	TOKEN_EQ, // == 
	TOKEN_NOTEQ, // !=
	TOKEN_LTEQ, // <=
	TOKEN_GTEQ, // >=
	TOKEN_AND, // &&
	TOKEN_OR, // ||
	TOKEN_COLON_ASSIGN, // :=
	TOKEN_ADD_ASSIGN, // +=
	TOKEN_SUB_ASSIGN, // -=
	TOKEN_AND_ASSIGN, // &=
	TOKEN_OR_ASSIGN, // |=
	TOKEN_XOR_ASSIGN, // ^=
	TOKEN_LSHIFT_ASSIGN, // <<=
	TOKEN_RSHIFT_ASSIGN, // >>=
	TOKEN_MUL_ASSIGN, // *=
	TOKEN_DIV_ASSIGN, // /=
	TOKEN_MOD_ASSIGN,  // %=
	TOKEN_INC, // ++
	TOKEN_DEC, // --


	TOKEN_POUND, 
	TOKEN_KEYWORD,

	TOKEN_COMMA, // ,
	TOKEN_DOT, // .

	TOKEN_NOT, // !
	
	TOKEN_COLON, // :
	TOKEN_SEMICOLON, // ;
	TOKEN_ASSIGN, // =
	TOKEN_LBRAC, // (
	TOKEN_RBRAC, // )
	TOKEN_LSBRAC, // [
	TOKEN_RSBRAC, // ]
	TOKEN_LCBRAC, // {
	TOKEN_RCBRAC, // }
	TOKEN_QM, // ?

	TOKEN_LT, // <
	TOKEN_GT, // >
	TOKEN_ADD, // +
	TOKEN_SUB, // -
	TOKEN_MUL, // *
	TOKEN_DIV, // /
	TOKEN_MOD, // %
	TOKEN_BAND, // &
	TOKEN_BOR, // |
	TOKEN_XOR, // ^
	TOKEN_HASHTAG, // #

} TokenKind;

typedef enum TokenMod{
	TOKENMOD_NONE,
	TOKENMOD_HEX,
	TOKENMOD_BIN,
	TOKENMOD_OCT,
	TOKENMOD_CHAR
} TokenMod;

const char *token_kind_names[] = {
	[TOKEN_LSHIFT]="<<",
	[TOKEN_RSHIFT]=">>",
	[TOKEN_EQ]="==",
	[TOKEN_NOTEQ]="!=",
	[TOKEN_LTEQ]="<=",
	[TOKEN_GTEQ]=">=",
	[TOKEN_AND]="&&",
	[TOKEN_OR]="||",
	[TOKEN_COLON_ASSIGN]=":=",
	[TOKEN_ADD_ASSIGN]="+=",
	[TOKEN_SUB_ASSIGN]="-=",
	[TOKEN_AND_ASSIGN]="&=",
	[TOKEN_OR_ASSIGN]="|=",
	[TOKEN_XOR_ASSIGN]="^=",
	[TOKEN_LSHIFT_ASSIGN]="<<=",
	[TOKEN_RSHIFT_ASSIGN]=">>=",
	[TOKEN_MUL_ASSIGN]="*=",
	[TOKEN_DIV_ASSIGN]="/=",
	[TOKEN_MOD_ASSIGN]="\%=",
	[TOKEN_INC]="++",
	[TOKEN_DEC]="--",		
	[TOKEN_COMMA]=",",
	[TOKEN_DOT]=".",
	[TOKEN_COLON]= ":",
	[TOKEN_SEMICOLON] = ";",
	[TOKEN_ASSIGN]= "=",
	[TOKEN_LBRAC]= "(",
	[TOKEN_RBRAC]= ")",
	[TOKEN_LSBRAC]= "[",
	[TOKEN_RSBRAC]= "]",
	[TOKEN_LCBRAC]= "{",
	[TOKEN_RCBRAC]= "}",
	[TOKEN_NOT] = "!",
	[TOKEN_QM]="?",
	[TOKEN_LT]= "<",
	[TOKEN_GT]= ">",
	[TOKEN_ADD]= "+",
	[TOKEN_SUB]= "-",
	[TOKEN_MUL]= "*",
	[TOKEN_DIV]= "/",
	[TOKEN_MOD]= "\%",
	[TOKEN_BAND]= "&",
	[TOKEN_BOR]= "|",
	[TOKEN_XOR]= "^",
	[TOKEN_HASHTAG]="#",
};

const char *token_kind_name(TokenKind kind) {
    if (kind < sizeof(token_kind_names)/sizeof(*token_kind_names)) {
        return token_kind_names[kind];
    } else {
        return "<unknown>";
    }
}


size_t copy_token_kind_str(char *dest,size_t dest_size, TokenKind kind){
	size_t n = 0;
	switch(kind){
		case 0:
			n = snprintf(dest,dest_size,"end of file");
			break;
		case TOKEN_INT:
			n = snprintf(dest,dest_size,"integer");
			break;
		case TOKEN_FLOAT:
			n = snprintf(dest,dest_size,"float");
			break;
		case TOKEN_NAME:
			n = snprintf(dest,dest_size,"name");
			break;
		default:
			if(kind<128 && isprint(kind)){
				n = snprintf(dest,dest_size,"%c", kind);
			}else{
				n = snprintf(dest,dest_size,"<ASCII %d>", kind);
			}
			break;
	}
	return n;
}

typedef struct Token{
	TokenKind kind;
	TokenMod mod;
	const char* start;
	const char* end;
	union {
		uint64_t int_val;
		double float_val;
		const char *str_val;
		const char *name;
	};
} Token;


Token token;
const char* stream;

uint8_t char_to_digit[256] = {
	['0'] = 0,
	['1'] = 1,
	['2'] = 2,
	['3'] = 3,
	['4'] = 4,
	['5'] = 5,
	['6'] = 6,
	['7'] = 7,
	['8'] = 8,
	['9'] = 9,
	['a'] = 10,	['A'] = 10,
	['b'] = 11,	['B'] = 11,
	['c'] = 12,	['C'] = 12,
	['d'] = 13,	['D'] = 13,
	['e'] = 14,	['E'] = 14,
	['f'] = 15,	['F'] = 15,
};

void scan_int(){
	uint64_t base  = 10;

	if(*stream== '0'){
		stream++;
		if(tolower(*stream)=='x'){ // hexadecimal
			stream++;
			base  = 16;
			token.mod = TOKENMOD_HEX;
		}else if(tolower(*stream)=='b'){ //binary
			stream++;
			base = 2;
			token.mod = TOKENMOD_BIN;
		}else if(isdigit(*stream)){
			base = 8;
			token.mod = TOKENMOD_OCT;
		}
	}
	uint64_t val = 0;
	for(;;){
		uint64_t digit = char_to_digit[*stream];
		if(digit == 0 && *stream != '0'){
			break;
		}
		if(digit >= base){
			syntax_error("Digit '%c' out of range for base %" PRIu64, *stream, base);
			digit = 0;
		}
		if(val > (UINT64_MAX - digit) / base){
			syntax_error("Integer literal overflow");
			while(isdigit(*stream)){
				stream++;
			}
			val = 0;
		}
		val = val*base + digit;
		stream++;
	}

	token.int_val = val;
	token.kind = TOKEN_INT;
}

void scan_float(){
	const char *start = stream;
	while(isdigit(*stream)){
		stream++;
	}
	if(*stream=='.'){
		stream++;
	}
	while(isdigit(*stream)){
		stream++;
	}
	if(tolower(*stream)=='e'){
		stream++;
		if(*stream=='+' || *stream=='-'){
			stream++;
		}
		if(!isdigit(*stream)){
				syntax_error("Expected digit after float literal exponent, found '%c'.",*stream);
		}
		while(isdigit(*stream)){
			stream++;
		}
	}
	const char *end = stream;
	double val = strtod(start,NULL);
	if(val == HUGE_VAL || val == -HUGE_VAL){
		syntax_error("Float literal overflow");
	}
	token.float_val = val;
	token.kind  = TOKEN_FLOAT;
}


char eacape_to_char[256] = {
	['n'] = '\n',
	['r'] = '\r',
	['t'] = '\t',
	['v'] = '\v',
	['b'] = '\b',
	['a'] = '\a',
	['0'] = 0,
};
void scan_char(){
	assert(*stream=='\'');
	stream++;

	char val = 0;
	if(*stream=='\''){
		syntax_error("Char literal cannot be empty");
		stream++;
	}else if(*stream=='\n'){
		syntax_error("Char literal cannot contain newline");
	}else if(*stream=='\\'){
		stream++;
		val = eacape_to_char[*stream];
		if(val == 0 && *stream!='0'){
				syntax_error("Invalid char literal eacape '\\%c'",*stream);
		}
		stream++;
	}else{
		val = *stream;
		stream++;
	}

	if(*stream != '\''){
			syntax_error("Expected closing char quote,got '%c'.",*stream);
	}else{
		stream++;
	}

	token.kind = TOKEN_INT;
	token.int_val = val;
	token.mod = TOKENMOD_CHAR;
}

void scan_str(){
	assert(*stream=='"');
	stream++;
	char* str = NULL;
	while(*stream && *stream != '"'){
		char val = *stream;
		if(val=='\n'){
			syntax_error("String literal cannot contain newline");
		}else if(val=='\\'){
			stream++;
			val = eacape_to_char[*stream];
			if(val == 0 && *stream!='0'){
				syntax_error("Invalid string literal eacape '\\%c'",*stream);
			}
		}
		buf_push(str,val);
		stream++;
	}

	if(*stream){
		assert(*stream=='"');
		stream++;
	}else{
		syntax_error("Unexpected end of file within string literal,got '%c'.",*stream);
	}
	buf_push(str,0);
	token.kind = TOKEN_STR;
	token.str_val = str;
}

#define CASE1(c1, k1) \
    case c1: \
        token.kind = k1; \
        stream++; \
        break;

#define CASE2(c1, k1, c2, k2) \
    case c1: \
        token.kind = k1; \
        stream++; \
        if (*stream == c2) { \
            token.kind = k2; \
            stream++; \
        } \
        break;

#define CASE3(c1, k1, c2, k2, c3, k3) \
    case c1: \
        token.kind = k1; \
        stream++; \
        if (*stream == c2) { \
            token.kind = k2; \
            stream++; \
        } else if (*stream == c3) { \
            token.kind = k3; \
            stream++; \
        } \
        break;
void next_token(){
top:
	token.start = stream;
	token.mod = 0;
	switch(*stream){
		case ' ':	case '\n': case '\r':	case '\t': case '\v':
			while(isspace(*stream)){
				stream++;
			}
			goto top;
			break;
		case '\'':
			scan_char();
			break;
		case '"':
			scan_str();
			break;
		case '.':
			scan_float();
			break;
		case '0':	case '1':	case '2':	case '3':	case '4':	case '5':	case '6':	case '7':	case '8':	case '9':{
			while(isdigit(*stream)){
				stream++;
			}
			char c = *stream;
			stream = token.start;
			if(c=='.' || tolower(c)=='e'){
				scan_float();
			}else{
				scan_int();
			}
			break;
		}
		case 'a':	case 'b':	case 'c':	case 'd':	case 'e':	case 'f':	case 'g':	case 'h':	case 'i':	case 'j':
		case 'k':	case 'l':	case 'm':	case 'n':	case 'o':	case 'p':	case 'q':	case 'r':	case 's':	case 't':	
		case 'u':	case 'v':	case 'w':	case 'x':	case 'y':	case 'z':	
		case 'A':	case 'B':	case 'C':	case 'D':	case 'E':	case 'F':	case 'G':	case 'H':	case 'I':	case 'J':	
		case 'K':	case 'L':	case 'M':	case 'N':	case 'O':	case 'P':	case 'Q':	case 'R':	case 'S':	case 'T':	
		case 'U':	case 'V':	case 'W':	case 'X':	case 'Y':	case 'Z':	
		case '_':
			while(isalnum(*stream) || *stream == '_'){
				stream++;
			} 
			token.name = str_intern_range(token.start, stream);
			token.kind = is_keyword_name(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;
			break;
		case '<':
			token.kind = *stream++;
			if(*stream=='<'){
				token.kind = TOKEN_LSHIFT;
				stream++;
				if(*stream=='='){
					token.kind = TOKEN_LSHIFT_ASSIGN;
					stream++;
				}
			}else if(*stream == '='){
				token.kind = TOKEN_LTEQ;
				stream++;
			}
			break;
		case '>':
			token.kind = *stream++;
			if(*stream=='>'){
				token.kind = TOKEN_RSHIFT;
				stream++;
				if(*stream=='='){
					token.kind = TOKEN_RSHIFT_ASSIGN;
					stream++;
				}
			}else if(*stream == '='){
				token.kind = TOKEN_GTEQ;
				stream++;
			}
			break;
		
		CASE1('(', TOKEN_LBRAC)
    	CASE1(')', TOKEN_RBRAC)
    	CASE1('{', TOKEN_LCBRAC)
    	CASE1('}', TOKEN_RCBRAC)
    	CASE1('[', TOKEN_LSBRAC)
    	CASE1(']', TOKEN_RSBRAC)
    	CASE1(',', TOKEN_COMMA)
    	CASE1('#', TOKEN_HASHTAG)
    	CASE1('?', TOKEN_QM)
    	CASE1(';', TOKEN_SEMICOLON)
    	CASE2('!', TOKEN_NOT, '=', TOKEN_NOTEQ)
    	CASE2(':', TOKEN_COLON, '=', TOKEN_COLON_ASSIGN)
    	CASE2('=', TOKEN_ASSIGN, '=', TOKEN_EQ)
    	CASE2('^', TOKEN_XOR, '=', TOKEN_XOR_ASSIGN)
    	CASE2('*', TOKEN_MUL, '=', TOKEN_MUL_ASSIGN)
    	CASE2('%', TOKEN_MOD, '=', TOKEN_MOD_ASSIGN)
    	CASE3('+', TOKEN_ADD, '=', TOKEN_ADD_ASSIGN, '+', TOKEN_INC)
    	CASE3('-', TOKEN_SUB, '=', TOKEN_SUB_ASSIGN, '-', TOKEN_DEC)
    	CASE3('&', TOKEN_BAND, '=', TOKEN_AND_ASSIGN, '&', TOKEN_ADD)
    	CASE3('|', TOKEN_BOR, '=', TOKEN_OR_ASSIGN, '|', TOKEN_OR)

		default:
			token.kind = *stream++;
			break;
	}
	token.end = stream;
}
#undef CASE1
#undef CASE2
#undef CASE3



void init_stream(const char *str){
	stream = str;
	next_token();
}


void print_token(Token token){
	switch(token.kind){
		case TOKEN_INT:
			printf("TOKEN INT: %" PRIu64 "\n", token.int_val);
			break;
		case TOKEN_FLOAT:
			printf("TOKEN FLOAT: %f\n", token.float_val);
			break;
		case TOKEN_NAME:
			printf("TOKEN NAME: %.*s\n",(int)(token.end - token.start), token.start);
			break;
		case TOKEN_KEYWORD:
			printf("TOKEN KEYWORD: %s\n",token.name);
		default:
			printf("'%c'\n",token.kind);
			break;
	}
}

bool is_token(TokenKind kind){
	return token.kind == kind;
}

bool is_token_eof(void) {
    return token.kind == TOKEN_EOF;
}

bool is_token_name(const char  *name){
	return token.kind == TOKEN_NAME && token.name == name;
}

bool match_token(TokenKind kind){
	if(is_token(kind)){
		next_token();
		return true;
	}else{
		return false;
	}
}

bool expect_token(TokenKind kind){
	if(is_token(kind)){
		next_token();
		return true;
	}else{
		fatal("expect token %s, got %s",token_kind_name(kind),token_kind_name(token.kind));
		return false;
	}
}

bool is_keyword(const char *name) {
    return is_token(TOKEN_KEYWORD) && token.name == name;
}

bool match_keyword(const char *name) {
    if (is_keyword(name)) {
        next_token();
        return true;
    } else {
        return false;
    }
}

