#pragma once

#include <initializer_list>
#include <iterator>
#include <stdexcept>

using std::initializer_list;

template <typename T>
class Vector {
public:
  using ValueType = T;
  using ConstValueType = const ValueType;
  using SizeType = size_t;
  using ConstSizeType = const SizeType;
  using Reference = ValueType&;
  using ConstReference = const ValueType&;
  using Pointer = ValueType*;
  using ConstPointer = const ValueType*;
  using Iterator = Pointer;
  using ConstIterator = ConstPointer;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

private:
  SizeType size_ = 0;
  SizeType capacity_ = 0;
  Pointer buf_ = nullptr;
  void Destroy(Pointer buffer, SizeType s) {
    for (SizeType i = 0; i < s; i++) {
      (buffer + i)->~ValueType();
    }
    operator delete(buffer);
  }

public:
  Vector() : size_(0), capacity_(0), buf_(nullptr) {
  }

  explicit Vector(SizeType s) : size_(s), capacity_(size_), buf_(nullptr) {
    if (capacity_ != 0) {
      SizeType i = 0;
      try {
        buf_ = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
        for (i; i < size_; i++) {
          new (buf_ + i) ValueType();
        }
      }
      catch (...) {
        Destroy(buf_, i);
        throw;
      }
    }
  }

  Vector(SizeType s, ConstValueType value) : size_(s), capacity_(size_), buf_(nullptr) {
    if (capacity_ != 0) {
      SizeType i = 0;
      try {
        buf_ = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
        for (i; i < size_; i++) {
          new (buf_ + i) ValueType(value);
        }
      }
      catch (...) {
        Destroy(buf_, i);
        throw;
      }
    }
  }

  Vector(Vector&& v) noexcept : size_(v.size_), capacity_(v.capacity_), buf_(v.buf_) {
    v.buf_ = nullptr;
    v.size_ = 0;
    v.capacity_ = 0;
  }

  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
    std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  Vector(Iterator first, Iterator last) : size_(last - first), capacity_(size_), buf_(nullptr) {
    if (capacity_ != 0) {
      SizeType i = 0;
      try {
        buf_ = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
        for (i; i < size_; i++) {
          new (buf_ + i) ValueType();
        }
        for (SizeType j = 0; j < size_; j++) {
          buf_[j] = first[j];
        }
      }
      catch (...) {
        Destroy(buf_, i);
        throw;
      }
    }
  }

  Vector(const Vector& v) : size_(v.size_), capacity_(v.capacity_), buf_(nullptr) {
    if (capacity_ != 0) {
      SizeType i = 0;
      try {
        buf_ = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
        for (i; i < size_; i++) {
          new (buf_ + i) ValueType();
        }
      }
      catch (...) {
        Destroy(buf_, i);
        // operator delete(buf_);
        throw;
      }
      try {
        for (SizeType i = 0; i < size_; i++) {
          buf_[i] = v.buf_[i];
        }
      }
      catch (...) {
        Destroy(buf_, size_);
      }
    }
  }

  Vector(const initializer_list<ValueType>& l) : size_(l.size()), capacity_(size_), buf_(nullptr) {
    if (capacity_ != 0) {
      SizeType i = 0;
      try {
        buf_ = static_cast<Pointer>(operator new(sizeof(ValueType) * capacity_));
        for (i; i < size_; i++) {
          new (buf_ + i) ValueType();
        }
      }
      catch (...) {
        Destroy(buf_, i);
        // operator delete(buf_);
        throw;
      }
      try {
        SizeType j = 0;
        for (auto i : l) {
          buf_[j++] = i;
        }
      }
      catch (...) {
        Destroy(buf_, size_);
        throw;
      }
    }
  }

  SizeType Size() const {
    return size_;
  }

  SizeType Capacity() const {
    return capacity_;
  }

  bool Empty() const {
    return (size_ == 0);
  }

  Reference operator[](ConstSizeType i) {
    if (i >= 0 and i < size_) {
      return buf_[i];
    }
    throw std::out_of_range("std::out_of_range");
  }

  ConstReference operator[](ConstSizeType i) const {
    if (i >= 0 and i < size_) {
      return buf_[i];
    }
    throw std::out_of_range("std::out_of_range");
  }

