#ifndef CC_MODEL_CPPRELATION_H
#define CC_MODEL_CPPRELATION_H

#include <memory>
#include <cstdint>
#include <string>

namespace cc
{
namespace model
{

#pragma db object
struct CppRelation
{
  enum class Kind
  {
    Override,
    Alias,
    Assign,
    DeclContext
  };

  #pragma db id auto
  int id;

  std::uint64_t lhs;
  std::uint64_t rhs;

  Kind kind;

  std::string toString() const
  {
    return std::string("id = ").append(std::to_string(id))
      .append("\nlhs = ").append(std::to_string(lhs))
      .append("\nrhs = ").append(std::to_string(rhs))
      .append("\nkind = ").append(
        kind == Kind::Override ? "Override" :
        kind == Kind::Alias ? "Alias" :
        kind == Kind::Assign ? "Assign" : "DeclContext");
  }

#pragma db index member(lhs)
#pragma db index member(rhs)
};

typedef std::shared_ptr<CppRelation> CppRelationPtr;

#pragma db view object(CppRelation)
struct CppRelationCount
{
  #pragma db column("count(" + CppRelation::id + ")")
  std::size_t count;
};

}
}

#endif
