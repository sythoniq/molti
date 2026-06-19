#include "common.h"
#include "scanner.h"

Scanner scanner;

static bool isAtEnd() {
				return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
				Token token;
				token.type = type;
				token.start = scanner.start;
				token.length = (int)(scanner.current - scanner.start);
				token.line = scanner.line;

				return token;
}

static Token errorToken(const char* error) {
				Token token;
				token.type = TOKEN_ERROR;
				token.start = error;
				token.length = (int)strlen(error);
				token.line = scanner.line;

				return token;
}

void initScanner(const char* source) {
				scanner.start = source;
				scanner.current = source;
				scanner.line = 1;
}

Token scanToken() {
				scanner.start = scanner.current;

				if (!isAtEnd()) return makeToken(TOKEN_EOF);

				return errorToken("Unexpected character.");
}
