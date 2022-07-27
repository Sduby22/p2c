# p2c
# 开发环境

## 编程语言

- 编程语言:使用C++17进行开发
- OS: 推荐使用Linux/MacOS
  - Windows用户请自行安装clang-format工具以及CMake
- 推荐使用VSCode、CLion等IDE
  - 安装拓展`CMake-Tools`

## 版本管理

本项目使用Git进行版本管理，使用git可以：
1. 完成版本库本地化,便于协同开发。每个开发者都拥有自己的版本控制库,在自己的版本库上可以任意的执行提交代码、创建分支等行为,回滚历史、反复提交、归并分支并不会影响到其他开发者,带来更少的代码污染
2. 分布式版本库,无单点故障,内容完整性好。内容存储使用的是SHA-1哈希算法。这能确保代码内容的完整性,确保在遇到磁盘故障和网络问题时降低对版本库的破坏
3. 方便代码管理,git可以很方便的创建、删除、提交、合并分支,同时配合git commit所提供的信息,更改的来源、内容、模块等一目了然


## 编译&运行

### 安装依赖

1. 安装以下依赖
    Flex 2.6及以上版本
    bison 3.8及以上版本
    cmake 3.0及以上版本

2. （可选）如果要编译WebAssembly和GUI 参考 :
    * [Compiling a New C/C++ Module to WebAssembly - WebAssembly | MDN](https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_wasm)
    * [Download and install — Emscripten 3.1.9-git (dev) documentation](https://emscripten.org/docs/getting_started/downloads.html)
    * 编译GUI需要安装node和yarn


### 如果使用IDE

请使用IDE中内置的CMake工具进行编译

### 使用命令行

```shell
$ cmake -B build -DBUILD_TESTING=on
$ cmake --build build -j8
$ ./build/main
```

### 运行程序

```shell
./main -h # 查看帮助
./main test.p -o test.c # 翻译pascal文件
./main -i - -o - # 若文件名为'-'，代表使用stdin/stdout进行输入/输
```

### 运行单元测试

```shell
$ cmake -B build -DBUILD_TESTING=ON # 启用单元测试编译
$ cmake --build build -j8
$ cd build
$ ctest # 或者使用make test命令
```

### GUI编译打包

首先按照[](https://emscripten.org/docs/getting_started/downloads.html)安装emsdk,然后执行以下命令

```shell
$ ./buildgui.sh
```

## 第三方库管理

第三方库统一放置在项目的`lib/`目录下，使用的第三方库如下:

* [gabime/spdlog: Fast C++ logging library](https://github.com/gabime/spdlog) - 用于统一管理日志
    * 将`spdlog`功能的子集重新包装为`logging`模块，从而隐藏复杂度便于使用
* [catchorg/Catch2: A modern, C++-native, test framework for unit-tests, TDD and BDD - using C++14, C++17 and later (C++11 support is in v2.x branch, and C++03 on the Catch1.x branch)](https://github.com/catchorg/Catch2) - C++单元测试框架
* [jarro2783/cxxopts: Lightweight C++ command line option parser](https://github.com/jarro2783/cxxopts) - C++命令行参数解析库
* [Neargye/magic_enum: Static reflection for enums (to string, from string, iteration) for modern C++, work with any enum type without any macro or boilerplate code](https://github.com/Neargye/magic_enum) - C++枚举类反射字符串库
    
## 翻译方案

### 程序结构分析

一个完整的Pascal-S程序的结构包括程序首部和程序体。
* 程序首部的结构为程序名与参数表，对应c语言的main函数及其参数表。
* 程序体依次为常量声明、变量声明、子程序声明、语句体。
    * 常、变量声明对应c语言的常、变量声明
    * 子程序声明包括过程与函数声明（数量为零或多个），其结构类似于Pascal-S程序的结构，但是不包括子程序声明，即不允许过程或函数的嵌套，此部分对应c语言中的函数声明与定义
    * 语句体即为Pascal-S的各种语句，包括表达式、函数或过程调用、判断语句、循环语句、I/O语句，分别对应c语言中的各类语句


### 语句类型分析

1. 常量类型
    Pascal-S中的常量没有类型，但常量的值有常数与字符，而c语言中常量有类型，所以在翻译的过程中应确定常量的值所对应的c语言的数据类型。
    Pascal-S的常量声明语句：
    ``` pascal
    const a = 'a'; b = 1; c = 1.0;
    ```
    对应的c语言语句：
    ``` c
    const char a = 'a'; const int b = 1; const float c = 1.0; 
    ```
2. 基本变量类型
    Pascal-S中的基本变量类型有：
    ``` pascal
    integer real boolean char
    ```
    对应的c语言变量类型为：
    ``` c
    int float int char
    ```
    Pascal-S的基本变量类型的声明语句：
    ``` pascal
    var a, b: integer; c: real; d: boolean; e: char;
    ```
    对应的c语言语句：
    ``` c
    int a, b;  float c;  int d; char e; 
    ```

3. 数组
    Pascal-S中的数组的声明：
    ``` pascal
    var a: array [period] of basic_type
    ```
    其中`period`的形式为`num_1..num_2, num_3..num_4 ···(num为正整数)`，`num_a..num_b`的个数表示数组的维数，每维的长度为`num_a-num_b+1`；`basic_type`为Pascal-S中的基本变量类型。
    Pascal-S中的数组的声明：
    ``` pascal
    var a: array [num_1..num_2, num_3..num_4] of basic_type;
    ```
    对应的c语言语句（`basic_type_c`为`basic_type`对应的c语言变量类型）：
    ``` 
    basic_type_c id[num_2 - num_1 + 1][num_4 - num_3 + 1];
    ```
    在Pascal-S中的数组的下标是从0开始的。
    数组元素的访问形式为（num为自然数）：
    ``` pascal
    a[num_1,num_2,···] := 1;
    ```
    对应的c语言形式：
    ``` c
    a[num_1][num_2][···] = 1;
    ```

4. 表达式
    Pascal-S的关系运算符：`= <> < <= > >=`，对应的c语言的关系运算符：`== != < <= > >=`；
    Pascal-S的运算符：`+ - * / or and div mod not`，对应的c语言的运算符：`+ - * / || && / % `，`div mod`只能用于整数运算；
    Pascal-S的赋值运算符：`:=`，对应的c语言的运算符：`=`。
    Pascal-S中的表达式语法、符号优先级与c语言中的完全相同，只需注意数组与运算符翻译即可。

5. 赋值语句
    Pascal-S中赋值语句：
    ``` pascal
    a:=b;
    ```
    如果b是常量名，则把b的值赋予a；
    如果b是变量，则把b的右值赋予a；
    如果b是函数，则把函数b的返回值赋予a。
    对应的c语言语句：
    ``` c
    a = b;
    ```

6. 代码块
    Pascal-S的形式:
    ``` pascal
    常量声明;
    变量声明;
    过程或函数声明;
    begin
        语句
    end
    ```
    对应的c语言的形式：
    ``` c
    {
        常量声明;
        变量声明;
        语句;
    }
    函数声明;
    ```

7. 函数与过程
    Pascal-S中，函数对应c语言的有返回值的函数，过程对应c语言的无返回值的函数。
    在函数与过程的声明语句中，若其参数表的形式为：
    `x, y: basic_type`
    即右值传递，则对应的c语言参数形式为：
    `basic_type_c x, basic_type_c y`
    若其参数表的形式为：
    `var x, y: basic_type`
    即左值传递，则对应的c语言参数形式为：
    `basic_type_c *x, basic_type_c *y`。
    Pascal-S的函数与过程声明语句:
    ``` pascal
        function func1(x :integer; var y :integer) :integer;
        const i = 1;
        var j: integer;
        begin         
            语句
        end;

        procedure func2(x :integer; var y :integer);
        const i = 1;
        var j: integer;
        begin
            语句
        end;  
    ```
    对应的c语言语句：
    ``` c
    int func1(int x, int *y)
    {
        const int i = 1;
        int j;
        语句
    }

    void func2(int x, int *y)
    {
        const int i = 1;
        int j;
        语句
    }
    ```
    Pascal-S的函数与过程调用语句:
    ``` pascal
    func0;
    a:=func1(x, y);
    func2(x, y)
    ```
    对应的c语言语句：
    ``` c
    func0();
    a = func1(x, &y);
    func2(x, &y);
    ```


8. I/O语句
    在把Pascal-S的I/O语句翻译成c语言的I/O语句前，应确定变量或表达式最终结果对应的c语言变量类型。
    Pascal-S的I/O语句:
    ``` pascal
    var a: integer; b: char;
    read(a, b);
    write(a, '-', b, 30)
    ```
    对应的c语言语句：
    ``` c
    int a;
    char b;
    scanf("%d %c", &a, &b);
    printf("%d%c%c%d", a, '-', b, 30);
    ```

9. 条件判断语句
    Pascal-S的条件判断语句：
    ``` pascal
    if (表达式) then
        语句
    else
        语句
    ```
    对应的c语言语句：
    ``` c
    if (表达式)
        语句;
    else
        语句;
    ```

10. `for`循环语句
    Pascal-S的`for`循环语句：
    ```pascal
    for a := 表达式_1 to 表达式_2 do
    begin
        语句
    end
    ```
    对应的c语言语句：
    ```c
    for (a = 表达式_1; a <= 表达式_2; ++a)
    {
        语句;
    }
    ```   

11. 其它说明
    在一个代码块内，语句之间用`;`间隔，最后一条语句后面不加`;`,以说明该语句为此代码块内最后一条语句；

## 代码生成

Pascal-S程序经过词法分析、语法分析、语义分析后，会被创建成一棵语法树，主程序通过调用根节点的C代码生成函数生成C语言代码，然后会递归地调用其子节点的C代码生成函数，直到到达叶子节点。
由于Pascal-S与C语言的程序结构并不相同，但其各自的组成部分在语法树上的层次是类似的，所以在生成C语言代码的过程中，数个子节点会将各自生成的C代码字符串返回给父节点，经由父节点的处理（如重新组织代码结构顺序、增加缩进和大括号、拼接等），生成在父节点层次下的正确的C语言代码，然后再返回给上层节点处理，直到到达根节点，生成完整并正确的C语言代码。

# 软件总体设计

## 软件功能

- 将Pascal-S程序翻译成C语言程序
- 将Pascal-S程序语法树以树型打印
- 显示词法、语法、语义分析的过程信息
- 显示词法、语法、语义分析的报错信息

## 软件结构

### 数据结构

#### 符号表设计




由于在后续的翻译过程中需要用到前面阶段识别出的标识符信息，故需要建立符号表，分别将词法分析阶段识别到的标识符和语法分析阶段识别到的类型与值存入符号表中，供后续使用。
根据需要，设计以下两种符号表类型：
1. 用于存储变量/常量信息的SymbolTable：
  - 每个函数对应一张SymbolTable，用于存放该函数内局部变量/常量的信息
  - 全局对应一张名为global的SymbolTable，用于存放其余变量/常量的信息
  
  ![](/codimd/uploads/upload_90006fcb455962efd2acdd2517f46156.jpg)
 ```cpp
  class SymbolTable {
    public:
      SymbolTable(const std::string &name);
      void add(std::string name, p2c::BasicType type, bool is_ref = false);
      void add(std::string name, p2c::ArrayType type, bool is_ref = false);
      bool contains(std::string name);
      Symbol& get(std::string name);
      void print();
      std::string name;

    private:
      std::unordered_map<std::string, Symbol> _symbols;
    };
```
- 成员变量name为对应函数的名字，如果name=="global"则代表为全局变量表
- add( )用来向SymbolTable中加入变量信息，其中is_ref用来说明是否为引用类型
- contains( )用来查找该表中是否含有所查变量
- get( )返回所查变量的信息

2. 用于存储函数信息的FunctionTable：
- Function用来存放全局所有函数的信息，包括对应的函数名，返回类型，参数信息等

![](/codimd/uploads/upload_06e424b75205a6ea9b20e7235ca9bd8e.jpg)

```cpp
class FunctionTable {
public:
  FunctionTable() = default;
  void add(std::string name, p2c::BasicType return_type, std::vector<Symbol> params);
  bool contains(std::string name);
  Function& get(std::string name);
  void print();
  void clear();

private:
  std::unordered_map<std::string, Function> _functions;
};
```
- add( )用来向FunctionTable中加入函数信息，其中参数name为函数名，return_type为返回类型，params为参数信息
- contains( )用来查找函数是否存在
- get( )返回所查函数的信息

除此之外，还设计以下全局可用的方法来检索各个符号表：
```cpp
Function& find_function(std::string name);
Symbol& find_symbol(std::string name);
```
- find_function( )用来检索函数是否存在，并返回函数信息
- find_symbol( )会在当前scope内检索变量的信息

#### 语法树设计

Pascal-S程序在语法分析和语义分析的过程中，需要创建一棵语法树，其节点需求如下：
- 在将Pascal-S程序翻译成C语言代码时，需要使用语法树节点的C代码生成函数
- 为生成C语言代码，需要记录该节点的相应信息，
- 为构建语法树结构，需要记录节点的父节点和子节点
- 为方便程序运行与调试，显示节点信息，需要以树型结构打印节点
- 为了打印节点，需要获取节点名称与信息

对于Pascal-S程序中的不同组成部分，可能对应于语法树中不同类型的节点，所以将语法树的节点设计成：
1. 具有所有节点共同属性与方法的基类
``` cpp
class ASTNode {
    public:
    virtual ~ASTNode() = default;

    virtual std::string printNode();
    virtual std::string genCCode() = 0;
    virtual std::string _genCCode(int level);
    virtual void _printNode(int level, std::string &str);
    void appendChild(std::unique_ptr<ASTNode> child);

    std::vector<std::unique_ptr<ASTNode>> _childs;
    ASTNode* _parent;

    private:
    virtual const std::string &_getName() = 0;
    virtual std::string _infoStr() = 0;
};
```
- 属性`_childs`为此节点的所有子节点，属性`_parent`为此节点的父节点
- 方法`genCCode()`与`_genCCode()`用于生成此节点的C语言代码
- 方法`printNode`与`_printNode`用于以树型结构打印此节点
- 方法`_getName()`与`_infoStr()`分别用于获取此节点的名称与信息
- 方法`appendChild()`用于构建语法树，为此节点添加子节点

2. 对应于不同类型的节点，继承于基类的派生类
- 各派生类可重载基类的方法，用于实现其在功能上相同，但在逻辑细节上不同的方法
- 各派生类可在基类的基础上，增加各自特有属性与方法，如变量声明节点需要增加的变量名和对应的变量类型的属性:
    ``` cpp
    vector<string>idlist;
    variant<BasicType, ArrayType>type;
    ```
    继承关系如下图：
    ![](https://kreedz.jnn.icu/doc/inherit_graph_15.png)

### LR分析技术

LALR其实是LR(1)方法的子集，全称为LookAhead-LR。
所谓LR(1)分析,是指从左至右扫描和自底向上的语法分析,且在分析的每一步,只须根据分析栈当前已移进和归约出的全部文法符号,并至多再向前查看1个输入符号,就能确定相对于某一产生式左部符号的句柄是否已在分析栈的顶部形成,从而也就可以确定当前所应采取的分析动作 (是移进还是按某一产生式进行归约等).
而LALR是在LR(1)的基础上寻找具有相同核心的LR(1)项集,并将这些项集合并为一个项集,从而减少自动机状态数.然后根据合并后得到的项集族构造语法分析表,如果分析表中没有语法分析动作冲突,给定的文法就称为LALR(1)文法,就可以根据该分析表进行语法分析.

工作流程:
![](/codimd/uploads/upload_05e70325619bb9a95823ae575f7648e0.jpg)


### 模块间接口

程序主要分为`main`、 `driver`、 `scanner`、 `parser`、 `astnode`五个模块
- 模块调用关系为:
![](/codimd/uploads/upload_72e399661edeae50d573b3e14aabab6c.png)
`main`模块调用`driver`模块，实现Pascal-S程序翻译成C语言代码的整个过程；
`driver`模块首先调用 `scanner`实现词法分析，然后调用 ``parser``实现语法分析、语义分析；
`driver`模块也可以调用`astnode`模块C代码生成和打印语法树；
`parser`模块调用`astnode`模块实现构建语法树。

- 模块间接口为:
![](/codimd/uploads/upload_4d068ca576798af7f64528427229f96d.png)


## 用户接口设计

### 命令行接口设计

1. help介绍:

```
$ ./main -h
A Pascal-S to C translator
Usage:
  p2c [OPTION...] positional parameters

  -d, --debug       Enable debugging
  -o, --output arg  Output File (default: a.out.c)
  -v, --verbose     Verbose output
  -h, --help        Print this help message
```

2. 指令执行方法:
    - `-i`:输入文件可以使用`-i`指定,否则默认第一个出现的非指定OPTION的命令行参数为输入文件名
         实例:`./main -i 1.txt` `./main 1.txt`
    - `-o`:输出文件可以使用`-o`指定,否则默认输出文件为`a.out.c`
         实例:`./main -i 1.txt -o 2.txt` `./main 1.txt`
    - `-d`:将以debug模式输出
         实例:`./main -i 1.txt -o 2.txt -d` `./main -d 1.txt`
    - `-v`:将在命令行输出运行信息
         实例:`./main -i 1.txt -o 2.txt -v` `./main -v 1.txt`
    

### 图形客户端设计

图形客户端使用Vite+Electron构建，Vue3+Element-Plus搭建页面。

#### 图形客户端需求

- 提供Pascal-S代码的编辑功能，包括代码高亮与格式化辅助；
- 提供Pascal-S代码向C语言代码的翻译功能；
- 提供可选的实时Pascal-S代码向C语言代码的翻译功能；
- 提供实时语法检查与错误信息展示的功能；
- 提供加载Pascal-S源文件的功能；
- 提供保存C源文件（翻译结果）的功能。

#### 布局预览与设计
输出C代码
![](/codimd/uploads/upload_18f1e266a024fa3bb4f56c6580f787fc.png)
输出语法树
![](/codimd/uploads/upload_93ae918955c9370add87b2ae6c44b0e0.png)

#### 功能设计

P2C构建出WASM库后，由图形客户端使用。P2C向图形客户端侧暴露的接口如下：

``` javascript=
export class ParseResult {
    success: boolean;
    c_code: string;
    syntax_tree: string;
    message: string;
}

export interface ParserMoudle {
    parse(pascalCode: string): ParseResult;
}

export default function CreateParserModule(mod?: any): Promise<ParserMoudle>;
```
1. ParseResult类：保存翻译结果，包含的信息有：
    - 是否成功；
    - C代码；
    - 语法树打印文本；
    - 输出信息。
2. parse接口：输入Pascal-S代码文本，输出翻译结果。

使用Vue3建立UI数据与变量的绑定，实现较为简单，仅展示重要的业务逻辑接口的设计。

1. compile
当用户点击Convert按钮时调用。计算Pascal-S代码MD5哈希，与之前保存的哈希比对，当哈希值变化时调用translate接口，将结果通过Toast反馈给用户，将日志更新在Output处，成功时更新C代码输出；
2. update
定时刷新时调用。当用户勾选自动翻译时，计算Pascal-S代码MD5哈希，与之前保存的哈希比对，当哈希值变化时调用translate接口获取翻译结果，将日志更新在Output处，若成功则更新C代码输出。
3. translate
封装WASM接口，返回翻译结果。
4. openFile
调用Electron提供的IO接口。
5. saveFile
调用Electron提供的IO接口。

使用AceEditor作为文本编辑器，提供语法高亮，行数显示，代码折叠等功能。翻译输出窗口提供切换语法树与C代码等功能，点击右上方按钮即可切换。

# 程序清单

```
include
└──logging.h    # 输出程序运行的调试信息
src	
├──libparser.cpp        	
├──main.cpp    # 主程序，命令行解析
└──parser
    ├──ASTNode.cpp    # 语法树节点的实现
    ├──ASTNode.h      # 语法树节点的定义
    ├──CMakeLists.txt # 项目配置文件
    ├──driver.cpp     # driver类的实现
    ├──driver.h       # driver类的定义
    ├──FlexLexer.h    # 词法分析的定义
    ├──lexer.l        # 词法分析的实现
    ├──magic_enum.hpp
    ├──parser.y       # 语法分析、语义分析、语法树构建
    ├──scanner.h      # Scanner类的定义
    ├──symtable.cpp   # 符号表的实现
    ├──symtable.h     # 符号表的定义
    └──types.h        # 运算符和数据类型的定义
```

# 测试报告

## 白盒单元测试


| 测试模块 | 测试内容             | 测试结果 |
| -------- | -------------------- | -------- |
| Lexer    | char类型变量         | Pass✅   |
|          | bool类型变量         | Pass✅   |
|          | real和int类型变量    | Pass✅   |
|          | 标识符               | Pass✅   |
|          | 保留字               | Pass✅   |
|          | 比较运算符           | Pass✅   |
|          | 基本运算符           | Pass✅   |
|          | 逻辑运算符           | Pass✅   |
|          | 标点符号             | Pass✅   |
|          | 注释                 | Pass✅   |
| Parser   | 注释                 | Pass✅   |
|          | 变量的定义           | Pass✅   |
|          | 程序体               | Pass✅   |
|          | 过程                 | Pass✅   |
|          | 函数                 | Pass✅   |
|          | 变量                 | Pass✅   |
|          | 参数表               | Pass✅   |
|          | 条件控制语句         | Pass✅   |
|          | for循环语句          | Pass✅   |
|          | io控制语句           | Pass✅   |
|          | 数组类型的定义和使用 | Pass✅   |


白盒单元测试编写时统一使用单元测试框架Catch2进行各模块的单元白盒测试


[Catch2/tutorial.md at devel · catchorg/Catch2](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md#top) - Catch2 官方文档

单元测试程序的结构如下:

```
tests
├── test_example.cpp
├── test_lexer.cpp
├── test_yacc.cpp
├── test_main.cpp
└── parser_cases
```

# 实验总结

通过该课程设计，我们从实践中更深一步掌握所学到的编译原理相关知识。并手动编写实现所涉及到的数据结构与算法，最终结合所学到的其他软件工程知识开发出用户友好的程序产品。在实现的过程中，我们不仅接触了Flex, Bison等较为成熟的技术，也接触了像WebAssembly等较为新兴的技术，大大提高了我们的开发效率。
