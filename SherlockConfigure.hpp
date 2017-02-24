//
//  SherlockConfigure.hpp
//  Sherlock
//
//  Created by Robbie on 2017/2/15.
//
//

#ifndef SherlockConfigure_hpp
#define SherlockConfigure_hpp

#include <stdio.h>
#include <set>
#include <array>

static array<string, 2> buildInTupleRules = {{"force_report", "project_prefix"}};

static array<string, 3> buildInSetRules = {{"disabled_rules", "blacklist_prefix", "blacklist_path"}};

static array<string, 12> buildInBaseRules = {
    {"interface_illegal_character", "project_prefix", "property_atomic",
     "property_unsafe", "property_copy", "weak_protocol",
     "category_method_naming", "initializer_design", "mutable_property_copy",
     "weak_proxy", "super_call", "observe_pairing"}};

static string RuleAbsolutePath = "/.../Sherlock/theRule.txt";

static string ProjectAbsoluteRootPath = "/.../SherlockTest/";

#endif /* SherlockConfigure_hpp */
