/**
* The MIT License (MIT)
* Copyright (c) 2016 Peter Goldsborough and Markus Engel
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <cstddef>
#include <string>

#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"

namespace Memoize {

class MemoizeHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  explicit MemoizeHandler(clang::Rewriter& Rewriter) : Rewriter(Rewriter) {}

  void run(const MatchResult& Result) override {
    // Set internal state
    SourceManager = Result.SourceManager;
    LanguageOptions = &(Result.Context->getLangOpts());

    // The matched function.
    const auto& Function =
        *(Result.Nodes.getNodeAs<clang::FunctionDecl>("target"));

    assert(Function != nullptr);

    const auto NewName = renameOriginalFunction(Function);
    const auto Prototype = getFunctionPrototype(Function);

    auto NewDefinition = createMemoizedDefinition(Function, Prototype, NewName);
    auto AfterOriginalFunction = Function.getLocEnd().getLocWithOffset(1);
    Rewriter.InsertTextAfter(AfterOriginalFunction, NewDefinition);

    // Redeclare the function before its original definition
    // so that recursive calls can see the declaration.
    Rewriter.InsertTextBefore(Function.getLocStart(), Prototype + ";\n");
  }

private:
  using size_t = std::size_t;

  std::string createMemoizedDefinition(const clang::FunctionDecl& Function,
                                       const std::string& Prototype,
                                       const std::string& NewName) const {
    // Add our new, memoized definition for the function under its original name
    std::string MemoizedDefinition;
    MemoizedDefinition.reserve(75);

    MemoizedDefinition += "\n\n";
    MemoizedDefinition += Prototype;
    MemoizedDefinition += " {\n";
    MemoizedDefinition += "static const auto proxy = memoize(";
    MemoizedDefinition += NewName;
    MemoizedDefinition += ");\nreturn proxy(";
    MemoizedDefinition += getParameterNames(Function);
    MemoizedDefinition += ");\n";
    MemoizedDefinition += "}";

    return MemoizedDefinition;
  }

  std::string getParameterNames(const clang::FunctionDecl& Function) const {
    std::string ParameterNames;

    // Reserve an upper bound of space for the names in a
    // parameter list (usually shorter without types)
    ParameterNames.reserve(128);

    size_t index = 0;
    for (const auto* Parameter : Function.parameters()) {
      ParameterNames += Parameter->getNameAsString();
      if (++index < Function.getNumParams()) {
        ParameterNames += ", ";
      }
    }

    return ParameterNames;
  }

  std::string
  renameOriginalFunction(const clang::FunctionDecl& Function) const {
    const auto OriginalName = Function.getNameAsString();
    const auto NewName = OriginalName + "__original__";

    const auto BeforeParameters =
        Function.getLocation().getLocWithOffset(OriginalName.length() - 1);

    const auto DeclarationBegin = Function.getLocStart();

    const auto ReturnType = Function.getReturnType().getAsString();
    const auto NewDeclaration = ReturnType + " " + NewName;
    Rewriter.ReplaceText({DeclarationBegin, BeforeParameters}, NewDeclaration);

    return NewName;
  }

  // Converts a FunctionDecl object into a string representation of the
  // prototype of the function, optionally renaming it.
  std::string getFunctionPrototype(const clang::FunctionDecl& Function) const {
    std::string Prototype;
    Prototype.reserve(256);

    const auto Name = Function.getNameAsString();

    Prototype += Function.getReturnType().getAsString() + " ";
    Prototype += Name;
    Prototype += getParameterDeclaration(Function, Name);

    return Prototype;
  }

  std::string getParameterDeclaration(const clang::FunctionDecl& Function,
                                      const std::string& Name) const {
    const auto BeforeParametersLocation =
        Function.getLocation().getLocWithOffset(Name.length());
    const auto* BeforeParameters =
        SourceManager->getCharacterData(BeforeParametersLocation);

    const auto AfterParametersLocation =
        Function.getBody()->getLocStart().getLocWithOffset(-1);
    const auto* AfterParameters =
        SourceManager->getCharacterData(AfterParametersLocation);

    return std::string(BeforeParameters, AfterParameters);
  }

  clang::Rewriter& Rewriter;
  const clang::SourceManager* SourceManager;
  const clang::LangOptions* LanguageOptions;
};

}  // namespace Memoize