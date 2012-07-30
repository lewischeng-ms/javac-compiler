#include "javac.h"

FILE *input;
int lineno = 0;

phashtab_t keywords;

token_t currtok;
token_t latok;

void lex_init(const char *filename)
{
	input = fopen(filename, "r");
	if (!input)
		fatal("cannot open file %s", filename);
	
	// After opening file, set line number to 1
	// to enable displaying of line number in 
	// fatal(), warn(), ...
	lineno = 1;
	
	keywords = hashtab_create(37);
	
	hashtab_insert(keywords, "native", NULL, 0);
	hashtab_insert(keywords, "record", NULL, 0);
	hashtab_insert(keywords, "new", NULL, 0);
	hashtab_insert(keywords, "int", NULL, 0);
	hashtab_insert(keywords, "string", NULL, 0);
	hashtab_insert(keywords, "char", NULL, 0);
	hashtab_insert(keywords, "null", NULL, 0);
	hashtab_insert(keywords, "if", NULL, 0);
	hashtab_insert(keywords, "else", NULL, 0);
	hashtab_insert(keywords, "while", NULL, 0);
	hashtab_insert(keywords, "for", NULL, 0);
	hashtab_insert(keywords, "return", NULL, 0);
	hashtab_insert(keywords, "break", NULL, 0);
	hashtab_insert(keywords, "continue", NULL, 0);
	hashtab_insert(keywords, ";", NULL, 0);
	hashtab_insert(keywords, "[", NULL, 0);
	hashtab_insert(keywords, "]", NULL, 0);
	hashtab_insert(keywords, "{", NULL, 0);
	hashtab_insert(keywords, "}", NULL, 0);
	hashtab_insert(keywords, "(", NULL, 0);
	hashtab_insert(keywords, ")", NULL, 0);
	hashtab_insert(keywords, ",", NULL, 0);
	hashtab_insert(keywords, "=", NULL, 0);
	hashtab_insert(keywords, "||", NULL, 0);
	hashtab_insert(keywords, "&&", NULL, 0);
	hashtab_insert(keywords, "==", NULL, 0);
	hashtab_insert(keywords, "!=", NULL, 0);
	hashtab_insert(keywords, "<", NULL, 0);
	hashtab_insert(keywords, "<=", NULL, 0);
	hashtab_insert(keywords, ">", NULL, 0);
	hashtab_insert(keywords, ">=", NULL, 0);
	hashtab_insert(keywords, "+", NULL, 0);
	hashtab_insert(keywords, "-", NULL, 0);
	hashtab_insert(keywords, "*", NULL, 0);
	hashtab_insert(keywords, "/", NULL, 0);
	hashtab_insert(keywords, "%", NULL, 0);
	hashtab_insert(keywords, "!", NULL, 0);
	hashtab_insert(keywords, ".", NULL, 0);
	
#	ifndef NDEBUG
	// To see if elemets are spreading evenly.
	HASHTAB_STAT(keywords);
#	endif
	
	// Mark the current token as valid.
	currtok.token_flag_valid = 1;
}

void lex_finit()
{
	fclose(input);

	hashtab_destroy(keywords);
}

// The lexer is just a big DFA.
enum DFA_STATES {
	STATE_START,
	STATE_END,
	STATE_TRY_BR, // breakline
	STATE_PARSE_INT, // 12345
	STATE_PARSE_ID, // maybe a keyword
	STATE_PARSE_STR, // "abc"
	STATE_PARSE_CHAR, // 'x'
	STATE_TRY_COMMENT,	//  'a / b' OR '//'
	STATE_PARSE_ESCAPE_IN_STR, // "hello\n"
	STATE_PARSE_ESCAPE_IN_CHAR, // '\n'
	STATE_LINE_COMMENT,		// comment like this
	STATE_BLOCK_COMMENT,	/* comment like this */
	STATE_TRY_END_BLOCK_COMMENT // comment like this: /* *?
};

// Helper function to return a string consisting of one character.
static const char *char2str(char c)
{
	static char buf[2];
	buf[0] = c;
	return buf;
}

#define MAX_TOKEN_LENGTH 100

