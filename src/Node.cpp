#include "Node.hpp"

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
