#include "weave/syntax/SyntaxFacts.hxx"
#include "weave/bugcheck/BugCheck.hxx"

#define ENABLE_KEYWORD_BIT_OPERATORS true

namespace weave::syntax
{
    bool SyntaxFacts::IsFunctionParameterModifier(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::ParamsKeyword:
        case SyntaxKind::LazyKeyword:

        case SyntaxKind::RefKeyword:
        case SyntaxKind::OutKeyword:
        case SyntaxKind::InKeyword:
        case SyntaxKind::MoveKeyword:
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxFacts::IsMemberModifier(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::PublicKeyword:
        case SyntaxKind::PrivateKeyword:
        case SyntaxKind::InternalKeyword:
        case SyntaxKind::AsyncKeyword:
        case SyntaxKind::UnsafeKeyword:
        case SyntaxKind::CheckedKeyword:
        case SyntaxKind::DiscardableKeyword:
        case SyntaxKind::DynamicKeyword:
        case SyntaxKind::ExplicitKeyword:
        case SyntaxKind::ExportKeyword:
        case SyntaxKind::ExternKeyword:
        case SyntaxKind::FinalKeyword:
        case SyntaxKind::FixedKeyword:
        case SyntaxKind::ImplicitKeyword:
        case SyntaxKind::InlineKeyword:
        case SyntaxKind::NativeKeyword:
        case SyntaxKind::OverrideKeyword:
        case SyntaxKind::PartialKeyword:
        case SyntaxKind::PreciseKeyword:
        case SyntaxKind::PureKeyword:
        case SyntaxKind::ReadonlyKeyword:
        case SyntaxKind::RecursiveKeyword:
        case SyntaxKind::RefKeyword:
        case SyntaxKind::SynchronizedKeyword:
        case SyntaxKind::TailCallKeyword:
        case SyntaxKind::ThreadLocalKeyword:
        case SyntaxKind::TransientKeyword:
        case SyntaxKind::TrustedKeyword:
        case SyntaxKind::UnalignedKeyword:
        case SyntaxKind::UniformKeyword:
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxFacts::IsTypeQualifier(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::ConstKeyword:
        case SyntaxKind::MutableKeyword:
        case SyntaxKind::RestrictKeyword:
        case SyntaxKind::AtomicKeyword:
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxFacts::IsTypeSpecifier(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::RefKeyword:
        case SyntaxKind::OutKeyword:
        case SyntaxKind::InKeyword:
        case SyntaxKind::MoveKeyword:
            return true;

        default:
            break;
        }

        return false;
    }

    SyntaxKind SyntaxFacts::GetPrefixUnaryExpression(SyntaxKind token)
    {
        switch (token) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::PlusToken:
            return SyntaxKind::UnaryPlusExpression;

        case SyntaxKind::MinusToken:
            return SyntaxKind::UnaryMinusExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::BitComplKeyword:
#endif
        case SyntaxKind::TildeToken:
            return SyntaxKind::BitwiseNotExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::NotKeyword:
#endif
        case SyntaxKind::ExclamationToken:
            return SyntaxKind::LogicalNotExpression;

        case SyntaxKind::PlusPlusToken:
            return SyntaxKind::PreIncrementExpression;

        case SyntaxKind::MinusMinusToken:
            return SyntaxKind::PreDecrementExpression;

        case SyntaxKind::AmpersandToken:
            return SyntaxKind::AddressOfExpression;

        case SyntaxKind::AsteriskToken:
            return SyntaxKind::DereferenceExpression;

        default:
            break;
        }

        return SyntaxKind::None;
    }
    SyntaxKind SyntaxFacts::GetPostfixUnaryExpression(SyntaxKind token)
    {
        switch (token) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::PlusPlusToken:
            return SyntaxKind::PostIncrementExpression;

        case SyntaxKind::MinusMinusToken:
            return SyntaxKind::PostDecrementExpression;

        default:
            break;
        }

        return SyntaxKind::None;
    }