static void lex_get_token(ptoken_t ptoken)
{
	assert(ptoken);
	
	char buf[MAX_TOKEN_LENGTH];
	int buflen = 0;
	
	int state = STATE_START;
	while (state != STATE_END) {
		int c = fgetc(input);
		switch (state) {
		case STATE_START:
			switch (c) {
			case ' ':
			case '\t':
			case '\r':
				// Ignore these characters,
				// and remain current state.
				break;
			case '\n':
				// Handle both '\n' and '\r\n'
				lineno++;
				break;
			case '"':
				// Entering a string.
				state = STATE_PARSE_STR;
				ptoken->token_type = TOKEN_TYPE_STRING_CONST;
				break;
			case '\'':
				// Entering a character.
				state = STATE_PARSE_CHAR;
				ptoken->token_type = TOKEN_TYPE_CHAR_CONST;
				break;
			case '/':
				// Maybe entering comment.
				state = STATE_TRY_COMMENT;
				break;
			case '.':
			case '[':
			case ']':
			case '(':
			case ')':
			case '{':
			case '}':
			case ';':
			case ',':
			case '+':
			case '-':
			case '*':
				// these operators consist of a single character.
				ptoken->token_type = TOKEN_TYPE_KEYWORD;
				ptoken->token_value.text = xstrdup(char2str(c));
				state = STATE_END;
				break;
			case '=':
			case '<':
			case '>':
			case '!':
				// these operators can be followed by a '='.
				ptoken->token_type = TOKEN_TYPE_KEYWORD;
				buf[buflen++] = c;
				c = fgetc(input);
				if (c == '=')
					buf[buflen++] = c;
				else
					ungetc(c, input);
				buf[buflen] = 0;
				ptoken->token_value.text = xstrdup(buf);
				state = STATE_END;
				break;
			case '&':
			case '|':
				// these operators must be followed by themself.
				ptoken->token_type = TOKEN_TYPE_KEYWORD;
				buf[0] = buf[1] = c;
				int la = fgetc(input);
				if (c != la)
					fatal("undefined operator %c%c", c, la);
				buf[2] = '\0';
				ptoken->token_value.text = xstrdup(buf);
				state = STATE_END;
				break;
			case EOF:
				ptoken->token_flag_eof = 1;
				state = STATE_END;
				break;
			default:
				if (isdigit(c)) {
					// Entering an integer.
					state = STATE_PARSE_INT;
					ptoken->token_type = TOKEN_TYPE_INT_CONST;
					buf[buflen++] = c;
				} else if (isalpha(c)) {
					// Entering an ID or keyword.
					// Unlike C, ID cannot start with an underscore.
					state = STATE_PARSE_ID;
					ptoken->token_type = TOKEN_TYPE_IDENTIFIER;
					buf[buflen++] = c;
				} else {
					// Oops.
					fatal("unrecognized character %c (ascii = %d)", c, c);
					state = STATE_END;
				}
				break;
			}
			break;
			
		case STATE_PARSE_INT:
			// We only support positive numbers.
			if (isdigit(c)) {
				buf[buflen++] = c;
			} else {
				buf[buflen] = '\0';
				
				ptoken->token_value.integer = atoi(buf);
				
				// Make sure the integer not be negative.
				if (ptoken->token_value.integer < 0) {
					warn("interger exceeds INT32_MAX (truncated)");
					ptoken->token_value.integer = INT32_MAX;
				}
				
				state = STATE_END;
				ungetc(c, input);
			}
			break;
			
		case STATE_PARSE_ID:
			if (isalnum(c) || c == '_') {
				// An id consists of letters, digits and underscores.
				buf[buflen++] = c;
			} else {
				buf[buflen] = '\0';
				
				// Maybe a keyword?
				if (hashtab_lookup(keywords, buf, NULL, 0))
					ptoken->token_type = TOKEN_TYPE_KEYWORD;
				ptoken->token_value.text = xstrdup(buf);
				
				state = STATE_END;
				ungetc(c, input);
			}
			break;
			
		case STATE_PARSE_STR:
			switch (c) {
			case '"':
				buf[buflen] = '\0';
				ptoken->token_value.text = xstrdup(buf);
				state = STATE_END;
				break;
				
			case '\\':
				// Meets an escape.
				state = STATE_PARSE_ESCAPE_IN_STR;
				break;
				
			case '\r':
			case '\n':
			case EOF:
				fatal("unexpected end of string");
				break;
			
			default:
				buf[buflen++] = c;
			}
			break;
			
		case STATE_PARSE_CHAR:
			switch (c) {
			case '\r':
			case '\n':
			case EOF:
				fatal("unexpected end of character");
				break;
			case '\\':
				// Meets an escape.
				state = STATE_PARSE_ESCAPE_IN_CHAR;
				break;
			case '\'':
				state = STATE_END;
				if (buflen != 1)
					warn("exactly one character required in a char const");
				ptoken->token_value.character = buf[0];
				break;
			default:
				buf[buflen++] = c;
				break;
			}
			break;
			
		// actually we are combining the processes
		// of the two states together here,
		// which saves much duplicate codes
		// but not violates the rules of a DFA.
		case STATE_PARSE_ESCAPE_IN_STR:
		case STATE_PARSE_ESCAPE_IN_CHAR:
			switch (c) {
			// Handle some escape characters.
			case 'n':
				buf[buflen++] = '\n';
				break;
			case 'r':
				buf[buflen++] = '\r';
				break;
			case 't':
				buf[buflen++] = '\t';
				break;
			case '\\':
			case '"':
			case '\'':
				buf[buflen++] = c;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				// in-str ascii.
				int ascii = c - '0';
				int count;
				for (count = 1; count < 3; ++count) {
					c = fgetc(input);
					if (isdigit(c)) {
						ascii = ascii * 10 + (c - '0');
					} else {
						ungetc(c, input);
						warn("ascii must be consist of three decimal digits");
						break;
					}
				}
				
				if (count < 3)
					break;
					
				if (ascii < 256)
					buf[buflen++] = ascii;
				else
					warn("ascii must be less than 256");
				break;
			}
			default:
				warn("undefined escape \\%c", c);
				break;
			}
			
			// return to the correct previous state
			// according to the current state.
			switch (state) {
			case STATE_PARSE_ESCAPE_IN_STR:
				state = STATE_PARSE_STR;
				break;
			case STATE_PARSE_ESCAPE_IN_CHAR:
				state = STATE_PARSE_CHAR;
				break;
			default:
				// how did you get in?
				assert(false);
				break;
			}
			break;
		
		case STATE_TRY_COMMENT:
			// are we going to enter a comment?
			switch (c) {
			case '/':
				// yes! it's a line comment.
				state = STATE_LINE_COMMENT;
				break;
			case '*':
				// yes! it's a block comment.
				state = STATE_BLOCK_COMMENT;
				break;
			default:
				// no! it's just a divide operator.
				ungetc(c, input);
				ptoken->token_type = TOKEN_TYPE_KEYWORD;
				ptoken->token_value.text = xstrdup(char2str('/'));
				state = STATE_END;
				break;
			}
			break;
			
		case STATE_LINE_COMMENT:
			switch (c) {
			case '\r':
				state = STATE_START;
				break;
			case '\n':
				// for the line number to be correct:
				// if we do not put back '\n', 
				// then line number would not be incremented.
				ungetc(c, input);
				state = STATE_START;
				break;
			case EOF:
				ptoken->token_flag_eof = 1;
				state = STATE_END;
				break;
			}
			break;
			
		case STATE_BLOCK_COMMENT:
			switch (c) {
			case '*':
				// maybe the block comment is ending.
				state = STATE_TRY_END_BLOCK_COMMENT;
				break;
			case '\n':
				// handle line number increment in the comment.
				lineno++;
				break;
			case EOF:
				fatal("unexpected end of block comment");
				break;
			}
			break;
			
		case STATE_TRY_END_BLOCK_COMMENT:
			// are we going to end a block comment?
			switch (c) {
			case '/':
				// yes.
				state = STATE_START;
				break;
			case '\n':
				/*
				 * handle conditions like this comment.
				 *
				 */
				lineno++;
				state = STATE_BLOCK_COMMENT;
				break;
			case EOF:
				fatal("unexpected end of block comment");
				break;
			default:
				// no! return to state STATE_BLOCK_COMMENT.
				state = STATE_BLOCK_COMMENT;
				break;
			}
			break;
			
		default:
			// unexpected state.
			assert(false);
			break;
		}
	}
}

