#pragma once

#include <new>
#include <utility>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>

namespace nonstd
{
namespace allocator
{
#if __cplusplus >= 201103L && __cplusplus <= 201402L
  template<typename T, size_t N>
  struct humble
  {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;

    template<typename U>
    struct rebind
    {
        using other = humble<U, N>;
    };

    ~humble()
    {}

    humble()
      : storage_(reinterpret_cast<T *>(malloc(max_bytes())), &std::free)
      , end_((storage_ ? storage_.get() : nullptr))
    {
      if (end_ == nullptr)
	throw std::bad_alloc();
    }

    humble(const humble& other)
      : storage_(reinterpret_cast<T *>(malloc(max_bytes())), &std::free)
      , end_((storage_.get() && other.storage_.get() ? storage_.get() + (other.end_ - other.storage_.get()) : nullptr))
    {
      if (end_ && storage_.get() <= end_ && end_ <= storage_.get() + N)
        std::memmove(storage_.get(), other.storage_.get(), max_bytes());
      else
	throw std::bad_alloc();
    }

    humble(humble&& other)
      : storage_(std::move(other.storage_))
      , end_(other.end_)
    {
      if (!(storage_.get() && end_ && storage_.get() <= end_ && end_ <= storage_.get() + N))
	throw std::bad_alloc();
    }

    humble& operator=(const humble& other)
    {
      if (storage_.get() && other.storage_.get() && other.end_)
      {
	std::memmove(storage_.get(), other.storage_.get(), max_bytes());
	end_ = storage_.get() + (other.end_ - other.storage_.get());
      }
      else
	throw std::bad_alloc();
      return *this;
    }

    humble& operator=(humble&& other)
    {
      storage_ = std::move(other.storage_);
      end_ = other.storage_;
      if (!(storage_.get() && end_ && storage_.get() <= end_ && end_ <= storage_.get() + N))
	throw std::bad_alloc();
      return *this;
    }

    pointer allocate(std::size_t n)
    {
      if (end_ + n > storage_.get() + N)
	throw std::bad_alloc();

      auto p = end_;

      if (!p)
	throw std::bad_alloc();

      end_ += n;

      return reinterpret_cast<T *>(p);
    }

    void deallocate(T *p, std::size_t n)
    {
      if ((storage_.get() <= p) && (p + n <= storage_.get() + N))
      {
	std::memmove(p, p + n, n * sizeof(T));
	end_ -= n;
      }
      else
	throw std::out_of_range(": someone doing something nasty!");
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args)
    {
      new(p) U(std::forward<Args>(args)...);
    }

    void destroy(T *p)
    {
      p->~T();
    }

    constexpr size_type max_size() const noexcept
    {
      return N;
    }

    constexpr size_type max_bytes() const noexcept
    {
      return N * sizeof(T);
    }

    std::unique_ptr<T, decltype(&std::free)> storage_;
    T * end_;
  };
#endif // __cplusplus <= 201402L
} // allocator
} //nonstd
