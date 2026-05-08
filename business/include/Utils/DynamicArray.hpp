#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

template <typename T>
class DynamicArray {
public:
  DynamicArray() noexcept = default;

  explicit DynamicArray(std::size_t initial_capacity)
      : data_(initial_capacity > 0 ? new T[initial_capacity] : nullptr),
        size_(0),
        capacity_(initial_capacity) {}

  DynamicArray(std::initializer_list<T> init)
      : data_(init.size() > 0 ? new T[init.size()] : nullptr),
        size_(0),
        capacity_(init.size()) {
    for (const auto& value : init) {
      data_[size_++] = value;
    }
  }

  DynamicArray(const DynamicArray& other)
      : data_(other.capacity_ > 0 ? new T[other.capacity_] : nullptr),
        size_(other.size_),
        capacity_(other.capacity_) {
    for (std::size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
  }

  DynamicArray(DynamicArray&& other) noexcept
      : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  DynamicArray& operator=(const DynamicArray& other) {
    if (this == &other) return *this;

    T* new_data = other.capacity_ > 0 ? new T[other.capacity_] : nullptr;
    for (std::size_t i = 0; i < other.size_; ++i) new_data[i] = other.data_[i];

    delete[] data_;
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;
    return *this;
  }

  DynamicArray& operator=(DynamicArray&& other) noexcept {
    if (this == &other) return *this;

    delete[] data_;
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    return *this;
  }

  ~DynamicArray() { delete[] data_; }

  void add(const T& value) {
    ensure_capacity(size_ + 1);
    data_[size_++] = value;
  }

  void add(T&& value) {
    ensure_capacity(size_ + 1);
    data_[size_++] = std::move(value);
  }

  void remove(std::size_t index) {
    if (index >= size_) throw std::out_of_range("DynamicArray::remove");
    for (std::size_t i = index; i + 1 < size_; ++i) {
      data_[i] = std::move(data_[i + 1]);
    }
    --size_;
  }

  T& get(std::size_t index) {
    if (index >= size_) throw std::out_of_range("DynamicArray::get");
    return data_[index];
  }

  const T& get(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("DynamicArray::get");
    return data_[index];
  }

  T& operator[](std::size_t index) { return get(index); }
  const T& operator[](std::size_t index) const { return get(index); }

  T* begin() noexcept { return data_; }
  const T* begin() const noexcept { return data_; }
  T* end() noexcept { return data_ + size_; }
  const T* end() const noexcept { return data_ + size_; }

  std::size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }

private:
  void ensure_capacity(std::size_t required_capacity) {
    if (required_capacity <= capacity_) return;

    std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    while (new_capacity < required_capacity) new_capacity *= 2;

    T* new_data = new T[new_capacity];
    for (std::size_t i = 0; i < size_; ++i) new_data[i] = std::move(data_[i]);

    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
  }

private:
  T* data_ = nullptr;
  std::size_t size_ = 0;
  std::size_t capacity_ = 0;
};
