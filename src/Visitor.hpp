#pragma once
#include <ostream>
#include <stdexcept>
#include "Node.hpp"

// ---------- Visitor<R>: la operación vive fuera de los nodos (diapositiva 27) ----------
template <class R>
class Visitor
{
public:
    virtual ~Visitor() = default;

    // Un solo switch: kind() decide qué visitX llamar
    R visit(const Node &n)
    {
        switch (n.kind())
        {
        case NodeKind::IntLiteral:
            return visitIntLiteral(n.as<IntLiteral>());
        case NodeKind::BoolLiteral:
            return visitBoolLiteral(n.as<BoolLiteral>());
        case NodeKind::IdentExpr:
            return visitIdentExpr(n.as<IdentExpr>());
        case NodeKind::UnaryExpr:
            return visitUnaryExpr(n.as<UnaryExpr>());
        case NodeKind::BinaryExpr:
            return visitBinaryExpr(n.as<BinaryExpr>());
        case NodeKind::CallExpr:
            return visitCallExpr(n.as<CallExpr>());
        }
        throw std::logic_error("unknown node kind");
    }

    // = 0: si un visitor olvida un nodo, no compila
    virtual R visitIntLiteral(const IntLiteral &) = 0;
    virtual R visitBoolLiteral(const BoolLiteral &) = 0;
    virtual R visitIdentExpr(const IdentExpr &) = 0;
    virtual R visitUnaryExpr(const UnaryExpr &) = 0;
    virtual R visitBinaryExpr(const BinaryExpr &) = 0;
    virtual R visitCallExpr(const CallExpr &) = 0;
};

// ---------- Visitor concreto: imprime el árbol (R = void) ----------
class AstPrinter : public Visitor<void>
{
public:
    AstPrinter(std::ostream &out, int indent = 0) : out_(out), indent_(indent) {}

    void visitIntLiteral(const IntLiteral &n) override;
    void visitBoolLiteral(const BoolLiteral &n) override;
    void visitIdentExpr(const IdentExpr &n) override;
    void visitUnaryExpr(const UnaryExpr &n) override;
    void visitBinaryExpr(const BinaryExpr &n) override;
    void visitCallExpr(const CallExpr &n) override;

private:
    std::ostream &out_;
    int indent_;                // estado del visitor, no parámetro
    void pad();
    void child(const Node *n);  // visita un hijo con un nivel más de sangría
};

// ---------- Visitor concreto: cuenta nodos (R = int, diapositiva 28) ----------
class NodeCounter : public Visitor<int>
{
public:
    int visitIntLiteral(const IntLiteral &) override { return 1; }
    int visitBoolLiteral(const BoolLiteral &) override { return 1; }
    int visitIdentExpr(const IdentExpr &) override { return 1; }
    int visitUnaryExpr(const UnaryExpr &e) override { return 1 + visit(*e.operand); }
    int visitBinaryExpr(const BinaryExpr &e) override { return 1 + visit(*e.left) + visit(*e.right); }
    int visitCallExpr(const CallExpr &e) override
    {
        int n = 1 + (e.callee ? visit(*e.callee) : 0);
        for (const auto &a : e.args)
            if (a)
                n += visit(*a);
        return n;
    }
};

// Envoltorio para no romper el código existente: usa AstPrinter
void printAST(const Node &node, std::ostream &out, int indent = 0);