  Reference At(ConstSizeType i) {
    if (i >= 0 and i < size_) {
      return buf_[i];
    }
    throw std::out_of_range("std::out_of_range");
    ;
  }

  ConstReference At(ConstSizeType i) const {
    if (i >= 0 and i < size_) {
      return buf_[i];
    }
    throw std::out_of_range("std::out_of_range");
  }

  Reference Front() {
    try {
      return buf_[0];
    }
    catch (...) {
      throw;
    }
  }

  ConstReference Front() const {
    try {
      return buf_[0];
    }
    catch (...) {
      throw;
    }
  }

  Reference Back() {
    try {
      return buf_[size_ - 1];
    }
    catch (...) {
      throw;
    }
  }

  ConstReference Back() const {
    try {
      return buf_[size_ - 1];
    }
    catch (...) {
      throw;
    }
  }

  Pointer Data() {
    return buf_;
  }

  ConstPointer Data() const {
    return buf_;
  }

  void Swap(Vector& v) {
    Pointer buf_copy = std::move(v.buf_);
    v.buf_ = std::move(buf_);
    buf_ = std::move(buf_copy);
    SizeType size_copy = std::move(v.size_);
    v.size_ = std::move(size_);
    size_ = std::move(size_copy);
    SizeType capacity_copy = std::move(v.capacity_);
    v.capacity_ = std::move(capacity_);
    capacity_ = std::move(capacity_copy);
  }

