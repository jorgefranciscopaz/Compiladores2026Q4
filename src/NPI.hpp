#pragma once
#include <memory>
#include <string>
#include <vector>
#include <ostream>
#include <stdexcept>

// ---------- Ubicación en el código fuente ----------
struct SourceLoc
{
    int line = 0;
    int column = 0;
};

// ---------- Tipo concreto de cada nodo ----------
enum class NodeKind
{
    IntLiteral,
    BoolLiteral,
    IdentExpr,
    UnaryExpr,
    BinaryExpr,
    CallExpr
};

enum class UnaryOpKind
{
    Negate,
    Not
};
enum class BinaryOpKind
{
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual
};

// ---------- Clase base ----------
class Node
{
public:
    virtual ~Node() = default;
    Node(const Node &) = delete; // los nodos no se copian
    Node &operator=(const Node &) = delete;

    NodeKind kind() const { return kind_; }
    SourceLoc loc() const { return loc_; }

    // Conversiones seguras sin RTTI 
    template <class T>
    bool is() const { return kind_ == T::Kind; }
    template <class T>
    const T &as() const { return static_cast<const T &>(*this); }

protected:
    Node(NodeKind k, SourceLoc l) : kind_(k), loc_(l) {} // protegido → no se instancia directo
private:
    NodeKind kind_;
    SourceLoc loc_;
};

// Es abstracto
class Expr : public Node
{
protected:
    using Node::Node;
};

using ExprPtr = std::unique_ptr<Expr>;

class IntLiteral final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::IntLiteral;
    IntLiteral(int v, SourceLoc l) : Expr(Kind, l), value(v) {}
    int value;
};

class BoolLiteral final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::BoolLiteral;
    BoolLiteral(bool v, SourceLoc l) : Expr(Kind, l), value(v) {}
    bool value;
};

class IdentExpr final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::IdentExpr;
    IdentExpr(std::string n, SourceLoc l) : Expr(Kind, l), name(std::move(n)) {}
    std::string name;
};

class UnaryExpr final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::UnaryExpr;
    UnaryExpr(UnaryOpKind o, ExprPtr e, SourceLoc l) : Expr(Kind, l), op(o), operand(std::move(e)) {}
    UnaryOpKind op;
    ExprPtr operand;
};

class BinaryExpr final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::BinaryExpr;
    BinaryExpr(BinaryOpKind o, ExprPtr l, ExprPtr r, SourceLoc lc) : Expr(Kind, lc), op(o), left(std::move(l)), right(std::move(r)) {}
    BinaryOpKind op;
    ExprPtr left;
    ExprPtr right;
};

class CallExpr final : public Expr
{
public:
    static constexpr NodeKind Kind = NodeKind::CallExpr;
    CallExpr(ExprPtr c, std::vector<ExprPtr> a, SourceLoc l) : Expr(Kind, l), callee(std::move(c)), args(std::move(a)) {}
    ExprPtr callee;
    std::vector<ExprPtr> args;
};

const char *toString(UnaryOpKind op);
const char *toString(BinaryOpKind op);
