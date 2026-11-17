#include "Lexer.hpp"

#include <string>

#include "NotImplemented.hpp"

// =============================================================================
// Keyword table
//
// The DFA has ONE accepting state for "identifier or keyword". After it has read a whole
// identifier, look the lexeme up here to decide whether it is one of the 13 keywords of
// MiniLang or a plain identifier. Write the function and call it from the state that
// accepts identifiers. Keywords are case-sensitive.
//
//     static TokenId identifierKind(const std::string& text);
//
// (Use any lookup you like: a chain of ifs, std::unordered_map, std::array of pairs, ...)
// =============================================================================
static TokenId identifierKind(const std::string& text)
{
    if(text == "func"){
        return TokenId::KeywordFunc;
    }else if(text == "int"){
        return TokenId::KeywordInt;
    }else if(text == "bool"){
        return TokenId::KeywordBool;
    }else if(text == "void"){
        return TokenId::KeywordVoid;
    }else if(text == "if"){
        return TokenId::KeywordIf;
    }else if(text == "else"){
        return TokenId::KeywordElse;
    }else if(text == "while"){
        return TokenId::KeywordWhile;
    }else if(text == "for"){
        return TokenId::KeywordFor;
    }else if(text == "return"){
        return TokenId::KeywordReturn;
    }else if(text == "print"){
        return TokenId::KeywordPrint;
    }else if(text == "read"){
        return TokenId::KeywordRead;
    }else if(text == "true"){
        return TokenId::KeywordTrue;
    }else if(text == "false"){
        return TokenId::KeywordFalse;
    }
    else{
        return TokenId::Ident;
    }
}
// =============================================================================
// The DFA, state by state
//
// Implement one `case` per state of your DFA (docs/MiniLang.numbered.svg), numbered exactly
// as in that picture, and write the state number in a comment ON THE SAME LINE as the case
// label so that docs/TRACEABILITY.md and `ctest -R docs.traceability` can find it:
//
//         case 7:             // S7  '<' (accepting): '<=' if '=' follows
//             ...
//
// Conventions of the class samples (LexerSample1 / ParserSample1):
//   * `ch` is the current character, NOT yet consumed (EOF at the end of the input).
//     `nextChar()` consumes it.
//   * `text` accumulates the characters of the lexeme; add a character (and call
//     nextChar()) exactly when the DFA follows an arrow.
//   * A state with no arrow for `ch` ends the token WITHOUT consuming `ch`:
//       - accepting state     -> `return {TokenId::..., text, startLine, startColumn};`
//       - non-accepting state -> `return {TokenId::Unknown, text, startLine, startColumn};`
//   * State 0 is re-entered after whitespace and comments: it must (re)start the lexeme
//     (clear `text`) and (re)capture the position of the token's first character.
//   * In state 0, EOF returns TokenId::EndOfFile; a character with no arrow returns a
//     one-character TokenId::Unknown (consume it first).
// =============================================================================
Token Lexer::nextToken()
{
    std::string text;                           // the lexeme read so far
    int state = 0;                              // current DFA state (a node of the SVG)
    int startLine = line;      // position of the first character of the token
    int startColumn = column;

    while (true) {
        switch (state) {
            case 0:   // S0  start: (re)starts the lexeme and its position
                text.clear();
                startLine = line;
                startColumn = column;
                if(ch == '\t' || ch == '\n' || ch == '\r' || ch == ' '){
                    state = 3;
                    nextChar();
                }else if(ch == '!'){
                    text += ch;
                    state = 11;
                    nextChar();
                }else if(ch == '&'){
                    text += ch;
                    state = 12;
                    nextChar();
                }else if(ch == '('){
                    text += ch;
                    state = 14;
                    nextChar();
                }else if(ch == ')'){
                    text += ch;
                    state = 15;
                    nextChar();
                }else if(ch == '*'){
                    text += ch;
                    state = 7;
                    nextChar();
                }else if(ch == '+'){
                    text += ch;
                    state = 5;
                    nextChar();
                }else if(ch == ','){
                    text += ch;
                    state = 18;
                    nextChar();
                }else if(ch == '-'){
                    text += ch;
                    state = 6;
                    nextChar();
                }else if(ch == '/'){
                    text += ch;
                    state = 4;
                    nextChar();
                }else if(ch >= '0' && ch <= '9'){
                    text += ch;
                    state = 2;
                    nextChar();
                }else if(ch == ';'){
                    text += ch;
                    state = 19;
                    nextChar();
                }else if(ch == '<'){
                    text += ch;
                    state = 8;
                    nextChar();
                }else if(ch == '='){
                    text += ch;
                    state = 10;
                    nextChar();
                }else if(ch == '>'){
                    text += ch;
                    state = 9;
                    nextChar();
                }else if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_'){
                    text += ch;
                    state = 1;
                    nextChar();
                }else if(ch == '{'){
                    text += ch;
                    state = 16;
                    nextChar();
                }else if(ch == '|'){
                    text += ch;
                    state = 13;
                    nextChar();
                }else if(ch == '}'){
                    text += ch;
                    state = 17;
                    nextChar();
                }else if(ch == EOF){
                    return {TokenId::EndOfFile, text, startLine, startColumn};
                }else{
                    text += ch;
                    nextChar();
                    return {TokenId::Unknown, text, startLine, startColumn};
                }
                break;

            case 1:   // S1  identifier or keyword [1]
                if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_'){
                    text += ch;
                    state = 1;
                    nextChar();
                }else{
                    return {identifierKind(text), text, startLine, startColumn};
                }
                break;

            case 2:   // S2  number [2]
                if(ch >= '0' && ch <= '9'){
                    text += ch;
                    state = 2;
                    nextChar();
                }else{
                    return {TokenId::Number, text, startLine, startColumn};
                }
                break;

            case 3:   // S3  whitespace [3], ignored
                if(ch == '\t' || ch == '\n' || ch == '\r' || ch == ' '){
                    state = 3;
                    nextChar();
                }else{
                    state = 0;
                }
                break;

            case 4:   // S4  '/' [8]: '//' comment if '/' follows
                if(ch == '/'){
                    text += ch;
                    state = 20;
                    nextChar();
                }else{
                    return {TokenId::OpDiv, text, startLine, startColumn};
                }
                break;

            case 5:   // S5  '+' [5]
                return {TokenId::OpAdd, text, startLine, startColumn};

            case 6:   // S6  '-' [6]
                return {TokenId::OpSub, text, startLine, startColumn};

            case 7:   // S7  '*' [7]
                return {TokenId::OpMul, text, startLine, startColumn};

            case 8:   // S8  '<' [9]: '<=' if '=' follows
                if(ch == '='){
                    text += ch;
                    state = 21;
                    nextChar();
                }else{
                    return {TokenId::OpLess, text, startLine, startColumn};
                }
                break;

            case 9:   // S9  '>' [11]: '>=' if '=' follows
                if(ch == '='){
                    text += ch;
                    state = 22;
                    nextChar();
                }else{
                    return {TokenId::OpGreater, text, startLine, startColumn};
                }
                break;

            case 10:  // S10 '=' [18]: '==' if '=' follows
                if(ch == '='){
                    text += ch;
                    state = 23;
                    nextChar();
                }else{
                    return {TokenId::Assign, text, startLine, startColumn};
                }
                break;

            case 11:  // S11 '!' [17]: '!=' if '=' follows
                if(ch == '='){
                    text += ch;
                    state = 24;
                    nextChar();
                }else{
                    return {TokenId::OpNot, text, startLine, startColumn};
                }
                break;

            case 12:  // S12 '&' (not accepting): '&&' if '&' follows
                if(ch == '&'){
                    text += ch;
                    state = 25;
                    nextChar();
                }else{
                    return {TokenId::Unknown, text, startLine, startColumn};
                }
                break;

            case 13:  // S13 '|' (not accepting): '||' if '|' follows
                if(ch == '|'){
                    text += ch;
                    state = 26;
                    nextChar();
                }else{
                    return {TokenId::Unknown, text, startLine, startColumn};
                }
                break;

            case 14:  // S14 '(' [19]
                return {TokenId::LParen, text, startLine, startColumn};

            case 15:  // S15 ')' [20]
                return {TokenId::RParen, text, startLine, startColumn};

            case 16:  // S16 '{' [21]
                return {TokenId::LBrace, text, startLine, startColumn};

            case 17:  // S17 '}' [22]
                return {TokenId::RBrace, text, startLine, startColumn};

            case 18:  // S18 ',' [23]
                return {TokenId::Comma, text, startLine, startColumn};

            case 19:  // S19 ';' [24]
                return {TokenId::Semicolon, text, startLine, startColumn};

            case 20:  // S20 '//' comment [4], ignored: stops AT the line feed or EOF
                if(ch != '\n' && ch != EOF){
                    state = 20;
                    nextChar();
                }else{
                    state = 0;
                }
                break;

            case 21:  // S21 '<=' [10]
                return {TokenId::OpLessEqual, text, startLine, startColumn};

            case 22:  // S22 '>=' [12]
                return {TokenId::OpGreaterEqual, text, startLine, startColumn};

            case 23:  // S23 '==' [13]
                return {TokenId::OpEqual, text, startLine, startColumn};

            case 24:  // S24 '!=' [14]
                return {TokenId::OpNotEqual, text, startLine, startColumn};

            case 25:  // S25 '&&' [15]
                return {TokenId::OpAnd, text, startLine, startColumn};

            case 26:  // S26 '||' [16]
                return {TokenId::OpOr, text, startLine, startColumn};

        default:
            // A state of the DFA that you have not implemented yet.
            throw NotImplementedError("Lexer::nextToken, DFA state " + std::to_string(state));
        }
    }
}