    SyntaxKind SyntaxFacts::GetBinaryExpression(SyntaxKind token)
    {
        switch (token) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::QuestionQuestionToken:
            return SyntaxKind::CoalesceExpression;

        case SyntaxKind::IsKeyword:
            return SyntaxKind::IsExpression;

        case SyntaxKind::AsKeyword:
            return SyntaxKind::AsExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::BitOrKeyword:
#endif
        case SyntaxKind::BarToken:
            return SyntaxKind::BitwiseOrExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::BitXorKeyword:
#endif
        case SyntaxKind::CaretToken:
            return SyntaxKind::ExclusiveOrExpression;


#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::BitAndKeyword:
#endif
        case SyntaxKind::AmpersandToken:
            return SyntaxKind::BitwiseAndExpression;

        case SyntaxKind::EqualsEqualsToken:
            return SyntaxKind::EqualsExpression;

        case SyntaxKind::ExclamationEqualsToken:
            return SyntaxKind::NotEqualsExpression;

        case SyntaxKind::LessThanToken:
            return SyntaxKind::LessThanExpression;

        case SyntaxKind::LessThanEqualsToken:
            return SyntaxKind::LessThanOrEqualExpression;

        case SyntaxKind::GreaterThanToken:
            return SyntaxKind::GreaterThanExpression;

        case SyntaxKind::GreaterThanEqualsToken:
            return SyntaxKind::GreaterThanOrEqualExpression;

        case SyntaxKind::LessThanLessThanToken:
            return SyntaxKind::LeftShiftExpression;

        case SyntaxKind::GreaterThanGreaterThanToken:
            return SyntaxKind::RightShiftExpression;

        case SyntaxKind::PlusToken:
            return SyntaxKind::AddExpression;

        case SyntaxKind::MinusToken:
            return SyntaxKind::SubtractExpression;

        case SyntaxKind::AsteriskToken:
            return SyntaxKind::MultiplyExpression;

        case SyntaxKind::SlashToken:
            return SyntaxKind::DivideExpression;

        case SyntaxKind::PercentToken:
            return SyntaxKind::ModuloExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::AndKeyword:
#endif
        case SyntaxKind::AmpersandAmpersandToken:
            return SyntaxKind::LogicalAndExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::OrKeyword:
#endif
        case SyntaxKind::BarBarToken:
            return SyntaxKind::LogicalOrExpression;
        default:
            break;
        }

