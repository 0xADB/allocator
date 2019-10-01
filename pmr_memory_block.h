#pragma once

#include "unlikely.h"
#include <cstddef>
#include <utility>
#include <mutex>
#include <atomic>

#ifdef MEMORY_BLOCK_TRACING
#include <iostream>
#endif

#if __cplusplus > 201402L
#include <memory_resource>

namespace nonstd
{
  namespace pmr
  {
    template<size_t N>
      class memory_block : public std::pmr::memory_resource
      {
	public:
	  using byte_type = std::byte;
	  using size_type = size_t;

	  virtual ~memory_block() override
	  {
#ifdef MEMORY_BLOCK_TRACING
	    std::cout << __PRETTY_FUNCTION__ << ": " << --count << std::endl;
#endif
	    std::lock_guard<std::mutex> lock(_mutex);
	    if (_storage)
	      _upstream->deallocate(const_cast<byte_type *>(_storage), static_cast<size_t>(_storage_end - _storage));
	  }

	  memory_block(std::pmr::memory_resource * upstream = std::pmr::get_default_resource())
	    : _upstream(upstream)
	  {}

	  memory_block(size_type bytes, std::pmr::memory_resource * upstream = std::pmr::get_default_resource())
	    : _upstream(upstream)
	    , _storage(static_cast<byte_type *>(upstream->allocate(N * bytes)))
	    , _storage_end(_storage + N * bytes)
	    , _end(_storage)
	    {
#ifdef MEMORY_BLOCK_TRACING
	      std::cout << __PRETTY_FUNCTION__ << ": " << ++count << std::endl;
#endif
	    }

	  bool is_pointed_by(const void * p, size_type size = 0) const noexcept
	  {
	    std::lock_guard<std::mutex> lock(_mutex);
	    return (initialized()
		&& (_storage <= reinterpret_cast<const byte_type *>(p))
		&& (reinterpret_cast<const byte_type *>(p) < _storage_end)
		&& (size == 0 || reinterpret_cast<const byte_type *>(p) + size <= _storage_end)
		);
	  }
	  
	  bool initialized() const
	  {
	    return (_storage && _storage_end);
	  }

	  bool empty() const
	  {
	    return (_stored.load() == 0);
	  }

	  size_type size() const
	  {
	    return _stored.load();
	  }

	protected:

	  void initialize(size_type bytes)
	  {
	    _storage = static_cast<byte_type *>(_upstream->allocate(N * bytes));
	    _storage_end = _storage + N * bytes;
	    _end = _storage;
	  }

	  void * do_allocate(size_type bytes, size_type alignment) override
	  {
	    std::lock_guard<std::mutex> lock(_mutex);
	    if (!initialized())
	      initialize(bytes);

	    void * p = nullptr;
	    if (_end + bytes <= _storage_end)
	    {
	      p = _end;
	      _end += bytes;
	      _stored += bytes;
#ifdef MEMORY_BLOCK_TRACING
	      std::cout << __PRETTY_FUNCTION__ << ": allocated: " << bytes << std::endl;
#endif
	    }
	    else 
	    {
#ifdef MEMORY_BLOCK_TRACING
	      std::cerr << __PRETTY_FUNCTION__ << ": no room for: " << bytes << std::endl;
#endif
	      throw std::bad_alloc();
	    }
	    return p;
	  }

	  void do_deallocate(void * p, size_type size, size_type alignment) override
	  {
	    std::lock_guard<std::mutex> lock(_mutex);
	    if (initialized() && _stored.load() && _storage <= p && p < _end)
	    {
#ifdef MEMORY_BLOCK_TRACING
	      std::cout << __PRETTY_FUNCTION__ << ": " << size << std::endl;
#endif
	      _stored -= size;
	    }
	    else
	      throw std::invalid_argument("wrong pointer or size");
	  }

	  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
	  {
	    return (this == &other);
	  }

	private:
	  std::pmr::memory_resource * _upstream = nullptr;
	  byte_type * _storage = nullptr;     //! block's beginning
	  byte_type * _storage_end = nullptr; //! block's end
	  byte_type * _end = nullptr;               //! end of space used at least once
	  std::atomic<size_type> _stored{};
	  std::atomic<size_type> _refcnt{1};
	  mutable std::mutex _mutex{};

#ifdef MEMORY_BLOCK_TRACING
	  static std::atomic_int count;
#endif
      };

  } // memory
} // nonstd

#endif // __cplusplus > 201402L
