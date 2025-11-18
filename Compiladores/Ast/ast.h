#ifndef COMPILER_AST
#define COMPILER_AST

#include "lexer.h"
#include <string>
using std::string;

// ---- tipos de nodo ----
enum NodeType
{
    UNKNOWN,
    STMT,
    EXPR,
    CONSTANT,
    IDENTIFIER,
    ACCESS,
    LOG,
    REL,
    ARI,
    UNARY,
    SEQ,
    ASSIGN,
    IF_STMT,
    WHILE_STMT,
    DOWHILE_STMT,
    CONVERT // nuevo nodo para conversion int<->bool
};

// ---- tipos de expresion ----
enum ExprType
{
    VOID,
    INT,
    FLOAT,
    BOOL
};

// ---- nodo base ----
struct Node
{
    int node_type;
    Node();
    Node(int t);
};

// ---- statement base ----
struct Statement : public Node
{
    Statement();
    Statement(int type);
};

// ---- expression base ----
struct Expression : public Node
{
    int type;
    Token *token;
    Expression(Token *t);
    Expression(int ntype, int etype, Token *t);
    string Name();
    string Type();
};

// ---- conversion automatico int <-> bool ----
struct ConvertExpr : public Expression
{
    Expression *expr;
    ConvertExpr(Expression *e, int targetType)
        : Expression(NodeType::CONVERT, targetType, e->token), expr(e) {}
};

// ---- constantes ----
struct Constant : public Expression
{
    Constant(int etype, Token *t);
};

// ---- identificadores ----
struct Identifier : public Expression
{
    Identifier(int etype, Token *t);
};

// ---- acceso a arreglos ----
struct Access : public Expression
{
    Expression * id;
    Expression * expr;
    Access(int etype, Token * t, Expression * i, Expression * e);
};

// ---- logicas ----
struct Logical : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Logical(Token *t, Expression *e1, Expression *e2);
};

// ---- relacionales ----
struct Relational : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Relational(Token *t, Expression *e1, Expression *e2);
};

// ---- aritmeticas ----
struct Arithmetic : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Arithmetic(int etype, Token *t, Expression *e1, Expression *e2);
};

// ---- unarias ----
struct UnaryExpr : public Expression
{
    Expression *expr;
    UnaryExpr(int etype, Token *t, Expression *e);
};

// ---- secuencia de statements ----
struct Seq : public Statement
{
    Statement *stmt;
    Statement *stmts;
    Seq(Statement *s, Statement *ss);
};

// ---- asignacion ----
struct Assign : public Statement
{
    Expression *id;
    Expression *expr;
    Assign(Expression *i, Expression *e);
};

// ---- if ----
struct If : public Statement
{
    Expression *expr;
    Statement *stmt;
    If(Expression *e, Statement *s);
};

// ---- while ----
struct While : public Statement
{
    Expression *expr;
    Statement *stmt;
    While(Expression *e, Statement *s);
};

// ---- do-while ----
struct DoWhile : public Statement
{
    Statement *stmt;
    Expression *expr;
    DoWhile(Statement *s, Expression *e);
};

#endif
