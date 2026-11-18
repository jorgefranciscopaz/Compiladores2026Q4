#pragma once
#include <memory>
#include <string>
#include <vector>
#include <ostream> 

// ---------- Ubicación en el código fuente ----------
struct SourceLoc {
    int line = 0;
    int column = 0;
};

// ---------- Tipo concreto de cada nodo ----------
enum class NodeKind {
    // Expr
    IntLiteral, BoolLiteral, IdentExpr, UnaryExpr, BinaryExpr, CallExpr,
    // Stmt ...
    // Decl ...
};

enum class UnaryOpKind { Negate, Not };
enum class BinaryOpKind { Add, Sub, Mul, Div, And, Or, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual };

// ---------- Clase base ----------
class Node {
public:
    virtual ~Node() = default;
    NodeKind  kind() const { return kind_; }
    SourceLoc loc()  const { return loc_; }
protected:
    Node(NodeKind k, SourceLoc l) : kind_(k), loc_(l) {}   // protegido → no se instancia directo
private:
    NodeKind  kind_;
    SourceLoc loc_;
};

//Es abstracto
class Expr : public Node{ protected: using Node::Node; };
class Stmt : public Node { protected: using Node::Node; };
class Decl : public Node { protected: using Node::Node; };


using ExprPtr = std::unique_ptr<Expr>;

class IntLiteral : public Expr{
    public:
        IntLiteral(int v, SourceLoc l) : Expr(NodeKind::IntLiteral, l), value(v) {}
        int value;
};

class BoolLiteral : public Expr{
    public:
        BoolLiteral(bool v, SourceLoc l) : Expr(NodeKind::BoolLiteral, l), value(v) {}
        bool value;
};

class IdentExpr : public Expr{
    public:
        IdentExpr(std::string n, SourceLoc l) : Expr(NodeKind::IdentExpr, l), name(std::move(n)) {}
        std::string name;
};

class UnaryExpr : public Expr{
    public:
        UnaryExpr(UnaryOpKind o, ExprPtr e, SourceLoc l) : Expr(NodeKind::UnaryExpr, l), op(o), operand(std::move(e)) {}
        UnaryOpKind op;
        ExprPtr operand;
};

class BinaryExpr : public Expr{
    public:
        BinaryExpr(BinaryOpKind o, ExprPtr l, ExprPtr r, SourceLoc lc) : Expr(NodeKind::BinaryExpr, lc), op(o), left(std::move(l)), right(std::move(r)) {}
        BinaryOpKind op;
        ExprPtr left;
        ExprPtr right;
};

class CallExpr : public Expr{
    public:
        CallExpr(ExprPtr c, std::vector<ExprPtr> a, SourceLoc l) : Expr(NodeKind::CallExpr, l), callee(std::move(c)), args(std::move(a)) {}
        ExprPtr callee;
        std::vector<ExprPtr> args;
};

const char* toString(UnaryOpKind op);
const char* toString(BinaryOpKind op);
void printAST(const Node& node, std::ostream& out, int indent = 0);
