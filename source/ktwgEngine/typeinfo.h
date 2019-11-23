#pragma once

#pragma once

//----------------------------------------------------------------------------//
#include <typeinfo>  // std::typeinfo

//------------------------ TypeInfo struct -----------------------------------//
struct TypeInfo
{
  TypeInfo(const std::type_info& rhs)
    : info_{ &rhs }, base_{ nullptr }
  {

  }

  TypeInfo(const std::type_info& t, const std::type_info& base)
    : info_{ &t }, base_{ &base }
  {

  }

  TypeInfo(const TypeInfo& rhs) = default;
  TypeInfo& operator=(const TypeInfo& rhs) = default;
  TypeInfo& operator=(TypeInfo&& rhs) = default;
  TypeInfo(TypeInfo&&) = default;

  const char * GetName() const { return info_->name(); }

  bool IfMyParents(const TypeInfo& typetocheck) const
  {
    if (base_)
      if (typetocheck == *base_)
        return true;
      else
        return TypeInfo{ *base_ }.IfMyParents(typetocheck);
    else
      return false;
  }

  friend bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return lhs.info_->before(*rhs.info_);
  }

  friend bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return rhs < lhs;
  }

  friend bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs > rhs);
  }

  friend bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs < rhs);
  }

  friend bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    return !(lhs == rhs);
  }

  friend bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
  {
    if (*lhs.info_ == *rhs.info_)
      return true;
    else if (lhs.IfMyParents(rhs))
      return true;
    else
      return rhs.IfMyParents(lhs);
  }

private:
  const std::type_info* info_ = nullptr;
  const std::type_info* base_ = nullptr;
};