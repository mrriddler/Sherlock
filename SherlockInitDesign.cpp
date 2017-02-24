//
//  SherlockInitDesign.cpp
//  Sherlock
//
//  Created by Robbie on 2017/2/9.
//
//

#include "SherlockInitDesign.hpp"

static int initMethodCount;

bool SherlockInitDesign::VisitStmt(Stmt *S)
{
    const ReturnStmt *RS = dyn_cast_or_null<ReturnStmt>(S);
    if (RS) {
        this->checkInitDesign(RS);
    }
    return true;
}

void SherlockInitDesign::checkInitDesign(const ReturnStmt *RS)
{
    if (RS->getRetValue()->isObjCSelfExpr()) {
        ++initMethodCount;
    }
}

bool SherlockInitDesign::isDesignatedInitializer()
{
    return initMethodCount == 1 || initMethodCount == 0 ? true : false;
}