// -----------------------------------------------------------------------------
// Provided: do not modify. Names must match the enumerators (the tests print them).
// -----------------------------------------------------------------------------
const char *Lexer::tokenToString(TokenId tid)
{
    switch (tid) {
    case TokenId::Unknown:        return "Unknown";
    case TokenId::EndOfFile:      return "EndOfFile";
    case TokenId::Ident:          return "Ident";
    case TokenId::Number:         return "Number";
    case TokenId::KeywordFunc:    return "KeywordFunc";
    case TokenId::KeywordInt:     return "KeywordInt";
    case TokenId::KeywordBool:    return "KeywordBool";
    case TokenId::KeywordVoid:    return "KeywordVoid";
    case TokenId::KeywordIf:      return "KeywordIf";
    case TokenId::KeywordElse:    return "KeywordElse";
    case TokenId::KeywordWhile:   return "KeywordWhile";
    case TokenId::KeywordFor:     return "KeywordFor";
    case TokenId::KeywordReturn:  return "KeywordReturn";
    case TokenId::KeywordPrint:   return "KeywordPrint";
    case TokenId::KeywordRead:    return "KeywordRead";
    case TokenId::KeywordTrue:    return "KeywordTrue";
    case TokenId::KeywordFalse:   return "KeywordFalse";
    case TokenId::OpAdd:          return "OpAdd";
    case TokenId::OpSub:          return "OpSub";
    case TokenId::OpMul:          return "OpMul";
    case TokenId::OpDiv:          return "OpDiv";
    case TokenId::OpLess:         return "OpLess";
    case TokenId::OpLessEqual:    return "OpLessEqual";
    case TokenId::OpGreater:      return "OpGreater";
    case TokenId::OpGreaterEqual: return "OpGreaterEqual";
    case TokenId::OpEqual:        return "OpEqual";
    case TokenId::OpNotEqual:     return "OpNotEqual";
    case TokenId::OpAnd:          return "OpAnd";
    case TokenId::OpOr:           return "OpOr";
    case TokenId::OpNot:          return "OpNot";
    case TokenId::Assign:         return "Assign";
    case TokenId::LParen:         return "LParen";
    case TokenId::RParen:         return "RParen";
    case TokenId::LBrace:         return "LBrace";
    case TokenId::RBrace:         return "RBrace";
    case TokenId::Comma:          return "Comma";
    case TokenId::Semicolon:      return "Semicolon";
    }
    return "?";
}
