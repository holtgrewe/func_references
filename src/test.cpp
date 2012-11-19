#include "gtest/gtest.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Regex.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"

#include "clang/Tooling/Tooling.h"

namespace cl = llvm::cl;
using namespace clang;
using namespace clang::tooling;

/// \brief RAV wrapper to filter the traversal of the AST.
///
/// The supplied string is used as a regex to match the name of NamedDecls,
/// and only matching nodes are passed on to the wrapped visitor.
template<typename T>
class ASTFilter : public RecursiveASTVisitor<ASTFilter<T> > {
public:
  typedef RecursiveASTVisitor<ASTFilter> RAV;

  ASTFilter(T &Visitor, StringRef FilterString)
    : Visitor(Visitor), Filter(NULL) {
      if (!FilterString.empty()) {
        std::string Error;
        Filter = new llvm::Regex(FilterString);
        if (!Filter->isValid(Error))
          llvm::report_fatal_error("malformed filter expression: "
              + FilterString + ": " + Error);
      }
    }

  ~ASTFilter() {
    delete Filter;
  }

  bool TraverseDecl(Decl *D) {
    if (!Filter || Filter->match(getName(D)))
      return Visitor.TraverseDecl(D);
    return RAV::TraverseDecl(D);
  }

private:
  std::string getName(Decl *D) {
    if (isa<NamedDecl>(D))
      return cast<NamedDecl>(D)->getQualifiedNameAsString();
    return "";
  }

  T &Visitor;
  llvm::Regex *Filter;
};

class ASTPrinter : public ASTConsumer, public RecursiveASTVisitor<ASTPrinter>
{
public:
    typedef RecursiveASTVisitor<ASTPrinter> RAV;
    
    explicit ASTPrinter(raw_ostream &OS) : OS(OS), Filter(*this, "")
    {}

    virtual void HandleTranslationUnit(ASTContext &Context);
    bool VisitClassTemplateSpecializationDecl(ClassTemplateSpecializationDecl *D);
    bool VisitClassTemplatePartialSpecializationDecl(ClassTemplatePartialSpecializationDecl *D);
    
    ASTFilter<ASTPrinter> Filter;
    ASTContext *Context;
    raw_ostream &OS;
};

bool ASTPrinter::VisitClassTemplateSpecializationDecl(ClassTemplateSpecializationDecl *D)
{
    OS << "specialization\t" << D->getNameAsString() << "\n";
    return true;
}

bool ASTPrinter::VisitClassTemplatePartialSpecializationDecl(ClassTemplatePartialSpecializationDecl *D)
{
    OS << "partial specialization\t" << D->getNameAsString() << "\n";
    return true;
}

void ASTPrinter::HandleTranslationUnit(ASTContext &Context)
{
    this->Context = &Context;
    Filter.TraverseDecl(Context.getTranslationUnitDecl());
    this->Context = NULL;
}

class ASTPrinterAction : public ASTFrontendAction
{
public:
    ASTPrinterAction() {}
    
    virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef InFile)
    {
        return new ASTPrinter(llvm::outs());
    }
};

TEST(runToolOnCode, CanSyntaxCheckCode)
{
    // runToolOnCode returns whether the action was correctly run over the
    // given code.
    EXPECT_TRUE(clang::tooling::runToolOnCode(new ASTPrinterAction, "class X {}; template <typename T> class Y {}; template <> class Y<int> {};"));
}
