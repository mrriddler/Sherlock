//
//  Sherlock.hpp
//  Sherlock
//
//  Created by Robbie on 2017/1/29.
//
//

#ifndef Sherlock_hpp
#define Sherlock_hpp

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/FrontEnd/FrontEndPluginRegistry.h"

#include "SherlockObservePairing.hpp"
#include "SherlockInitDesign.hpp"

using namespace clang;
using namespace llvm;
using namespace std;

namespace Sherlock
{    
    class SherlockASTVisitor : public RecursiveASTVisitor<SherlockASTVisitor>
    {
    public:
        void setContext(ASTContext &context);
        bool VisitStmt(Stmt *S);
        bool VisitObjCInterfaceDecl(ObjCInterfaceDecl *ID);
        bool VisitObjCImplementationDecl(ObjCImplementationDecl *IMPD);
        bool VisitObjCCategoryDecl(ObjCCategoryDecl *CD);
        bool VisitObjCPropertyDecl(ObjCPropertyDecl *PD);
        bool VisitObjCMethodDecl(ObjCMethodDecl *MD);
    private:
        ASTContext *context;
        
        void checkInterfacePrefix(ObjCInterfaceDecl *ID);
        void checkInterfaceIllegalCharacters(ObjCInterfaceDecl *ID);
        
        void checkCopyProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin);
        void checkMutableProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin);
        void checkDelegateProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin);
        void checkAtomicProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin);
        void checkUnsafeProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin);
                
        void checkTimerAndDisplayer(ObjCMessageExpr *ME);
        
        void checkSuperMethod(ObjCMessageExpr *ME);
        
        void checkObservePairing(ObjCImplementationDecl *IMPD);
        
        void checkInitDesign(ObjCMethodDecl *MD);
        
        void checkCategoryMethodNaming(ObjCCategoryDecl *CD);
        
        bool checkIsSourceInBlackList(SourceLocation SL, string name);
        bool checkIsInBlackListPath(SourceLocation SL);
        bool checkIsNameInBlackList(SourceLocation SL, string name);
        bool checkIsInSystemHeader(SourceLocation SL);
        bool isBuildInBaseRuleEnable(size_t idx);
        bool isForceError();
        bool isForceWarning();
        DiagnosticsEngine::Level decideReportType(DiagnosticsEngine::Level level);
        bool isSubclassOf(const char *name, ObjCInterfaceDecl *ID);
    };
    
    class SherlockASTConsumer : public ASTConsumer
    {
    private:
        SherlockASTVisitor visitor;
        void HandleTranslationUnit(ASTContext &context);
    };
    
    class SherlockASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,StringRef InFile);
        bool ParseArgs(const CompilerInstance &CI, const vector<string>& args);
    };
}

static clang::FrontendPluginRegistry::Add<Sherlock::SherlockASTAction> X("SherlockPlugin", "SherlockPlugin");

#endif /* Sherlock_hpp */
