
using namespace std;
using namespace clang;
using namespace TransformationUtility;

class RedundantConditionalOperatorRule
{
private:
    template<typename nodeType>
    nodeType* extractFromImplicitCastExpr(Expr *fromExpr)
    {
        ImplicitCastExpr *implicitCastExpr = dyn_cast_or_null<ImplicitCastExpr>(fromExpr);
        if (implicitCastExpr)
        {
            return dyn_cast_or_null<nodeType>(implicitCastExpr->getSubExpr());
        }
        return nullptr;
    }

    bool isCXXBoolNotEquals(Expr *trueExpr, Expr *falseExpr)
    {
        CXXBoolLiteralExpr *trueCXXBool = dyn_cast_or_null<CXXBoolLiteralExpr>(trueExpr);
        CXXBoolLiteralExpr *falseCXXBool = dyn_cast_or_null<CXXBoolLiteralExpr>(falseExpr);
        return trueCXXBool && falseCXXBool && trueCXXBool->getValue() != falseCXXBool->getValue();
    }

    bool isCXXBoolEquals(Expr *trueExpr, Expr *falseExpr)
    {
        CXXBoolLiteralExpr *trueCXXBool = dyn_cast_or_null<CXXBoolLiteralExpr>(trueExpr);
        CXXBoolLiteralExpr *falseCXXBool = dyn_cast_or_null<CXXBoolLiteralExpr>(falseExpr);
        return trueCXXBool && falseCXXBool && trueCXXBool->getValue() == falseCXXBool->getValue();
    }

    // TODO: we need to leverage C++11 lambda expressions to reduce number of methods here

    bool isObjCBOOLNotEquals(Expr *trueExpr, Expr *falseExpr)
    {
        ObjCBoolLiteralExpr *trueObjCBOOL =
            extractFromImplicitCastExpr<ObjCBoolLiteralExpr>(trueExpr);
        ObjCBoolLiteralExpr *falseObjCBOOL =
            extractFromImplicitCastExpr<ObjCBoolLiteralExpr>(falseExpr);
        return trueObjCBOOL && falseObjCBOOL &&
                trueObjCBOOL->getValue() != falseObjCBOOL->getValue();
    }

    bool isObjCBOOLEquals(Expr *trueExpr, Expr *falseExpr)
    {
        ObjCBoolLiteralExpr *trueObjCBOOL =
            extractFromImplicitCastExpr<ObjCBoolLiteralExpr>(trueExpr);
        ObjCBoolLiteralExpr *falseObjCBOOL =
            extractFromImplicitCastExpr<ObjCBoolLiteralExpr>(falseExpr);
        return trueObjCBOOL && falseObjCBOOL &&
                trueObjCBOOL->getValue() == falseObjCBOOL->getValue();
    }

    bool isObjCIntegerLiteralBOOLNotEquals(Expr *trueExpr, Expr *falseExpr)
    {
        CStyleCastExpr *trueObjCBOOL = extractFromImplicitCastExpr<CStyleCastExpr>(trueExpr);
        CStyleCastExpr *falseObjCBOOL = extractFromImplicitCastExpr<CStyleCastExpr>(falseExpr);
        if (trueObjCBOOL && falseObjCBOOL)
        {
            IntegerLiteral *trueInteger =
                dyn_cast_or_null<IntegerLiteral>(trueObjCBOOL->getSubExpr());
            IntegerLiteral *falseInteger =
                dyn_cast_or_null<IntegerLiteral>(falseObjCBOOL->getSubExpr());
            return trueInteger && falseInteger && trueInteger->getValue().getBoolValue() !=
                falseInteger->getValue().getBoolValue();
        }
        return false;
    }

    // TODO: use C++11 lambda to remove the duplicated logic between this method and above method
    bool isObjCIntegerLiteralBOOLEquals(Expr *trueExpr, Expr *falseExpr)
    {
        CStyleCastExpr *trueObjCBOOL = extractFromImplicitCastExpr<CStyleCastExpr>(trueExpr);
        CStyleCastExpr *falseObjCBOOL = extractFromImplicitCastExpr<CStyleCastExpr>(falseExpr);
        if (trueObjCBOOL && falseObjCBOOL)
        {
            IntegerLiteral *trueInteger =
                dyn_cast_or_null<IntegerLiteral>(trueObjCBOOL->getSubExpr());
            IntegerLiteral *falseInteger =
                dyn_cast_or_null<IntegerLiteral>(falseObjCBOOL->getSubExpr());
            return trueInteger && falseInteger && trueInteger->getValue().getBoolValue() ==
                falseInteger->getValue().getBoolValue();
        }
        return false;
    }

