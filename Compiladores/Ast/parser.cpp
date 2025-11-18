#include "parser.h"
#include "error.h"
#include <iostream>
#include <sstream>
#include <cctype>
using std::cin;
using std::cout;
using std::endl;
using std::stringstream;

extern Lexer * scanner;

Node *Parser::Program()
{
    // program -> int main() block
    if (!Match(Tag::TYPE))
        throw SyntaxError(scanner->Lineno(), "\'int\' esperado");

    if (!Match(Tag::MAIN))
        throw SyntaxError(scanner->Lineno(), "\'main\' esperado");

    if (!Match('('))
        throw SyntaxError(scanner->Lineno(), "\'(\' esperado");

    if (!Match(')'))
        throw SyntaxError(scanner->Lineno(), "\')\' esperado");

    return Block();
}

Statement *Parser::Block()
{
    // block -> { decls stmts }
    if (!Match('{'))
        throw SyntaxError(scanner->Lineno(), "\'{\' esperado");

    // ------------------------------------
    // nueva tabla de símbolos para el bloque
    // ------------------------------------
    SymTable *saved = symtable;
    symtable = new SymTable(symtable);
    // ------------------------------------

    Decls();
    Statement *sts = Stmts();

    if (!Match('}'))
        throw SyntaxError(scanner->Lineno(), "\'}\' esperado");

    // ------------------------------------------------------
    // tabla de ambito envolvente vuelve a ser la tabla activa
    // ------------------------------------------------------
    delete symtable;
    symtable = saved;
    // ------------------------------------------------------

    return sts;
}

