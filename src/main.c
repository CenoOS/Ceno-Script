#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX(x, y)	((x)>=(y) ? (x) : (y))


void *xrealloc(void *ptr, size_t num_bytes){
	ptr = realloc(ptr, num_bytes);
	if(!ptr){
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

void *xmalloc(size_t num_bytes){
	void *ptr = malloc(num_bytes);
	if(!ptr){
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

void fatal(const char *fmt,...){
	va_list args;
	va_start(args, fmt);
	printf("FATAL: ");
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}

void syntax_error(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	printf("Syntax Error: ");
	vprintf(fmt, args);
	printf("\n");
}

typedef struct BufHdr{
	size_t len;
	size_t cap;
	char buf[0];
} BufHdr;

#define buf__hdr(b)	((BufHdr *)((char *)b - offsetof(BufHdr, buf)))
#define buf__fits(b, n)	(buf_len(b)+(n)<=buf_cap(b))
#define buf__fit(b, n)	(buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b)+(n), sizeof(*(b)))))

#define buf_len(b)	((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b)	((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x)	(buf__fit(b, 1), (b)[buf__hdr(b)->len++] = (x))
#define buf_free(b)	((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

void *buf__grow(const void *buf, size_t new_len, size_t elem_size){
	size_t new_cap = MAX(1 + 2*buf_cap(buf), new_len);
	assert(new_len<=new_cap);
	size_t new_size = offsetof(BufHdr, buf) + new_cap*elem_size;
	BufHdr *new_hdr;
	if(buf){
		new_hdr = xrealloc(buf__hdr(buf), new_size);
	}else{
		new_hdr =  xmalloc(new_size);
		new_hdr->len = 0;
	} 
	new_hdr->cap = new_cap;
	return  new_hdr->buf;
}

void buf_test(){
	int *asdf = NULL;
	assert(buf_len(asdf)==0);
	enum{ N = 1024 };
	for(int i = 0;i<N;i++){
		buf_push(asdf,i);
	}
	assert(buf_len(asdf)==N);
	for(int i = 0;i<N;i++){
		// printf("%d\n",asdf[i]);
		assert(asdf[i]==i);
	}
	buf_free(asdf);
	assert(asdf==NULL);
	assert(buf_len(asdf)==NULL);
}


typedef struct InternStr{
	size_t len;
	const char *str;
} InternStr;

static InternStr *interns;

const char *str_intern_range(const char *start, const char *end){
	size_t len = end-start;
	for(size_t i = 0; i<buf_len(interns);i++){
		if(interns[i].len == len && strncmp(interns[i].str,start,len)==0){
			return interns[i].str;
		}
	}
	char *str = malloc(len + 1);
	memcpy(str,start,len);
	str[len] = 0;

	buf_push(interns, ((InternStr){len, str}));
	return str;
}

const char *str_intern(const char *str){
	return str_intern_range(str, str+strlen(str));
}

void str_intern_test(){
	char x[] = "hello";
	char y[] = "hello";
	assert(x!=y);
	assert(str_intern(x)==str_intern(y));
	char z[] = "hello!";
	assert(str_intern(z) != str_intern(y));

}

// lexing:treanslating char stream to token stream
typedef enum TokenKind{
	TOKEN_EOF = 0,
	TOKEN_LAST_CHAR = 127,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_STR,
	TOKEN_NAME,
	
	//...
} TokenKind;

typedef enum TokenMod{
	TOKENMOD_NONE,
	TOKENMOD_HEX,
	TOKENMOD_BIN,
	TOKENMOD_OCT,
	TOKENMOD_CHAR
} TokenMod;

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

const char *token_kind_name(TokenKind kind){
	static char buf[245];
	switch(kind){
		case TOKEN_INT:
			sprintf(buf, "integer");
			break;
		case TOKEN_NAME:
			sprintf(buf,"name");
			break;
		default:
			if(kind<128 && isprint(kind)){
				sprintf(buf,"%c", kind);
			}else{
				sprintf(buf,"<ASCII %d>", kind);
			}
			break;
	}
	return buf;
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
		}else if(tolower(*stream)=='b'){ //binary
			stream++;
			base = 2;
		}else if(isdigit(*stream)){
			base = 8;
		}
	}
	uint64_t val = 0;
	for(;;){
		uint64_t digit = char_to_digit[*stream];
		if(digit == 0 && *stream != '0'){
			break;
		}
		if(digit >= base){
			syntax_error("Digit '%c' out of range for base %llu", *stream, base);
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
			if(*stream=='.' || tolower(*stream)=='e'){
				stream = token.start;
				scan_float();
			}else{
				stream = token.start;
				scan_int();
			}
			break;
		}
		case 'a':	case 'b':	case 'c':	case 'd':	case 'e':	case 'f':	case 'g':	case 'h':	case 'i':	case 'j':
		case 'k':	case 'l':	case 'm':	case 'n':	case 'o':	case 'p':	case 'q':	case 'r':	case 's':	case 't':	
		case 'u':	case 'v':	case 'w':	case 'x':	case 'y':	case 'z':	
		case 'A':	case 'B':	case 'C':	case 'D':	case 'E':	case 'F':	case 'G':	case 'H':	case 'I':	case 'J':	
		case 'K':	case 'L':	case 'M':	case 'N':	case 'O':	case 'P':	case 'Q':	case 'R':	case 'S':	case 'T':	
		case 'U':	case 'V':	case 'W':	case 'X':	case 'Y':	case 'Z':	case '_':
			while(isalnum(*stream) || *stream == '_'){
				stream++;
			} 
			token.kind = TOKEN_NAME;
			token.name = str_intern_range(token.start, stream);
			break;
		default:
			token.kind = *stream++;
			break;
	}
	token.end = stream;
}


void init_stream(const char *str){
	stream = str;
	next_token();
}
void print_token(Token token){
	switch(token.kind){
		case TOKEN_INT:
			printf("TOKEN INT: %llu\n", token.int_val);
			break;
		case TOKEN_FLOAT:
			printf("TOKEN FLOAT: %f\n", token.float_val);
			break;
		case TOKEN_NAME:
			printf("TOKEN NAME: %.*s\n",(int)(token.end - token.start), token.start);
			break;
		default:
			printf("'%c'\n",token.kind);
			break;
	}
}

bool is_token(TokenKind kind){
	return token.kind == kind;
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

/**
 * expr3 = INT | '(' expr ')'
 * expr2 = [-]expr2 | exp3
 * expr1 = expr2 ([/*] expr2)* 
 * expr0 = expr1 ([+-] expr1)* 
 * expr = expr0
 */
int parse_expr();

int parse_expr3(){
	if(is_token(TOKEN_INT)){
		uint64_t val = token.int_val;
		next_token();
		return val;
	}else if(match_token('(')){
		int val = parse_expr();
		expect_token(')');
		return val;
	}else{
		fatal("expected integer or (, got %s",token_kind_name(token.kind));
		return 0;
	}
}

int parse_expr2(){
	if(match_token('-')){
		return -parse_expr2();
	}else{
		return parse_expr3();
	}
}

int parse_expr1(){
	int val = parse_expr2();
	while(is_token('*') || is_token('/')){
		char op = token.kind;
		next_token();

		int rval =	parse_expr2();
		if(op=='*'){
			val*=rval;
		}else{
			assert(op=='/');
			assert(rval!=0);
			val/=rval;
		}
	}
	return val;
}

int parse_expr0(){
	int val = parse_expr1();
	while(is_token('+') || is_token('-')){
		char op = token.kind;
		next_token();
		int rval = 	parse_expr1();
		if(op=='+'){
			val+=rval;
		}else{
			assert(op=='-');
			val-=rval;
		}
	}
	return val;
}
int parse_expr(){
	return parse_expr0();
}


int parse_expr_str(const char *str){
	init_stream(str);
	return parse_expr();
}

#define TEST_EXPR(x) assert(parse_expr_str(#x) == (x))
void parse_test(){
	TEST_EXPR(1);
	TEST_EXPR((1));
	TEST_EXPR(1-2-3);
	TEST_EXPR(2*3+4*5);
	TEST_EXPR(-2*3+4*5);
	TEST_EXPR(-2+-4);
	TEST_EXPR(-2+(-4*7)*4);
}
#undef TEST_EXPR

#define assert_token(x) assert(match_token(x))
#define assert_token_name(x) assert(token.name == str_intern(x) && match_token(TOKEN_NAME))
#define assert_token_int(x) assert(token.int_val == (x) && match_token(TOKEN_INT))
#define assert_token_float(x) assert(token.float_val == (x) && match_token(TOKEN_FLOAT))
#define assert_token_str(x) assert(strcmp(token.str_val,(x)) ==0 && match_token(TOKEN_STR))
#define assert_token_eof(x) assert(is_token(0))

void lex_test(){
	// Integer literal tests
	init_stream("2342352352");
	assert_token_int(2342352352ull);
	assert_token_eof();

	init_stream("0xffff");
	assert_token_int(0xffffull);
	assert_token_eof();

	init_stream("042");
	assert_token_int(042);
	assert_token_eof();

	init_stream("0b0101");
	assert_token_int(0b0101);
	assert_token_eof();

	init_stream("0 2342352352 0xffff 042 0b1111");
	assert_token_int(0);
	assert_token_int(2342352352ull);
	assert_token_int(0xffffull);
	assert_token_int(042);
	assert_token_int(0xf);
	assert_token_eof();

	//Float literal tests
	init_stream("3.14");
	assert_token_float(3.14);
	assert_token_eof();

	init_stream(".14");
	assert_token_float(.14);
	assert_token_eof();

	init_stream("3.14 .14 43.22     12. 3e10");
	assert_token_float(3.14);
	assert_token_float(0.14);
	assert_token_float(43.22);
	assert_token_float(12);
	assert_token_float(3e10);
	assert_token_eof();

	//Char literal tests
	init_stream("'a'");
	assert_token_int('a');
	assert_token_eof();
	init_stream("'\\n'");
	assert_token_int('\n');
	assert_token_eof();
	init_stream("'a' '\\n' '\\r'");
	assert_token_int('a');
	assert_token_int('\n');
	assert_token_int('\r');
	assert_token_eof();

	//String literal tests
	init_stream("\"foo\"");
	assert_token_str("foo");
	assert_token_eof();
	init_stream("\"a\\nb\"");
	assert_token_str("a\nb");
	assert_token_eof();


	init_stream("XY+(XY)_HELLO1,234+2147");
	assert_token_name("XY");
	assert_token('+');
	assert_token('(');
	assert_token_name("XY");
	assert_token(')');
	assert_token_name("_HELLO1");
	assert_token(',');
	assert_token_int(234);
	assert_token('+');
	assert_token_int(2147);
	assert_token_eof();

}

#undef assert_token_eof
#undef assert_token_int
#undef assert_token_name
#undef assert_token


void run_test(){
	buf_test();
	lex_test();
	str_intern_test();
	parse_test();
}

int main(){
	run_test();
	return 0;
}