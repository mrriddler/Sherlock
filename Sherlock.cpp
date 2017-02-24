//
//  Sherlock.cpp
//  Sherlock
//
//  Created by Robbie on 2017/1/29.
//
//

#include "Sherlock.hpp"
#include "clang/AST/ASTContext.h"
#include "SherlockConfigure.hpp"
#include "SherlockParser.hpp"

static bool isInBlackList;

namespace Sherlock
{
    void SherlockASTVisitor::setContext(ASTContext &context)
    {
        this->context = &context;
    }
            
    bool SherlockASTVisitor::VisitStmt(Stmt *S)
    {
        if (isInBlackList) {
            return true;
        }
        
        ObjCMessageExpr *ME = dyn_cast_or_null<ObjCMessageExpr>(S);
        if (ME) {
            if (this->isBuildInBaseRuleEnable(10)) {
                this->checkSuperMethod(ME);
            }
            
            if (this->isBuildInBaseRuleEnable(9)) {
                this->checkTimerAndDisplayer(ME);
            }
        }
        
        return true;
    }
        
    bool SherlockASTVisitor::VisitObjCInterfaceDecl(ObjCInterfaceDecl *ID)
    {
        if (this->checkIsSourceInBlackList(ID->getLocStart(), ID->getNameAsString())) {
            isInBlackList = true;
            return true;
        }
        
        isInBlackList = false;
        if (this->isBuildInBaseRuleEnable(0)) {
            this->checkInterfaceIllegalCharacters(ID);
        }
        
        if (this->isBuildInBaseRuleEnable(1)) {
            this->checkInterfacePrefix(ID);
        }
        return true;
    }
    
    bool SherlockASTVisitor::VisitObjCImplementationDecl(ObjCImplementationDecl *IMPD)
    {
        if (this->checkIsSourceInBlackList(IMPD->getLocStart(), IMPD->getNameAsString())) {
            isInBlackList = true;
            return true;
        }
        
        isInBlackList = false;
        if (this->isBuildInBaseRuleEnable(11)) {
            this->checkObservePairing(IMPD);
        }
        return true;
    }
    
    bool SherlockASTVisitor::VisitObjCCategoryDecl(ObjCCategoryDecl *CD)
    {
        if (CD->IsClassExtension()) {
            ObjCInterfaceDecl *ID = CD->getClassInterface();
            if (ID) {
                if (this->checkIsSourceInBlackList(CD->getLocStart(), ID->getNameAsString())) {
                    isInBlackList = true;
                    return true;
                }
                
                isInBlackList = false;
            }
        } else if (this->checkIsSourceInBlackList(CD->getLocStart(), CD->getNameAsString())) {
            return true;
        }
        
        if (this->isBuildInBaseRuleEnable(6)) {
            this->checkCategoryMethodNaming(CD);
        }
        return true;
    }
    
    bool SherlockASTVisitor::VisitObjCPropertyDecl(ObjCPropertyDecl *PD)
    {
        if (isInBlackList) {
            return true;
        }
        
        string typeStr = PD->getType().getAsString();
        ObjCPropertyDecl::PropertyAttributeKind attKin = PD->getPropertyAttributes();
        
        if (this->isBuildInBaseRuleEnable(4)) {
            this->checkCopyProperty(PD, typeStr, attKin);
        }
        
        if (this->isBuildInBaseRuleEnable(8)) {
            this->checkMutableProperty(PD, typeStr, attKin);
        }
        
        if (this->isBuildInBaseRuleEnable(5)) {
            this->checkDelegateProperty(PD, typeStr, attKin);
        }
        if (this->isBuildInBaseRuleEnable(2)) {
            this->checkAtomicProperty(PD, typeStr, attKin);
        }
        
        if (this->isBuildInBaseRuleEnable(3)) {
            this->checkUnsafeProperty(PD, typeStr, attKin);
        }
        
        return true;
    }
    
    bool SherlockASTVisitor::VisitObjCMethodDecl(ObjCMethodDecl *MD)
    {
        if (isInBlackList) {
            return true;
        }
        
        if (this->isBuildInBaseRuleEnable(7)) {
            this->checkInitDesign(MD);
        }
        
        return true;
    }
    
