//
//  SherlockObservePairing.cpp
//  Sherlock
//
//  Created by Robbie on 2017/2/6.
//
//

#include "SherlockObservePairing.hpp"

static int notRefCount;
static int KVORefCount;
static bool isIgnoreNot;

bool SherlockObservePairing::VisitStmt(Stmt *S)
{
    const ObjCMessageExpr *ME = dyn_cast_or_null<ObjCMessageExpr>(S);
    if (ME) {
        this->checkNotificationPairing(ME);
        this->checkKVOPairing(ME);
    }
    return true;
}

void SherlockObservePairing::checkNotificationPairing(const ObjCMessageExpr *ME)
{
    ObjCInterfaceDecl *ID = ME->getReceiverInterface();
    if (ID) {
        StringRef DName = ID->getName();
        if (DName.equals(StringRef("NSNotificationCenter"))) {
            
            string selStr = ME->getSelector().getAsString();
            if (selStr.find("addObserver") != string::npos) {
                ++notRefCount;
            }
            
            if (selStr.find("removeObserver") != string::npos
                && selStr.find("name") != string::npos) {
                --notRefCount;
            }
            
            if (selStr.find("removeObserver") != string::npos
                && selStr.find("name") == string::npos) {
                notRefCount = 0;
                isIgnoreNot = true;
            }
        }
    }
}

void SherlockObservePairing::checkKVOPairing(const ObjCMessageExpr *ME)
{
    string selStr = ME->getSelector().getAsString();
    
    if (selStr.find("addObserver") != string::npos
        && selStr.find("forKeyPath") != string::npos
        && selStr.find("options") != string::npos
        && selStr.find("context") != string::npos) {
        ++KVORefCount;
    }
    
    if (selStr.find("removeObserver") != string::npos
        && selStr.find("forKeyPath") != string::npos) {
        --KVORefCount;
    }
}

bool SherlockObservePairing::isKVOMissPairing()
{
    return KVORefCount == 0 ? false : true;
}

bool SherlockObservePairing::isNotificationMissPairing()
{
    return isIgnoreNot == true ? false : notRefCount == 0 ? false : true;
}
