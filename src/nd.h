#ifndef NUMDOT_ND_H
#define NUMDOT_ND_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"

#include "xtv.h"

using namespace godot;

// From https://github.com/xtensor-stack/xtensor/issues/1413
template <class E>
String xt_to_string(const xt::xexpression<E>& e)
{
    std::ostringstream out;
    out << e;
    return String(out.str().c_str());
}

class nd : public Object {
	GDCLASS(nd, Object)

protected:
	static void _bind_methods();

public:
	// Godot needs nd::DType here.
	using DType = xtv::DType;

	nd();
	~nd();

	static DType dtype(Variant array);
	static PackedInt64Array shape(Variant array);
	static uint64_t size(Variant array);
	static uint64_t ndim(Variant array);
	
	static Variant as_type(Variant array, DType dtype);
	static Variant as_array(Variant array, DType dtype = DType::DTypeMax);
	static Variant array(Variant array, DType dtype = DType::DTypeMax);

	static Variant zeros(Variant shape, DType dtype = DType::Float64);
	static Variant ones(Variant shape, DType dtype = DType::Float64);

	static Variant add(Variant a, Variant b);
	static Variant subtract(Variant a, Variant b);
	static Variant multiply(Variant a, Variant b);
	static Variant divide(Variant a, Variant b);

	static Variant sin(Variant a);
	static Variant cos(Variant a);
	static Variant tan(Variant a);
};

VARIANT_ENUM_CAST(nd::DType);

#endif