        return SyntaxKind::None;
    }

    SyntaxKind SyntaxFacts::GetAssignmentExpression(SyntaxKind token)
    {
        switch (token) // NOLINT(clang-diagnostic-switch-enum)
        {
#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::OrEqualKeyword:
#endif
        case SyntaxKind::BarEqualsToken:
            return SyntaxKind::OrAssignmentExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::AndEqualKeyword:
#endif
        case SyntaxKind::AmpersandEqualsToken:
            return SyntaxKind::AndAssignmentExpression;

#if ENABLE_KEYWORD_BIT_OPERATORS
        case SyntaxKind::XorEqualKeyword:
#endif
        case SyntaxKind::CaretEqualsToken:
            return SyntaxKind::ExclusiveOrAssignmentExpression;

        case SyntaxKind::LessThanLessThanEqualsToken:
            return SyntaxKind::LeftShiftAssignmentExpression;

        case SyntaxKind::GreaterThanGreaterThanEqualsToken:
            return SyntaxKind::RightShiftAssignmentExpression;

        case SyntaxKind::PlusEqualsToken:
            return SyntaxKind::AddAssignmentExpression;

        case SyntaxKind::MinusEqualsToken:
            return SyntaxKind::SubtractAssignmentExpression;

        case SyntaxKind::AsteriskEqualsToken:
            return SyntaxKind::MultiplyAssignmentExpression;

        case SyntaxKind::SlashEqualsToken:
            return SyntaxKind::DivideAssignmentExpression;

        case SyntaxKind::PercentEqualsToken:
            return SyntaxKind::ModuloAssignmentExpression;

        case SyntaxKind::EqualsToken:
            return SyntaxKind::SimpleAssignmentExpression;

        case SyntaxKind::QuestionQuestionEqualsToken:
            return SyntaxKind::CoalesceAssignmentExpression;
        default:
            break;
        }

        return SyntaxKind::None;
    }

    bool SyntaxFacts::IsRightAssociative(SyntaxKind operation)
    {
        switch (operation) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::SimpleAssignmentExpression:
        case SyntaxKind::AddAssignmentExpression:
        case SyntaxKind::SubtractAssignmentExpression:
        case SyntaxKind::MultiplyAssignmentExpression:
        case SyntaxKind::DivideAssignmentExpression:
        case SyntaxKind::ModuloAssignmentExpression:
        case SyntaxKind::AndAssignmentExpression:
        case SyntaxKind::ExclusiveOrAssignmentExpression:
        case SyntaxKind::OrAssignmentExpression:
        case SyntaxKind::LeftShiftAssignmentExpression:
        case SyntaxKind::RightShiftAssignmentExpression:
        case SyntaxKind::CoalesceAssignmentExpression:
        case SyntaxKind::CoalesceExpression:
        case SyntaxKind::EvalExpressionSyntax:
            return true;
        default:
            break;
        }
        return false;
    }

    Precedence SyntaxFacts::GetPrecedence(SyntaxKind operation)
    {
        switch (operation) // NOLINT(clang-diagnostic-switch-enum)
        {
        // case SyntaxKind::QueryExpression:
        //     return Precedence::Expression;
        // case SyntaxKind::ParenthesizedLambdaExpression:
        // case SyntaxKind::SimpleLambdaExpression:
        // case SyntaxKind::AnonymousMethodExpression:
        //     return Precedence::Lambda;
        case SyntaxKind::SimpleAssignmentExpression:
        case SyntaxKind::AddAssignmentExpression:
        case SyntaxKind::SubtractAssignmentExpression:
        case SyntaxKind::MultiplyAssignmentExpression:
        case SyntaxKind::DivideAssignmentExpression:
        case SyntaxKind::ModuloAssignmentExpression:
        case SyntaxKind::AndAssignmentExpression:
        case SyntaxKind::ExclusiveOrAssignmentExpression:
        case SyntaxKind::OrAssignmentExpression:
        case SyntaxKind::LeftShiftAssignmentExpression:
        case SyntaxKind::RightShiftAssignmentExpression:
        case SyntaxKind::CoalesceAssignmentExpression:
            return Precedence::Assignment;
        case SyntaxKind::CoalesceExpression:
            return Precedence::Coalescing;
        case SyntaxKind::LogicalOrExpression:
            return Precedence::ConditionalOr;
        case SyntaxKind::LogicalAndExpression:
            return Precedence::ConditionalAnd;
        case SyntaxKind::BitwiseOrExpression:
            return Precedence::LogicalOr;
        case SyntaxKind::ExclusiveOrExpression:
            return Precedence::LogicalXor;
        case SyntaxKind::BitwiseAndExpression:
            return Precedence::LogicalAnd;
        case SyntaxKind::EqualsExpression:
        case SyntaxKind::NotEqualsExpression:
            return Precedence::Equality;
        case SyntaxKind::LessThanExpression:
        case SyntaxKind::LessThanOrEqualExpression:
        case SyntaxKind::GreaterThanExpression:
        case SyntaxKind::GreaterThanOrEqualExpression:
        case SyntaxKind::IsExpression:
        case SyntaxKind::AsExpression:
            return Precedence::Relational;
        // case SyntaxKind::SwitchExpression:
        // case SyntaxKind::WithExpression:
        //     return Precedence::Switch;
        case SyntaxKind::LeftShiftExpression:
        case SyntaxKind::RightShiftExpression:
            return Precedence::Shift;
        case SyntaxKind::AddExpression:
        case SyntaxKind::SubtractExpression:
            return Precedence::Additive;
        case SyntaxKind::MultiplyExpression:
        case SyntaxKind::DivideExpression:
        case SyntaxKind::ModuloExpression:
            return Precedence::Multiplicative;
        case SyntaxKind::UnaryPlusExpression:
        case SyntaxKind::UnaryMinusExpression:
        case SyntaxKind::BitwiseNotExpression:
        case SyntaxKind::LogicalNotExpression:
        case SyntaxKind::PreIncrementExpression:
        case SyntaxKind::PreDecrementExpression:
        case SyntaxKind::SizeOfExpression:
        case SyntaxKind::TypeOfExpression:
        case SyntaxKind::NameOfExpression:
            // TODO: Support for await, checked, unchecked
            return Precedence::Unary;
        // case SyntaxKind::CastExpression:
        //     return Precedence::Cast;
        case SyntaxKind::DereferenceExpression:
            return Precedence::Dereference;
        case SyntaxKind::AddressOfExpression:
            return Precedence::AddressOf;
        case SyntaxKind::ConditionalExpression:
            return Precedence::Expression;
        case SyntaxKind::CharacterLiteralExpression:
        case SyntaxKind::FalseLiteralExpression:
        case SyntaxKind::GenericNameSyntax:
        case SyntaxKind::IdentifierNameSyntax:
        case SyntaxKind::InvocationExpression:
        case SyntaxKind::IntegerLiteralExpression:
        case SyntaxKind::FloatLiteralExpression:
        case SyntaxKind::PointerMemberAccessExpression:
        case SyntaxKind::PostDecrementExpression:
        case SyntaxKind::PostIncrementExpression:
        case SyntaxKind::SimpleMemberAccessExpression:
        case SyntaxKind::StringLiteralExpression:
        case SyntaxKind::TrueLiteralExpression:
            // case SyntaxKind::CollectionExpression:
            // case SyntaxKind::ConditionalAccessExpression:
            // case SyntaxKind::DeclarationExpression:
            // case SyntaxKind::DefaultExpression:
            // case SyntaxKind::DefaultLiteralExpression:
            // case SyntaxKind::ElementAccessExpression:
            // TODO: Support for tuples
            // TODO: Support for array expressions
            // TODO: Support for stack allocated expressions (alloca)?
            // TODO: Interpolated strings
            // TODO: Null literal?
            return Precedence::Primary;

        default:
            WEAVE_BUGCHECK("Unexpected precedence value");
        }
    }

    bool SyntaxFacts::IsName(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::IdentifierNameSyntax:
        case SyntaxKind::GenericNameSyntax:
        case SyntaxKind::QualifiedNameSyntax:
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxFacts::IsInvalidSubexpression(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::BreakKeyword:
        case SyntaxKind::ContinueKeyword:
        case SyntaxKind::CaseKeyword:
        case SyntaxKind::ConstKeyword:
        case SyntaxKind::ReturnKeyword:
        case SyntaxKind::WhileKeyword:
        case SyntaxKind::DoKeyword:
        case SyntaxKind::ForKeyword:
        case SyntaxKind::SwitchKeyword:
        case SyntaxKind::UsingKeyword:
        case SyntaxKind::VarKeyword:
        case SyntaxKind::GotoKeyword:
        // case SyntaxKind::IfKeyword:
        // case SyntaxKind::ElseKeyword:
        case SyntaxKind::TryKeyword:
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxFacts::IsStartOfStatement(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::ReturnKeyword:
        case SyntaxKind::BreakKeyword:
        case SyntaxKind::ContinueKeyword:
        case SyntaxKind::WhileKeyword:
        case SyntaxKind::GotoKeyword:
        case SyntaxKind::OpenBraceToken:
        case SyntaxKind::SemicolonToken:
            return true;

        default:
            return false;
        }
    }

    bool SyntaxFacts::IsStartOfDeclaration(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::UsingKeyword:
        case SyntaxKind::FunctionKeyword:
        case SyntaxKind::StructKeyword:
        case SyntaxKind::ConceptKeyword:
        case SyntaxKind::ExtendKeyword:
        case SyntaxKind::VarKeyword:
        case SyntaxKind::LetKeyword:
        case SyntaxKind::NamespaceKeyword:
        case SyntaxKind::TypeKeyword:
        case SyntaxKind::EnumKeyword:
        case SyntaxKind::DelegateKeyword:
        case SyntaxKind::ConstKeyword:
            return true;

        default:
            return false;
        }
    }

    bool SyntaxFacts::IsStartOfExpression(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::TypeOfKeyword:
        case SyntaxKind::SizeOfKeyword:
        case SyntaxKind::AlignOfKeyword:
        case SyntaxKind::NameOfKeyword:
        case SyntaxKind::AddressOfKeyword:
        // case SyntaxKind::DefaultKeyword:
        case SyntaxKind::FalseKeyword:
        case SyntaxKind::TrueKeyword:
        case SyntaxKind::IntegerLiteralToken:
        case SyntaxKind::FloatLiteralToken:
        case SyntaxKind::StringLiteralToken:
        case SyntaxKind::CharacterLiteralToken:
        case SyntaxKind::OpenParenToken:
        case SyntaxKind::OpenBracketToken:
        case SyntaxKind::IdentifierToken:
        case SyntaxKind::SelfKeyword:
        case SyntaxKind::UnreachableKeyword:
        case SyntaxKind::EvalKeyword:
        case SyntaxKind::IfKeyword:
        case SyntaxKind::MatchKeyword:
        case SyntaxKind::AssertKeyword:
        case SyntaxKind::LetKeyword:
            return true;

        default:
            if (GetPostfixUnaryExpression(kind) != SyntaxKind::None)
            {
                return true;
            }

            if (GetPrefixUnaryExpression(kind) != SyntaxKind::None)
            {
                return true;
            }

            if (GetBinaryExpression(kind) != SyntaxKind::None)
            {
                return true;
            }

            if (GetAssignmentExpression(kind) != SyntaxKind::None)
            {
                return true;
            }

            return false;
        }
    }
}