    bool isIntegerLiteralEquals(Expr *trueExpr, Expr *falseExpr)
    {
        IntegerLiteral *trueInteger = dyn_cast_or_null<IntegerLiteral>(trueExpr);
        IntegerLiteral *falseInteger = dyn_cast_or_null<IntegerLiteral>(falseExpr);
        return trueInteger && falseInteger && trueInteger->getValue() == falseInteger->getValue();
    }

    bool isFloatingLiteralEquals(Expr *trueExpr, Expr *falseExpr)
    {
        FloatingLiteral *trueFloating = dyn_cast_or_null<FloatingLiteral>(trueExpr);
        FloatingLiteral *falseFloating = dyn_cast_or_null<FloatingLiteral>(falseExpr);
        return trueFloating && falseFloating &&
            trueFloating->getValueAsApproximateDouble() ==
                falseFloating->getValueAsApproximateDouble();
    }

    bool isCharacterLiteralEquals(Expr *trueExpr, Expr *falseExpr)
    {
        CharacterLiteral *trueChar = dyn_cast_or_null<CharacterLiteral>(trueExpr);
        CharacterLiteral *falseChar = dyn_cast_or_null<CharacterLiteral>(falseExpr);
        return trueChar && falseChar && trueChar->getValue() == falseChar->getValue();
    }

    bool isStringLiteralEquals(Expr *trueExpr, Expr *falseExpr)
    {
        StringLiteral *trueStr = extractFromImplicitCastExpr<StringLiteral>(trueExpr);
        StringLiteral *falseStr = extractFromImplicitCastExpr<StringLiteral>(falseExpr);
        return trueStr && falseStr && trueStr->getString().equals(falseStr->getString());
    }

    bool isNotEquals(Expr *trueExpr, Expr *falseExpr)
    {
        return isCXXBoolNotEquals(trueExpr, falseExpr) ||
            isObjCBOOLNotEquals(trueExpr, falseExpr) ||
            isObjCIntegerLiteralBOOLNotEquals(trueExpr, falseExpr);
    }

    bool isSameConstant(Expr *trueExpr, Expr *falseExpr)
    {
        return isCXXBoolEquals(trueExpr, falseExpr) ||
            isObjCBOOLEquals(trueExpr, falseExpr) ||
            isObjCIntegerLiteralBOOLEquals(trueExpr, falseExpr) ||
            isIntegerLiteralEquals(trueExpr, falseExpr) ||
            isFloatingLiteralEquals(trueExpr, falseExpr) ||
            isCharacterLiteralEquals(trueExpr, falseExpr) ||
            isStringLiteralEquals(trueExpr, falseExpr);
    }

    bool isSameVariable(Expr *trueExpr, Expr *falseExpr, ASTContext* context)
    {
        DeclRefExpr *trueDeclRef = extractFromImplicitCastExpr<DeclRefExpr>(trueExpr);
        DeclRefExpr *falseDeclRef = extractFromImplicitCastExpr<DeclRefExpr>(falseExpr);
        return trueDeclRef && falseDeclRef && areSameExpr(context, trueDeclRef, falseDeclRef);
    }

public:

    enum class ViolationType{
	RedundantCondition,
	SameValue,
	NoViolation
    };

    ViolationType VisitConditionalOperator(ConditionalOperator *conditionalOperator, ASTContext* context)
    {
        // There are three types of violations: 1. true expression and false expression
        // are returning true/false or false/true respectively; 2. true expression and false
        // expression are the same constant; 3. true expression and false expression are the
        // same variable expression.
        // The first case can be replaced by a simple boolean expression, and the
        // second/third cases can be replaced by constant or variable expression.

        Expr *trueExpression = conditionalOperator->getTrueExpr();
        Expr *falseExpression = conditionalOperator->getFalseExpr();
        if (isNotEquals(trueExpression, falseExpression) )
        {
            //addViolation(conditionalOperator, this);
	    return ViolationType::RedundantCondition;
        }
	if ( isSameConstant(trueExpression, falseExpression) ) {
	    return ViolationType::SameValue;
	}
	if ( isSameVariable(trueExpression, falseExpression, context) ) {
	    return ViolationType::SameValue;
	}

        return ViolationType::NoViolation;
    }
};
