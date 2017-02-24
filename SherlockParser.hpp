//
//  SherlockParser.hpp
//  Sherlock
//
//  Created by Robbie on 2017/2/20.
//
//

#ifndef SherlockParser_hpp
#define SherlockParser_hpp

#include <stdio.h>

#include <map>
#include <string>
#include <set>

using namespace std;

typedef map<string, string> TupleRuleMap;
static TupleRuleMap tupleRuleMap;

typedef map<string, set<string>> SetRuleMap;
static SetRuleMap setRuleMap;

class SherlockParser {
public:
    TupleRuleMap parseTupleRule();
    SetRuleMap parseSetRule();
};

#endif /* SherlockParser_hpp */