  void Resize(ConstSizeType new_size) {
    auto tmp = size_;
    SizeType i = size_;
    if (new_size <= size_) {
      for (SizeType j = new_size; j < size_; j++) {
        (buf_ + j)->~ValueType();
      }
      size_ = new_size; // |--------|--------|--------------|
      return;
    }
    try {
      if (new_size <= capacity_) {
        for (i; i < new_size; i++) {
          buf_[i] = ValueType();
        }
        size_ = new_size;
        return;
      }
    }
    catch (...) {
      Destroy(buf_ + size_; i - size_);
      size_ = tmp;
      throw;
    }
    Pointer new_buf = nullptr;
    SizeType ii = 0;
    try {
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * new_size));
      for (ii; ii < new_size; ii++) {
        new (new_buf + ii) ValueType();
      }
    }
    catch (...) {
      Destroy(new_buf, ii);
      // operator delete(new_buf);
      throw;
    }

    capacity_ = new_size;
    for (SizeType i = 0; i < size_; i++) {
      new_buf[i] = std::move(buf_[i]);
    }
    size_ = new_size;
    Destroy(buf_, size_);
    buf_ = new_buf;
  }

  void Resize(ConstSizeType new_size, ConstValueType value) {
    auto tmp = size_;
    SizeType i = size_;
    if (new_size <= size_) {
      for (SizeType j = 0; j < size_; j++) {
        (buf_ + j)->~ValueType();
      }
      size_ = new_size;
      return;
    }
    try {
      if (new_size <= capacity_) {
        for (i; i < new_size; i++) {
          buf_[i] = ValueType();
        }
        for (SizeType j = size_; j < new_size; j++) {
          buf_[j] = value;
        }
        size_ = new_size;
        return;
      }
    }
    catch (...) {
      Destroy(buf_ + size_, i - size_);
      size_ = tmp;
      throw;
    }
    Pointer new_buf = nullptr;
    auto tmp1 = capacity_;
    SizeType ii = 0;
    try {
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * new_size));
      for (ii; ii < new_size; ii++) {
        new (new_buf + ii) ValueType();
      }
    }
    catch (...) {
      Destroy(new_buf, ii);
      // operator delete(new_buf);
      throw;
    }
    capacity_ = new_size;
    for (SizeType i = 0; i < size_; i++) {
      new_buf[i] = std::move(buf_[i]);
    }
    try {
      for (SizeType i = size_; i < capacity_; i++) {
        new_buf[i] = value;
      }
    }
    catch (...) {
      for (SizeType i = 0; i < size_; i++) {
        buf_[i] = std::move(new_buf[i]);
      }
      Destroy(new_buf + size_, new_size - size_);
      capacity_ = tmp1;
      throw;
    }

    size_ = new_size;
    Destroy(buf_, size_);
    buf_ = new_buf;
  }
  void Reserve(ConstSizeType new_cap) {
    if (new_cap <= capacity_) {
      return;
    }
    Pointer new_buf = nullptr;
    auto tmp = capacity_;
    SizeType i = 0;
    try {
      new_buf = static_cast<Pointer>(operator new(sizeof(ValueType) * new_cap));
      for (i; i < size_; i++) {
        new (new_buf + i) ValueType();
      }
    }
    catch (...) {
      Destroy(new_buf, i);
      // operator delete(new_buf);
      throw;
    }
    //try {
    capacity_ = new_cap;
    for (SizeType j = 0; j < size_; j++) {
      new_buf[j] = std::move(buf_[j]);
    }
    /*catch (...) {
      for (SizeType j = 0; j < ii; j++) {
        buf_[j] = std::move(new_buf[j]);
      }
      capacity_ = tmp;
      // Destroy(new_buf, new_cap);
      Destroy(new_buf, ii);
      throw;
    }*/
    Destroy(buf_, size_);
    buf_ = new_buf;
  }

  void ShrinkToFit() {
    if (size_ == capacity_) {
      return;
    }
    if (size_ == 0) {
      Destroy(buf_, size_);
      buf_ = nullptr;
      capacity_ = 0;
      return;
    }
    capacity_ = size_;
    /*Pointer new_buf = nullptr;
    auto tmp = capacity_;
    try {
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * capacity_));
      for (SizeType i = 0; i < capacity_; i++) { new (new_buf + i)
    ValueType();
      }
    } catch (...) {
      //Destroy(new_buf, capacity_);
      operator delete(new_buf);
      throw;
    }
    capacity_ = size_;
    for (SizeType i = 0; i < size_; i++) {
      new_buf[i] = std::move(buf_[i]);
    }
    Destroy(buf_, size_);
    buf_ = new_buf;*/
  }

  void Clear() {
    Destroy(buf_, size_);
    buf_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }

  void PushBack(ConstReference a) {
    try {
      if (capacity_ == 0) {
        capacity_ = 1;
        buf_ = static_cast<Pointer>(operator new(sizeof(ConstValueType)));
        new (buf_) ValueType();
        buf_[size_++] = a;
        return;
      }
    }
    catch (...) {
      Destroy(buf_, size_);
      buf_ = nullptr;
      capacity_ = size_ = 0;
      throw;
    }
    try {
      if (size_ != capacity_) {
        buf_[size_++] = a;
        return;
      }
    }
    catch (...) {
      size_--;
      throw;
    }
    Pointer new_buf = nullptr;
    auto tmp = capacity_;
    SizeType i = 0;
    try {
      capacity_ *= 2;
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * capacity_));
      for (i; i <= size_; i++) {
        new (new_buf + i) ValueType();
      }
    }
    catch (...) {
      capacity_ = tmp;
      Destroy(new_buf, i);
      // operator delete(new_buf);
      throw;
    }
    //try {
    for (SizeType j = 0; j < size_; j++) {
      new_buf[j] = std::move(buf_[j]);
    }
    try {
      new_buf[size_++] = a;
    }
    catch (...) {
      --size_;
      capacity_ = tmp;
      for (SizeType j = 0; j < size_; j++) {
        buf_[j] = std::move(new_buf[j]);
      }
      Destroy(new_buf, size_ + 1);
      throw;
    }
    Destroy(buf_, size_);
    buf_ = new_buf;
  }

  void PushBack(ValueType&& a) {
    try {
      if (capacity_ == 0) {
        capacity_ = 1;
        buf_ = static_cast<Pointer>(operator new(sizeof(ConstValueType)));
        new (buf_) ValueType();
        buf_[size_++] = std::move(a);
        return;
      }
    }
    catch (...) {
      Destroy(buf_, size_);
      buf_ = nullptr;
      capacity_ = size_ = 0;
      throw;
    }
    try {
      if (size_ != capacity_) {
        buf_[size_++] = a;
        return;
      }
    }
    catch (...) {
      size_--;
      throw;
    }
    Pointer new_buf = nullptr;
    auto tmp = capacity_;
    SizeType i = 0;
    try {
      capacity_ *= 2;
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * capacity_));
      for (i; i <= size_; i++) {
        new (new_buf + i) ValueType();
      }
    }
    catch (...) {
      capacity_ = tmp;
      Destroy(new_buf, i);
      // operator delete(new_buf);
      throw;
    }
    //try {
    for (SizeType j = 0; j < size_; j++) {
      new_buf[j] = std::move(buf_[j]);
    }
    try {
      new_buf[size_++] = std::move(a);
    }
    catch (...) {
      --size_;
      capacity_ = tmp;
      for (SizeType j = 0; j < size_; j++) {
        buf_[j] = std::move(new_buf[j]);
      }
      Destroy(new_buf, size_ + 1);
      throw;
    }
    Destroy(buf_, size_);
    buf_ = new_buf;
  }

  void PopBack() {
    if (size_ == 0) {
      return;
    }
    (buf_ + size_ - 1)->~ValueType();
    size_--;
  }

  bool operator<(const Vector& v) const {
    size_t i = 0;
    while (i < size_ and i < v.size_ and buf_[i] == v.buf_[i]) {
      i++;
    }
    if (i == size_ and i == v.size_) {
      return false;
    }
    if (i == size_) {
      return true;
    }
    if (i == v.size_) {
      return false;
    }
    return buf_[i] < v.buf_[i];
  }
  Vector& operator=(const Vector& v) {
    if (this == &v) {
      return *this;
    }
    if (v.capacity_ == 0) {
      Destroy(buf_, size_);
      size_ = capacity_ = 0;
      buf_ = nullptr;
      return *this;
    }

    Pointer new_buf = nullptr;
    SizeType i = 0;
    try {
      new_buf = static_cast<Pointer>(operator new(sizeof(ConstValueType) * v.capacity_));
      for (i; i < v.size_; i++) {
        new (new_buf + i) ValueType();
      }
    }
    catch (...) {
      Destroy(new_buf, i);
      // operator delete(new_buf);
      throw;
    }
    try {
      for (SizeType j = 0; j < v.size_; ++j) {
        new_buf[j] = v.buf_[j];
      }
    }
    catch (...) {
      Destroy(new_buf, v.size_);
      throw;
    }
    Destroy(buf_, size_);
    buf_ = new_buf;
    capacity_ = v.capacity_;
    size_ = v.size_;
    return *this;
  }
  Vector& operator=(Vector&& v) noexcept {
    if (this != &v) {
      Destroy(buf_, size_);
      buf_ = v.buf_;
      size_ = v.size_;
      capacity_ = v.capacity_;
      v.buf_ = nullptr;
      v.size_ = 0;
      v.capacity_ = 0;
    }
    return *this;
  }
  bool operator>(const Vector& v) const {
    return v < *this;
  }
  bool operator<=(const Vector& v) const {
    return !(*this > v);
  }
  bool operator>=(const Vector& v) const {
    return !(*this < v);
  }
  bool operator==(const Vector& v) const {
    if (size_ == v.size_) {
      for (SizeType i = 0; i < size_; i++) {
        if (buf_[i] != v.buf_[i]) {
          return false;
        }
      }
      return true;
    }
    return false;
  }
  bool operator!=(const Vector& v) const {
    return !(v == *this);
  }

  Iterator begin() {  // NOLINT
    return Iterator(buf_);
  }

  Iterator end() {  // NOLINT
    return Iterator(buf_ + size_);
  }

  ConstIterator begin() const {  // NOLINT
    return ConstIterator(buf_);
  }

  ConstIterator end() const {  // NOLINT
    return ConstIterator(buf_ + size_);
  }

  ConstIterator cbegin() const {  // NOLINT
    return begin();
  }

  ConstIterator cend() const {  // NOLINT
    return end();
  }

  ReverseIterator rbegin() {  // NOLINT
    return ReverseIterator(buf_ + size_);
  }

  ReverseIterator rend() {  // NOLINT
    return ReverseIterator(buf_);
  }

  ConstReverseIterator rbegin() const {  // NOLINT
    return ConstReverseIterator(buf_ + size_);
  }

  ConstReverseIterator rend() const {  // NOLINT
    return ConstReverseIterator(buf_);
  }
  ConstReverseIterator crbegin() const {  // NOLINT
    return ConstReverseIterator(buf_ + size_);
  }

  ConstReverseIterator crend() const {  // NOLINT
    return ConstReverseIterator(buf_);
  }

  ~Vector() {
    Destroy(buf_, size_);
  }
};
