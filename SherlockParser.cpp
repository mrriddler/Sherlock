//
//  SherlockParser.cpp
//  Sherlock
//
//  Created by Robbie on 2017/2/20.
//
//

#include "SherlockParser.hpp"

#include <fstream>
#include <string>

#include "SherlockConfigure.hpp"

using namespace std;

TupleRuleMap SherlockParser::parseTupleRule()
{
    TupleRuleMap result;
    ifstream file(RuleAbsolutePath);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("-") == string::npos
                && strcmp(&line.back(), ":")) {
                string first;
                string second;
                string::size_type pos = line.find(":");
                
                if (pos != string::npos) {
                    first = line.substr(0, pos);
                    second = line.substr(pos + 1 , line.length());
                    result.insert(pair<string, string>(first, second));
                }
            }
        }
        
        file.close();
    }
    
    return result;
}

SetRuleMap SherlockParser::parseSetRule()
{
    SetRuleMap result;
    ifstream file(RuleAbsolutePath);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            for (auto ite = begin(buildInSetRules); ite != end(buildInSetRules); ++ite) {
                string rule = *ite;
                if (line.find(rule) != string::npos) {
                    
                    set<string> set;
                    
                    while (getline(file, line)) {
                        string element;
                        string::size_type pos = line.find("-");
                        
                        if (pos != string::npos) {
                            element = line.substr(pos + 2, line.length());
                            set.insert(element);
                        } else if (line.find("~end") != string::npos) {
                            break;
                        }
                    }
                    
                    if (!set.empty()) {
                        result.insert(pair<string, std::set<string>>(rule, set));
                    }
                }
            }
        }
        
        file.close();
    }
    
    return result;
}