void Parser::Decls()
{
    // decls -> decl decls
    //        | empty
    // decl  -> type id index;
    //
    // index -> [ integer ]
    //        | empty

    while (lookahead->tag == Tag::TYPE)
    {
        // captura nombre del tipo
        string type{lookahead->lexeme};
        Match(Tag::TYPE);

        // captura nombre del identificador
        string name{lookahead->lexeme};
        Match(Tag::ID);

        // crea símbolo
        Symbol s{name, type};

        // inserta variable en la tabla de símbolos
        if (!symtable->Insert(name, s))
        {
            // a inserción falla cuando la variable ya está en la tabla
            stringstream ss;
            ss << "variable \"" << name << "\" ya definida";
            throw SyntaxError(scanner->Lineno(), ss.str());
        }

        // verifica si es una declaración de arreglo
        if (Match('['))
        {
            if (!Match(Tag::INTEGER))
            {
                stringstream ss;
                ss << "el indice de un arreglo debe ser un valor entero";
                throw SyntaxError{scanner->Lineno(), ss.str()};
            }
            if (!Match(']'))
            {
                stringstream ss;
                ss << "esperado ] en lugar de  \'" << lookahead->lexeme << "\'";
                throw SyntaxError{scanner->Lineno(), ss.str()};
            }
        }

        // verifica punto y coma
        if (!Match(';'))
        {
            stringstream ss;
            ss << "encontrado \'" << lookahead->lexeme << "\' en  lugar de ';'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
    }
}

Statement *Parser::Stmts()
{
    // stmts -> stmt stmts
    //        | empty

    Statement *seq = nullptr;

    switch (lookahead->tag)
    {
    // stmts -> stmt stmts
    case Tag::ID:
    case Tag::IF:
    case Tag::WHILE:
    case Tag::DO:
    case '{':
    {
        Statement *st = Stmt();
        Statement *sts = Stmts();
        seq = new Seq(st, sts);
    }
    }

    // stmts -> empty
    return seq;
}

Statement *Parser::Stmt()
{
    // stmt  -> local = bool;
    //        | if (bool) stmt
    //        | while (bool) stmt
    //        | do stmt while (bool);
    //        | block

    Statement *stmt = nullptr;

    switch (lookahead->tag)
    {
    // stmt -> local = bool;
    case Tag::ID:
    {
        Expression *left = Local();
        if (!Match('='))
        {
            stringstream ss;
            ss << "esperado = en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *right = Bool();
        stmt = new Assign(left, right);
        if (!Match(';'))
        {
            stringstream ss;
            ss << "esperado ; en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        return stmt;
    }

    // stmt -> if (bool) stmt
    case Tag::IF:
    {
        Match(Tag::IF);
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Statement *inst = Stmt();
        stmt = new If(cond, inst);
        return stmt;
    }

    // stmt -> while (bool) stmt
    case Tag::WHILE:
    {
        Match(Tag::WHILE);
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Statement *inst = Stmt();
        stmt = new While(cond, inst);
        return stmt;
    }

    // stmt -> do stmt while (bool);
    case Tag::DO:
    {
        Match(Tag::DO);
        Statement *inst = Stmt();
        if (!Match(Tag::WHILE))
        {
            stringstream ss;
            ss << "esperado \'while\' en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        stmt = new DoWhile(inst, cond);
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        if (!Match(';'))
        {
            stringstream ss;
            ss << "esperado ; en lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        return stmt;
    }
    // stmt -> block
    case '{':
    {
        stmt = Block();
        return stmt;
    }
    default:
    {
        stringstream ss;
        ss << "\'" << lookahead->lexeme << "\' no inicia una instruccion válida";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
    }
}

Expression *Parser::Local()
{
    // local -> local[bool]
    //        | id

    Expression *expr = nullptr;

    switch (lookahead->tag)
    {
    case Tag::ID:
    {
        // verifica tipo de variable en la tabla de símbolos
        Symbol *s = symtable->Find(lookahead->lexeme);
        if (!s)
        {
            stringstream ss;
            ss << "variable \"" << lookahead->lexeme << "\" no declarada";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }

        // identifica el tipo de la expresión
        int etype = ExprType::VOID;
        if (s->type == "int")
            etype = ExprType::INT;
        else if (s->type == "float")
            etype = ExprType::FLOAT;
        else if (s->type == "bool")
            etype = ExprType::BOOL;

        // identificador
        expr = new Identifier(etype, new Token{*lookahead});
        Match(Tag::ID);

        // acceso a elemento de un arreglo
        if (Match('['))
        {
            expr = new Access(etype, new Token{*lookahead}, expr, Bool());

            if (!Match(']'))
            {
                stringstream ss;
                ss << "esperado ] en lugar de  \'" << lookahead->lexeme << "\'";
                throw SyntaxError{scanner->Lineno(), ss.str()};
            }
        }
        break;
    }
    default:
    {
        stringstream ss;
        ss << "esperado un local de almacenamiento (variable o arreglo)";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
    }

    return expr;
}

Expression *Parser::Bool()
{
    // bool -> join lor
    // lor  -> || join lor
    //       | empty

    Expression *expr1 = Join();

    // funcion Lor()
    while (true)
    {
        Token t = *lookahead;

        if (Match(Tag::OR))
        {
            Expression *expr2 = Join();
            expr1 = new Logical(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Join()
{
    // join -> equality land
    // land -> && equality land
    //       | empty

    Expression *expr1 = Equality();

    // funcion Land()
    while (true)
    {
        Token t = *lookahead;
        if (Match(Tag::AND))
        {
            Expression *expr2 = Equality();
            expr1 = new Logical(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Equality()
{
    // equality -> rel eqdif
    // eqdif    -> == rel eqdif
    //           | != rel eqdif
    //           | empty

    Expression *expr1 = Rel();

    // funcion Eqdif()
    while (true)
    {
        Token t = *lookahead;

        if (lookahead->tag == Tag::EQ)
        {
            Match(Tag::EQ);
            Expression *expr2 = Rel();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::NEQ)
        {
            Match(Tag::NEQ);
            Expression *expr2 = Rel();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Rel()
{
    // rel  -> ari comp
    // comp -> < ari comp
    //       | <= ari comp
    //       | > ari comp
    //       | >= ari comp
    //       | empty

    Expression *expr1 = Ari();

    // funcion Comp()
    while (true)
    {
        Token t = *lookahead;

        if (lookahead->tag == '<')
        {
            Match('<');
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::LTE)
        {
            Match(Tag::LTE);
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == '>')
        {
            Match('>');
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::GTE)
        {
            Match(Tag::GTE);
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Ari()
{
    // ari  -> term oper
    // oper -> + term oper
    //       | - term oper
    //       | empty

    Expression *expr1 = Term();

    // funcion Oper()
    while (true)
    {
        Token t = *lookahead;

        // oper -> + term oper
        if (lookahead->tag == '+')
        {
            Match('+');
            Expression *expr2 = Term();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // oper -> - term oper
        else if (lookahead->tag == '-')
        {
            Match('-');
            Expression *expr2 = Term();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // oper -> empty
        else
            break;
    }

    return expr1;
}

Expression *Parser::Term()
{
    // term -> unary calc
    // calc -> * unary calc
    //       | / unary calc
    //       | empty

    Expression *expr1 = Unary();

    // funcion Calc()
    while (true)
    {
        Token t = *lookahead;

        // calc -> * unary calc
        if (lookahead->tag == '*')
        {
            Match('*');
            Expression *expr2 = Unary();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // calc -> / unary calc
        else if (lookahead->tag == '/')
        {
            Match('/');
            Expression *expr2 = Unary();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // calc -> empty
        else
            break;
    }

    return expr1;
}

Expression *Parser::Unary()
{
    // unary -> !unary
    //        | -unary
    //        | factor

    Expression *unary = nullptr;

    // unary -> !unary
    if (lookahead->tag == '!')
    {
        Token t = *lookahead;
        Match('!');
        Expression *expr = Unary();
        unary = new UnaryExpr(ExprType::BOOL, new Token{t}, expr);
    }
    // unary -> -unary
    else if (lookahead->tag == '-')
    {
        Token t = *lookahead;
        Match('-');
        Expression *expr = Unary();
        unary = new UnaryExpr(expr->type, new Token{t}, expr);
    }
    else
    {
        unary = Factor();
    }

    return unary;
}

Expression *Parser::Factor()
{
    // factor -> (bool)
    //         | local
    //         | integer
    //         | real
    //         | true
    //         | false

    Expression *expr = nullptr;

    switch (lookahead->tag)
    {
    // factor -> (bool)
    case '(':
    {
        Match('(');
        expr = Bool();
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        break;
    }

    // factor -> local
    case Tag::ID:
    {
        expr = Local();
        break;
    }

    // factor -> integer
    case Tag::INTEGER:
    {
        expr = new Constant(ExprType::INT, new Token{*lookahead});
        Match(Tag::INTEGER);
        break;
    }

    // factor -> real
    case Tag::REAL:
    {
        expr = new Constant(ExprType::FLOAT, new Token{*lookahead});
        Match(Tag::REAL);
        break;
    }

    // factor -> true
    case Tag::TRUE:
    {
        expr = new Constant(ExprType::BOOL, new Token{*lookahead});
        Match(Tag::TRUE);
        break;
    }

    // factor -> false
    case Tag::FALSE:
    {
        expr = new Constant(ExprType::BOOL, new Token{*lookahead});
        Match(Tag::FALSE);
        break;
    }

    default:
    {
        stringstream ss;
        ss << "uma expressão é esperada no lugar de  \'" << lookahead->lexeme << "\'";
        throw SyntaxError{scanner->Lineno(), ss.str()};
        break;
    }
    }

    return expr;
}

bool Parser::Match(int tag)
{
    if (tag == lookahead->tag)
    {
        lookahead = scanner->Scan();
        return true;
    }

    return false;
}

Parser::Parser()
{
    lookahead = scanner->Scan();
    symtable = nullptr;
}

Node * Parser::Start()
{
    return Program();
}

