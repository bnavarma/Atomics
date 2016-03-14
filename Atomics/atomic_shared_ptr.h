#pragma once
#include <atomic>
#include <memory>
#include <windows.h>

template<typename T>
class atomic_shared_ptr {
	template<typename T>
	struct base {
		long long ref_ctr;
		T* raw_ptr;
	};

	std::atomic<base<T>*> atm_base{ nullptr };
public:
	constexpr atomic_shared_ptr() noexcept {}
	constexpr atomic_shared_ptr(std::nullptr_t) noexcept {}

	atomic_shared_ptr(T* ptr_) {
		/*resetp(ptr_);*/
		atm_base.store((atomic_shared_ptr<T>::base<T>*)_aligned_malloc(sizeof(base<T>) , 16));
		atm_base.load()->ref_ctr = 1;
		atm_base.load()->raw_ptr = ptr_;
	}

	atomic_shared_ptr(const T& other_) {
		reset(other_);
	}

	atomic_shared_ptr(const atomic_shared_ptr<T>& right_) {
		__int64 comparand[2];
		if (right_.atm_base.load() == nullptr) {
			throw;
		}
		auto temp_base = right_.atm_base.load();

		comparand[0] = temp_base->ref_ctr;
		comparand[1] = (__int64)temp_base->raw_ptr;
		while (!_InterlockedCompareExchange128((volatile __int64*)temp_base, comparand[1], comparand[0]+1, comparand)) {
			if (comparand[0] == 0) {
				return;
			}
		}
		atm_base.store(right_.atm_base.load());
	}

	T& operator=(const atomic_shared_ptr<T>& right_) {
		resetp();
		long long comparand[2];
		auto temp_base = right_.atm_base.load();

		comparand[0] = temp_base->ref_ctr;
		comparand[1] = (long long)temp_base->raw_ptr;
		while (!_InterlockedCompareExchange128(temp_base, comparand[0] + 1, comparand[1], comparand)) {
			if (comparand[0] == 0) {
				return (*this);
			}
		}
		atm_base.store(right_.atm_base.load());
		return (*this);
	}
};