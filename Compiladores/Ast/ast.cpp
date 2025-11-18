#include <sstream>
#include "ast.h"
#include "error.h"
using std::stringstream;

extern Lexer *scanner;

// ---- nodo base ----
Node::Node() : node_type(NodeType::UNKNOWN) {}
Node::Node(int t) : node_type(t) {}

// ---- statement ----
Statement::Statement() : Node(NodeType::STMT) {}
Statement::Statement(int type) : Node(type) {}

// ---- expression ----
Expression::Expression(Token *t) : Node(NodeType::EXPR), type(ExprType::VOID), token(t) {}
Expression::Expression(int ntype, int etype, Token *t) : Node(ntype), type(etype), token(t) {}

string Expression::Name() { return token->lexeme; }

string Expression::Type()
{
    switch (type)
    {
    case ExprType::INT: return "int";
    case ExprType::FLOAT: return "float";
    case ExprType::BOOL: return "bool";
    default: return "void";
    }
}

// ---- conversion int<->bool ----
// ya definido en el header como ConvertExpr

// ---- constante ----
Constant::Constant(int etype, Token *t) : Expression(NodeType::CONSTANT, etype, t) {}

// ---- identificador ----
Identifier::Identifier(int etype, Token *t) : Expression(NodeType::IDENTIFIER, etype, t) {}

// ---- acceso ----
Access::Access(int etype, Token * t, Expression * i, Expression * e): Expression(NodeType::ACCESS, etype, t), id(i), expr(e) {}

// ---- logica ----
Logical::Logical(Token *t, Expression *e1, Expression *e2) : Expression(NodeType::LOG, ExprType::BOOL, t), expr1(e1), expr2(e2)
{
    // coercion int<->bool
    if ((expr1->type == ExprType::INT && expr2->type == ExprType::BOOL))
        expr1 = new ConvertExpr(expr1, ExprType::BOOL);
    else if ((expr1->type == ExprType::BOOL && expr2->type == ExprType::INT))
        expr2 = new ConvertExpr(expr2, ExprType::BOOL);

    if (expr1->type != ExprType::BOOL || expr2->type != ExprType::BOOL)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado con operandos no booleanos ("
           << expr1->Name() << ":" << expr1->Type() << ") ("
           << expr2->Name() << ":" << expr2->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---- relacional ----
Relational::Relational(Token *t, Expression *e1, Expression *e2) : Expression(NodeType::REL, ExprType::BOOL, t), expr1(e1), expr2(e2)
{
    // coercion int<->bool
    if ((expr1->type == ExprType::INT && expr2->type == ExprType::BOOL))
        expr1 = new ConvertExpr(expr1, ExprType::BOOL);
    else if ((expr1->type == ExprType::BOOL && expr2->type == ExprType::INT))
        expr2 = new ConvertExpr(expr2, ExprType::BOOL);

    if (expr1->type != expr2->type)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado con operandos de tipos diferentes ("
           << expr1->Name() << ":" << expr1->Type() << ") ("
           << expr2->Name() << ":" << expr2->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---- aritmetica ----
Arithmetic::Arithmetic(int etype, Token *t, Expression *e1, Expression *e2) : Expression(NodeType::ARI, etype, t), expr1(e1), expr2(e2)
{
    // permitir suma/resta bool+int o int+bool
    if ((expr1->type == ExprType::INT && expr2->type == ExprType::BOOL))
        expr2 = new ConvertExpr(expr2, ExprType::INT);
    else if ((expr1->type == ExprType::BOOL && expr2->type == ExprType::INT))
        expr1 = new ConvertExpr(expr1, ExprType::INT);

    if (expr1->type != expr2->type)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado con operandos de tipos diferentes ("
           << expr1->Name() << ":" << expr1->Type() << ") ("
           << expr2->Name() << ":" << expr2->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---- unario ----
UnaryExpr::UnaryExpr(int etype, Token *t, Expression *e) : Expression(NodeType::UNARY, etype, t), expr(e)
{
    // coercion int->bool para ! operador
    if (token->lexeme == "!" && expr->type == ExprType::INT)
        expr = new ConvertExpr(expr, ExprType::BOOL);

    if (token->lexeme == "!" && expr->type != ExprType::BOOL)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado con operando no booleano ("
           << expr->Name() << ":" << expr->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---- secuencia ----
Seq::Seq(Statement *s, Statement *ss) : Statement(NodeType::SEQ), stmt(s), stmts(ss) {}

// ---- asignacion ----
Assign::Assign(Expression *i, Expression *e) : Statement(NodeType::ASSIGN), id(i), expr(e)
{
    // coercion automática int <-> bool
    if ((id->type == ExprType::BOOL && e->type == ExprType::INT) ||
        (id->type == ExprType::INT && e->type == ExprType::BOOL)) 
    {
        expr = new ConvertExpr(e, id->type);
    }
    else if (id->type != e->type) 
    {
        stringstream ss;
        ss << "\'=\' usado con operandos de tipos diferentes ("
           << id->Name() << ":" << id->Type() << ") ("
           << e->Name() << ":" << e->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---- if ----
If::If(Expression *e, Statement *s) : Statement(NodeType::IF_STMT), expr(e), stmt(s) {}

// ---- while ----
While::While(Expression *e, Statement *s) : Statement(NodeType::WHILE_STMT), expr(e), stmt(s) {}

// ---- do-while ----
DoWhile::DoWhile(Statement *s, Expression *e) : Statement(NodeType::DOWHILE_STMT), stmt(s), expr(e) {}
