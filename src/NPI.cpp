#include "NPI.hpp"

const char* toString(UnaryOpKind op){
    switch(op){
        case UnaryOpKind::Negate: return "Negate";
        case UnaryOpKind::Not: return "Not";
    }
    return "?";
}

const char* toString(BinaryOpKind op){
    switch(op){
        case BinaryOpKind::Add: return "Add";
        case BinaryOpKind::Sub: return "Sub";
        case BinaryOpKind::Mul: return "Mul";
        case BinaryOpKind::Div: return "Div";
        case BinaryOpKind::Less: return "Less";
        case BinaryOpKind::LessEqual: return "LessEqual";
        case BinaryOpKind::Greater: return "Greater";
        case BinaryOpKind::GreaterEqual: return "GreaterEqual";
        case BinaryOpKind::Equal: return "Equal";
        case BinaryOpKind::NotEqual: return "NotEqual";
        case BinaryOpKind::And: return "And";
        case BinaryOpKind::Or: return "Or";
    }
    return "?";
}

void printAST(const Node& node, std::ostream& out, int indent){
    std::string pad(indent * 2, ' ');
    out << pad;
    switch(node.kind()){
        case NodeKind::IntLiteral: {
            auto& n = static_cast<const IntLiteral&>(node);
            out << "IntLiteral(" << n.value << ")\n";
            break;
        }

        case NodeKind::BoolLiteral: {
            auto& n = static_cast<const BoolLiteral&>(node);
            out << "BoolLiteral(" << (n.value ? "true" : "false") << ")\n";
            break;
        }

        case NodeKind::IdentExpr: {
            auto& n = static_cast<const IdentExpr&>(node);
            out << "IdentExpr(" << n.name << ")\n";
            break;
        }

        case NodeKind::UnaryExpr: {
            auto& n = static_cast<const UnaryExpr&>(node);
            out << "UnaryExpr(" << toString(n.op) << ")\n";
            if(n.operand) printAST(*n.operand, out, indent + 1);
            break;
        }
        
        case NodeKind::BinaryExpr: {
            auto& n = static_cast<const BinaryExpr&>(node);
            out << "BinaryExpr(" << toString(n.op) << ")\n";
            if(n.left) printAST(*n.left, out, indent + 1);
            if(n.right) printAST(*n.right, out, indent + 1);
            break;
        }

        case NodeKind::CallExpr: {
            auto& n = static_cast<const CallExpr&>(node);
            out << "CallExpr\n";
            for(const auto& arg : n.args){
                if(arg) printAST(*arg, out, indent + 1);
            }
            break;
        }
    }
}