    void SherlockASTVisitor::checkInterfaceIllegalCharacters(ObjCInterfaceDecl *ID)
    {
        string name = ID->getNameAsString();
        
        if (name.find("_") != string::npos) {
            SourceLocation SL = ID->getLocation();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "Interface Naming should not contain _");
            diagEngine.Report(SL, diagId);
        }
    }
    
    void SherlockASTVisitor::checkInterfacePrefix(ObjCInterfaceDecl *ID)
    {
        if (!isSubclassOf("UIViewController", ID)) {
            return;
        }
        
        string name = ID->getNameAsString();
        string rule = buildInTupleRules[1];
        map<string, string>::iterator ite = tupleRuleMap.find(rule);
        if (ite != tupleRuleMap.end()) {
            string prefix = ite->second;
            
            if (name.compare(0, prefix.length(), prefix)) {
                SourceLocation SL = ID->getLocation();
                DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
                unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "Interface Naming should start with project prefix");
                diagEngine.Report(SL, diagId);
            }
        }
    }
    
    void SherlockASTVisitor::checkCopyProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin)
    {
        if (!(attKin & ObjCPropertyDecl::OBJC_PR_copy)
            && (typeStr.find("NSArray") != string::npos
                || typeStr.find("NSString") != string::npos
                || typeStr.find("NSNumber") != string::npos
                || PD->getType()->isBlockPointerType())) {
            SourceLocation location = PD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "Property should be declared as Copy instead");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkMutableProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin)
    {
        if (typeStr.find("NSMutable") == string::npos) {
            return;
        }
        
        if (attKin & ObjCPropertyDecl::OBJC_PR_copy) {
            SourceLocation location = PD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Error), "Mutable Object should not be declared as Copy");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkDelegateProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin)
    {
        if (typeStr.find("id<") != string::npos
            && typeStr.find(">") != string::npos
            && !(attKin & ObjCPropertyDecl::OBJC_PR_weak)) {
            SourceLocation location = PD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "Delegate should be declared as weak instead");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkAtomicProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin)
    {
        if (attKin & ObjCPropertyDecl::OBJC_PR_atomic) {
            SourceLocation location = PD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "pthread_mutex_t And dispatch_semaphore_t have a better performance, you should consider twice");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkUnsafeProperty(ObjCPropertyDecl *PD, string typeStr, ObjCPropertyDecl::PropertyAttributeKind attKin)
    {
        if (attKin & ObjCPropertyDecl::OBJC_PR_unsafe_unretained
            && !(attKin & ObjCPropertyDecl::OBJC_PR_assign)) {
            SourceLocation location = PD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "unsafe_unretained though have better performance than weak, may lead wild pointer, please consider twice");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkCategoryMethodNaming(ObjCCategoryDecl *CD)
    {
        if (CD->IsClassExtension()) {
            return;
        }
        
        string IDStr = CD->getClassInterface()->getNameAsString();
        if (IDStr.find("NS") == string::npos) {
            return;
        }
        
        for (ObjCContainerDecl::method_iterator ite = CD->meth_begin(); ite != CD->meth_end(); ++ite) {
            ObjCMethodDecl *MD = *ite;
            string selStr = MD->getSelector().getAsString();
            if (selStr.find("_") == string::npos) {
                SourceLocation location = MD->getSourceRange().getBegin();
                DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
                unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "Cocoa Foundation Category method naming should start with XX_");
                diagEngine.Report(location, diagId);
            }
        }
    }
    
    void SherlockASTVisitor::checkInitDesign(ObjCMethodDecl *MD)
    {
        SherlockInitDesign initDes;
        
        string selStr = MD->getSelector().getAsString();
        
        if (selStr.find("init") != string::npos
            && selStr.find("initWithCoder") == string::npos
            && selStr.find("initWithNibName") == string::npos) {
            if (MD->hasBody()) {
                initDes.TraverseStmt(MD->getBody());
                
                if (!initDes.isDesignatedInitializer()) {
                    SourceLocation location = MD->getSourceRange().getBegin();
                    DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
                    unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Warning), "multiple init method should provide Designated Initializer");
                    diagEngine.Report(location, diagId);
                }
            }
        }
    }
    
    void SherlockASTVisitor::checkObservePairing(ObjCImplementationDecl *IMPD)
    {
        SherlockObservePairing notPair;
        
        for (ObjCContainerDecl::method_iterator ite = IMPD->meth_begin(); ite != IMPD->meth_end(); ++ite) {
            ObjCMethodDecl *MD = *ite;
            if (MD->hasBody()) {
                notPair.TraverseStmt(MD->getBody());
            }
        }
        
        if (notPair.isNotificationMissPairing()) {
            SourceLocation location = IMPD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Error), "Add and Remove Observer for Notification are not pairing");
            diagEngine.Report(location, diagId);
        }
        
        if (notPair.isKVOMissPairing()) {
            SourceLocation location = IMPD->getSourceRange().getBegin();
            DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
            unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Error), "Add and Remove Observer for KVO are not pairing");
            diagEngine.Report(location, diagId);
        }
    }
    
    void SherlockASTVisitor::checkSuperMethod(ObjCMessageExpr *ME)
    {
        const ObjCMessageExpr::ReceiverKind recek = ME->getReceiverKind();
        if (recek == ObjCMessageExpr::SuperInstance) {
            
            ObjCMethodDecl *MD = ME->getMethodDecl();
            string selStr = MD->getNameAsString();
            if (selStr.find("respondsToSelector") != string::npos
                || selStr.find("conformsToProtocol") != string::npos) {
                SourceLocation location = ME->getSourceRange().getBegin();
                DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
                unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Error), "call super  respondsToSelector or conformsToProtocol method only lead to check current class");
                diagEngine.Report(location, diagId);
            }
        }
    }
    
    void SherlockASTVisitor::checkTimerAndDisplayer(ObjCMessageExpr *ME)
    {
        ObjCInterfaceDecl *IE = ME->getReceiverInterface();
        if (IE) {
            StringRef declName = IE->getName();
            if (declName.equals(StringRef("NSTimer"))
                || declName.equals(StringRef("CADisplayLink"))) {
                for (ObjCMessageExpr::arg_iterator ite = ME->arg_begin(); ite != ME->arg_end(); ++ite) {
                    Expr *E = *ite;
                    if (E->isObjCSelfExpr()) {
                        SourceLocation location = ME->getSourceRange().getBegin();
                        DiagnosticsEngine &diagEngine = this->context->getDiagnostics();
                        unsigned diagId = diagEngine.getCustomDiagID(this->decideReportType(DiagnosticsEngine::Error), "NSTimer or CADisplayLink method invoke with self as parameter lead to retain cycle, should use weakProxy instead");
                        diagEngine.Report(location, diagId);
                    }
                }
            }
        }
    }
    
    bool SherlockASTVisitor::checkIsSourceInBlackList(SourceLocation SL, string name)
    {
        if (this->checkIsInSystemHeader(SL)) {
            return true;
        }
        
        if (this->checkIsInBlackListPath(SL)) {
            return true;
        }
        
        if (this->checkIsNameInBlackList(SL, name)) {
            return true;
        }
        
        return false;
    }
    
    bool SherlockASTVisitor::checkIsInBlackListPath(SourceLocation SL)
    {
        const FileEntry *fileEntry = this->context->getSourceManager().getFileEntryForID(this->context->getSourceManager().getFileID(SL));
        
        StringRef filePath = fileEntry->tryGetRealPathName();
        
        if (filePath.startswith(ProjectAbsoluteRootPath)) {
            string filePathStr = filePath.str();
            
            string relativePath = filePathStr.erase(0, ProjectAbsoluteRootPath.length());
            
            string blackListPath = buildInSetRules[2];
            map<string, set<string>>::iterator ite = setRuleMap.find(blackListPath);
            set<string> blackSet;
            if (ite == setRuleMap.end()) {
                return false;
            } else {
                blackSet = ite->second;
            }

            for (set<string>::iterator ite = blackSet.begin(); ite != blackSet.end(); ++ite) {
                string path = *ite;
                if (relativePath.find(path) != string::npos) {
                    return true;
                }
            }
        }
        
        return false;
    }

    bool SherlockASTVisitor::checkIsNameInBlackList(SourceLocation SL, string name)
    {
        string blackListPrefix = buildInSetRules[1];
        map<string, set<string>>::iterator ite = setRuleMap.find(blackListPrefix);
        set<string> blackSet;
        if (ite == setRuleMap.end()) {
            return false;
        } else {
            blackSet = ite->second;
        }
        
        for (set<string>::iterator ite = blackSet.begin(); ite != blackSet.end(); ++ite) {
            string prefix = *ite;
            if (!name.compare(0, prefix.size(), prefix)) {
                return true;
            }
        }
        
        return false;
    }

    bool SherlockASTVisitor::checkIsInSystemHeader(SourceLocation SL)
    {
        SourceManager &SM = this->context->getSourceManager();
        FullSourceLoc fullLoc = this->context->getFullLoc(SL);
        if (SM.isInSystemHeader(fullLoc)) {
            return true;
        }

        return false;
    }
    
    DiagnosticsEngine::Level SherlockASTVisitor::decideReportType(DiagnosticsEngine::Level level)
    {
        DiagnosticsEngine::Level result = level;
        switch (level) {
            case DiagnosticsEngine::Warning:
            {
                 result = this->isForceError() ? DiagnosticsEngine::Error : DiagnosticsEngine::Warning;
            }
                break;
            case DiagnosticsEngine::Error:
            {
                result = this->isForceWarning() ? DiagnosticsEngine::Warning : DiagnosticsEngine::Error;
            }
            default:
                break;
        }
        
        return result;
    }
    
    bool SherlockASTVisitor::isBuildInBaseRuleEnable(size_t idx)
    {
        string disabledRule = buildInSetRules[0];
        map<string, set<string>>::iterator ite = setRuleMap.find(disabledRule);
        set<string> blackSet;
        if (ite == setRuleMap.end()) {
            return true;
        } else {
            blackSet = ite->second;
        }
        
        string baseRule = buildInBaseRules[idx];
        
        for (set<string>::iterator ite = blackSet.begin(); ite != blackSet.end(); ++ite) {
            string blackRule = *ite;
            if (!baseRule.compare(blackRule)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool SherlockASTVisitor::isForceError()
    {
        string rule = buildInTupleRules[0];
        map<string, string>::iterator ite = tupleRuleMap.find(rule);
        if (ite != tupleRuleMap.end()) {
            string forceCast = ite->second;
            if (forceCast.find("Error") != string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    bool SherlockASTVisitor::isForceWarning()
    {
        string rule = buildInTupleRules[0];
        map<string, string>::iterator ite = tupleRuleMap.find(rule);
        if (ite != tupleRuleMap.end()) {
            string forceCast = ite->second;
            if (forceCast.find("Warning") != string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    bool SherlockASTVisitor::isSubclassOf(const char *name, ObjCInterfaceDecl *IE)
    {
        ObjCInterfaceDecl *superIE = IE;
        while (superIE != nullptr) {
            if (superIE->getName().equals(name)) {
                return true;
            }
            
            superIE = superIE->getSuperClass();
        }
        return false;
    }
    
    void SherlockASTConsumer::HandleTranslationUnit(ASTContext &context)
    {
        this->visitor.setContext(context);
        this->visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
    
    unique_ptr<ASTConsumer> SherlockASTAction::CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        return unique_ptr<SherlockASTConsumer>(new SherlockASTConsumer);
    }
    
    bool SherlockASTAction::ParseArgs(const clang::CompilerInstance &CI, const std::vector<std::string> &args)
    {
        SherlockParser parser;
        tupleRuleMap = parser.parseTupleRule();
        setRuleMap = parser.parseSetRule();
        
        return true;
    }
}
