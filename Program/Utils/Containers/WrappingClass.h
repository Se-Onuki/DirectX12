#pragma once
#include "../SoLib/SoLib_Traits.h"
namespace SoLib::Containers {

	template <SoLib::IsRealType Derived, typename T>
	struct WrappingClass {
		WrappingClass() = default;
		~WrappingClass() = default;

		WrappingClass(const T &other) { *Get() = other; }
		WrappingClass(T &&other) { *Get() = other; }

		Derived &operator=(const T &other) { *Get() = other; return *GetDerived(); }
		Derived &operator=(T &&other) { *Get() = other;  return *GetDerived(); }

		/// @brief メンバ変数への参照を返す
		T *const Get() noexcept {
			return &(GetDerived()->*Derived::ptr());
		}
		const T *const Get() const noexcept {
			return &(GetDerived()->*Derived::ptr());
		}

		T &operator*() noexcept { return *Get(); }
		const T &operator*() const noexcept { return *Get(); }

		T *operator->() noexcept { return Get(); }
		const T *operator->() const noexcept { return Get(); }

		operator Vector3 &() noexcept { return *Get(); }
		operator const Vector3 &() const noexcept { return *Get(); }

		bool operator==(const T &r) const noexcept { return *Get() == r; }

	private:

		Derived *const GetDerived() noexcept { return reinterpret_cast<Derived *>(this); }
		const Derived *const GetDerived() const noexcept { return reinterpret_cast<const Derived *>(this); }

	};
}