#include "ast.hh"

#include <print>

std::string
ASTNode::to_string() const
{
    switch (m_kind) {
        case ASTKind::Unknown: return "Unknown";
        case ASTKind::NumericExpr: return "NumericExpr";
        case ASTKind::BinaryExpr: return "BinaryExpr";
        case ASTKind::Identifier: return "Identifier";
        case ASTKind::FunctionDeclaration: return "FunctionDeclaration";
        case ASTKind::FunctionCall: return "FunctionCall";
        case ASTKind::VariableAssignment: return "VariableAssignment";
        case ASTKind::Keyword: return "Keyword";
        case ASTKind::Print: return "Print";
    }
}

void dump_node(ASTPtr node, uint8_t depth, bool has_child)
{
    if (depth > 0) {
        std::print("\n");
        int spaces = depth * 4 - depth;
        for (int i = 0; i < spaces; ++i) {
            std::print(" ");
        }

        if (has_child) {
            std::print("└──│[{}] ", node->to_string());
        } else {
            std::print("└───[{}] ", node->to_string());
        }
    }

    switch (node->kind()) {
        case ASTKind::Identifier: {
            auto ident = dynamic_pointer_cast<ASTIdentifier>(node);
            std::print("{}", ident->name());
            break;
        }
        case ASTKind::NumericExpr: {
            auto value = dynamic_pointer_cast<ASTNumericExpr>(node);
            std::print("{}", value->literal());
            break;
        }
        case ASTKind::BinaryExpr: {
            auto binop = dynamic_pointer_cast<ASTBinaryOp>(node);
            dump_node(binop->left(), 0, false);
            std::print(" {} ", binop->op().literal());
            dump_node(binop->right(), 0, false);
            break;
        }
        case ASTKind::VariableAssignment: {
            auto decl = dynamic_pointer_cast<ASTVariableAssignment>(node);
            auto ident = std::make_shared<ASTIdentifier>(decl->identifier());
            dump_node(ident, depth+1, false);
            dump_node(decl->expression(), depth+1, false);
            break;
        }
        case ASTKind::FunctionDeclaration: {
            auto decl = dynamic_pointer_cast<ASTFunctionDeclaration>(node);
            auto func_name = std::make_shared<ASTIdentifier>(decl->func_name());
            auto body = decl->body();

            dump_node(func_name, depth+1, true);
            
            for (const auto& expr : body) {
                dump_node(expr, depth+1, true);
            }

            break;
        }
        default:
            break;
    }
}