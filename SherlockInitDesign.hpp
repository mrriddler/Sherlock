//
//  SherlockInitDesign.hpp
//  Sherlock
//
//  Created by Robbie on 2017/2/9.
//
//

#ifndef SherlockInitDesign_hpp
#define SherlockInitDesign_hpp

#include <stdio.h>

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace llvm;
using namespace std;

class SherlockInitDesign : public RecursiveASTVisitor<SherlockInitDesign>
{
public:
    bool VisitStmt(Stmt *S);
    void checkInitDesign(const ReturnStmt *RS);
    bool isDesignatedInitializer();
};

#endif /* SherlockInitDesign_hpp */
