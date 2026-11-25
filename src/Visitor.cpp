#include "Visitor.hpp"
#include <string>

// ---------- AstPrinter ----------
void AstPrinter::pad() { out_ << std::string(indent_ * 2, ' '); }

void AstPrinter::child(const Node* n) {
    if (!n) return;
    ++indent_;
    visit(*n);
    --indent_;
}

void AstPrinter::visitIntLiteral(const IntLiteral& n) {
    pad(); out_ << "IntLiteral(" << n.value << ")\n";
}

void AstPrinter::visitBoolLiteral(const BoolLiteral& n) {
    pad(); out_ << "BoolLiteral(" << (n.value ? "true" : "false") << ")\n";
}

void AstPrinter::visitIdentExpr(const IdentExpr& n) {
    pad(); out_ << "IdentExpr(" << n.name << ")\n";
}

void AstPrinter::visitUnaryExpr(const UnaryExpr& n) {
    pad(); out_ << "UnaryExpr(" << toString(n.op) << ")\n";
    child(n.operand.get());
}

void AstPrinter::visitBinaryExpr(const BinaryExpr& n) {
    pad(); out_ << "BinaryExpr(" << toString(n.op) << ")\n";
    child(n.left.get());
    child(n.right.get());
}

void AstPrinter::visitCallExpr(const CallExpr& n) {
    pad(); out_ << "CallExpr\n";
    child(n.callee.get());
    for (const auto& arg : n.args) child(arg.get());
}

void printAST(const Node& node, std::ostream& out, int indent) {
    AstPrinter printer(out, indent);
    printer.visit(node);
}
