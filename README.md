## Sherlock

> Yep, This is "the detective", the one with a magnifying glass.

Sherlock是一个有高拓展性的代码规范检查基础工具，您的团队可以在Sherlock的基础上，轻松做定制、拓展。Sherlock是一个C++写的Clang Plugin，面向检查Objective-C代码。注意，使用Sherlock前必须要先将Clang Plugin整合到Xcode中，如何整合Clang Plugin和更多详细内容点[这里](http://blog.mrriddler.com/2017/02/24/Clang%E6%8F%92%E4%BB%B6-Sherlock/)。项目中的SherlockTest是测试代码，将Clang Plugin整合好后，可以创建新工程跑测试代码。

#### buildIn base rule

首先看看Sherlock提供的基本规则，首先是给Warning的:

- interface_illegal_character：项目文件命名中有"_"。
- project_prefix：项目中间接或直接继承自UIVIewController的controller命名没有以XX开头。
- property_atomic：属性声明为atomic。
- property_unsafe：属性声明为unsafe_unretained。
- property_copy：NSArray、NSNumber、NSString、block类型属性声明不为copy。
- weak_protocol：protocol类型的属性声明不为weak。
- category_method_naming：给系统类添加的Category方法命名没有以XX_开头。
- initializer_design：类没有用Designated Initializer设计init初始化方法。

然后是直接给Error的：

- mutable_property_copy：可变数据类型属性声明为copy。(copy会制作一个不可变数据深复制)
- weak_proxy：调用NSTimer或CADisplayLink方法，并将self作为参数传入。(容易引起循环引用，推荐使用WeakProxy)
- super_call：调用[super respondsToSelector:]或[super conformsToProtocol:]。(这样调用，等价于检查自己是否实现，没意义)
- observe_pairing：在一个编译单位中，添加NSNotificationCenter或KVO的观察者和移除观察者的操作数量不配对。(忘记移除观察者)

#### Configure

使用Sherlock前你需要在SherlockConfigure.hpp文件中修改ProjectAbsoluteRootPath为项目的根路径。然后，可以提供一个脚本的路径给RuleAbsolutePath字段，在脚本中进行Configure。现在支持的Configure有以下几项：

- disabled_rules：将规则名称以"-"开头，紧跟空格，如- initializer_design，可以关掉相应的规则。
- force_cast：可以键入Error或Warning，强制所有rule报告错误或是警告。
- project_prefix：键入比如Sherlock，提供项目的前缀名。
- blacklist_path：键入比如- Pods，忽略所有项目中Pods目录下的编译单元。
- blacklist_prefix：键入比如- AF，忽略所有Interface、Extension、implementation、Category以AF作为前缀的代码。

注意，所有以- 开头代表的多个选项必须以另起一行~end结束。样例Configure文件如下：

```shell
disabled_rules:
  - interface_illegal_character
  - project_prefix
  - property_atomic
  - property_unsafe 
  - property_copy
  - weak_protocol
  - category_method_naming
  - initializer_design
  - mutable_property_copy
  - weak_proxy
  - super_call
  - observe_pairing
  ~end

force_report: Warning

project_prefix: Sherlock

blacklist_path:
  - Pods
  - ThirdPart
  ~end

blacklist_prefix:
  - AF
  - SD
  - FM
  - YY
  ~end
```

还有一点需注意，如果你想忽略所有Pod下的第三方编译单元，只在blacklist_path中加入- Pods是行不通的。预处理的时候，会将引入编译单元合并，任意编译单元引入第三方，都会导致第三方编译单元被检查。如果想要完全忽略第三方编译单元要在blacklist_prefix加入前缀。



#### CustomRule

Sherlock提供了良好的拓展性，但是你还是需要一个clang的AST知识才能编写检查规则代码。添加一条新规则有如下几步：

- 为新的规则取一个名字，与原有规则不能重名，名字将作为规则的识别。然后加入到SherlockConfigure.hpp文件中的buildInBaseRules结构里。
- 在Sherlock.cpp创建一个新方法编写检查规则代码，在调用该方法的地方调用isBuildInBaseRuleEnable方法查看是否被关闭。

如果有更多的自定义Configure，从SherlockParser.hpp中的tupleRuleMap或setRuleMap就可以找到解析出来的字符串。

