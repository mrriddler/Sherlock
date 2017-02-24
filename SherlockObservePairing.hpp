//
//  SherlockObservePairing.hpp
//  Sherlock
//
//  Created by Robbie on 2017/2/6.
//
//

#ifndef SherlockObservePairing_hpp
#define SherlockObservePairing_hpp

#include <string>
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace llvm;
using namespace std;

class SherlockObservePairing : public RecursiveASTVisitor<SherlockObservePairing>
{
public:
    bool VisitStmt(Stmt *S);
    void checkNotificationPairing(const ObjCMessageExpr *ME);
    void checkKVOPairing(const ObjCMessageExpr *ME);
    bool isNotificationMissPairing();
    bool isKVOMissPairing();
};


#endif /* SherlockObservePairing_hpp */