void lex_next_token()
{
	// 'text' allocated for the previous token is no longer useful.
	if (currtok.token_type == TOKEN_TYPE_KEYWORD ||
		currtok.token_type == TOKEN_TYPE_IDENTIFIER ||
		currtok.token_type == TOKEN_TYPE_STRING_CONST)
			xfree(currtok.token_value.text);
	
	if (latok.token_flag_valid) {
		// lex_peek_token() is called.
		// Prefetched an token for us,
		// so first use that.
		currtok = latok;
		latok.token_flag_valid = 0;
	} else {
		// Fetch a token from the file stream.
		lex_get_token(&currtok);
	}

	assert(currtok.token_flag_valid);
}

void lex_peek_token()
{
	// If we have called lex_peek_token() twice or more
	// before calling lex_next_token().
	// It should not prefetch more than one token.
	if (latok.token_flag_valid == 0) {
		lex_get_token(&latok);
		latok.token_flag_valid = 1;
	}
	
	assert(latok.token_flag_valid);
}

void lex_print_all_tokens()
{
	while (true) {
		lex_next_token();
		
		if (currtok.token_flag_eof) {
			printf(">>>>> eof <<<<<\n");
			break;
		}
			
		if (!currtok.token_flag_valid) {
			printf("invalid token\n");
			break;
		}
			
		switch (currtok.token_type) {
		case TOKEN_TYPE_KEYWORD:
			printf("keyword: %s\n", currtok.token_value.text);
			break;
		case TOKEN_TYPE_IDENTIFIER:
			printf("id: %s\n", currtok.token_value.text);
			break;
		case TOKEN_TYPE_INT_CONST:
			printf("int: %d\n", currtok.token_value.integer);
			break;
		case TOKEN_TYPE_CHAR_CONST:
			printf("char: %c (ascii = %d)\n", currtok.token_value.character, currtok.token_value.character);
			break;
		case TOKEN_TYPE_STRING_CONST:
			printf("string: %s\n", currtok.token_value.text);
			break;
		}
	}
	
	// Seek to the beginning of the input file
	// for the parser to work right.
	fseek(input, 0, SEEK_SET);
}

