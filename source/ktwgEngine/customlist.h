#pragma once

/* Node that uses link list must have a next_ and a prev_ */
template <typename T>
class List
{
public:
  T* head_;
  size_t size_;

  List()
    : head_{ nullptr }, size_{ 0 }
  {
  }

  ~List() {}

  void PushFront(T* elem)
  {
    elem->prev_ = nullptr;
    elem->next_ = head_;

    if (head_)
      head_->prev_ = elem;

    head_ = elem;
    ++size_;
  }

  void Remove(T* elem)
  {
    if (elem->prev_)
      elem->prev_->next_ = elem->next_;

    if (elem->next_)
      elem->next_->prev_ = elem->prev_;

    if (elem == head_)
      head_ = elem->next_;

    --size_;
  }

  void Clear()
  {
    head_ = nullptr;
    size_ = 0;
  }
};