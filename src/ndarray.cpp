#include "ndarray.h"

#include <gdconvert/conversion_axes.h>             // for variant_to_axes
#include <vatensor/comparison.h>                   // for equal_to, greater
#include <vatensor/logical.h>                      // for logical_and, logic...
#include <vatensor/reduce.h>                       // for max, mean, min, prod
#include <vatensor/trigonometry.h>                 // for acos, acosh, asin
#include <vatensor/vmath.h>                        // for abs, add, deg2rad
#include <algorithm>                               // for copy
#include <functional>                              // for function
#include <stdexcept>                               // for runtime_error
#include <variant>                                 // for visit
#include "gdconvert/conversion_array.h"            // for varray_to_packed
#include "gdconvert/conversion_slice.h"            // for variants_as_slice_...
#include "gdconvert/conversion_string.h"           // for xt_to_string
#include "godot_cpp/classes/global_constants.hpp"  // for MethodFlags
#include "godot_cpp/core/class_db.hpp"             // for D_METHOD, ClassDB
#include "godot_cpp/core/error_macros.hpp"         // for ERR_FAIL_V_MSG
#include "godot_cpp/core/memory.hpp"               // for _post_initialize
#include "godot_cpp/variant/string_name.hpp"       // for StringName
#include "godot_cpp/variant/variant.hpp"           // for Variant
#include "nd.h"                                    // for nd
#include "vatensor/round.h"                        // for ceil, floor, nearb...
#include "vatensor/varray.h"                       // for VArray, Axes, cons...
#include "xtensor/xiterator.hpp"                   // for operator==
#include "xtensor/xlayout.hpp"                     // for layout_type
#include "xtensor/xstrided_view.hpp"               // for xstrided_slice_vector
#include "xtl/xiterator_base.hpp"                  // for operator!=

using namespace godot;

void NDArray::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("dtype"), &NDArray::dtype);
	godot::ClassDB::bind_method(D_METHOD("shape"), &NDArray::shape);
	godot::ClassDB::bind_method(D_METHOD("size"), &NDArray::size);
	godot::ClassDB::bind_method(D_METHOD("array_size_in_bytes"), &NDArray::array_size_in_bytes);
	godot::ClassDB::bind_method(D_METHOD("ndim"), &NDArray::ndim);

	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "set", &NDArray::set);
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "get", &NDArray::get);
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "get_float", &NDArray::get_float);
	ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "get_int", &NDArray::get_int);

	godot::ClassDB::bind_method(D_METHOD("as_type", "type"), &NDArray::as_type);

	godot::ClassDB::bind_method(D_METHOD("to_float"), &NDArray::to_float);
	godot::ClassDB::bind_method(D_METHOD("to_int"), &NDArray::to_int);

	godot::ClassDB::bind_method(D_METHOD("to_packed_float32_array"), &NDArray::to_packed_float32_array);
	godot::ClassDB::bind_method(D_METHOD("to_packed_float64_array"), &NDArray::to_packed_float64_array);
	godot::ClassDB::bind_method(D_METHOD("to_packed_byte_array"), &NDArray::to_packed_byte_array);
	godot::ClassDB::bind_method(D_METHOD("to_packed_int32_array"), &NDArray::to_packed_int32_array);
	godot::ClassDB::bind_method(D_METHOD("to_packed_int64_array"), &NDArray::to_packed_int64_array);
	godot::ClassDB::bind_method(D_METHOD("to_godot_array"), &NDArray::to_godot_array);

	godot::ClassDB::bind_method(D_METHOD("assign_add", "a", "b"), &NDArray::assign_add);
	godot::ClassDB::bind_method(D_METHOD("assign_subtract", "a", "b"), &NDArray::assign_subtract);
	godot::ClassDB::bind_method(D_METHOD("assign_multiply", "a", "b"), &NDArray::assign_multiply);
	godot::ClassDB::bind_method(D_METHOD("assign_divide", "a", "b"), &NDArray::assign_divide);
	godot::ClassDB::bind_method(D_METHOD("assign_remainder", "a", "b"), &NDArray::assign_remainder);
	godot::ClassDB::bind_method(D_METHOD("assign_pow", "a", "b"), &NDArray::assign_pow);

	godot::ClassDB::bind_method(D_METHOD("assign_minimum", "a", "b"), &NDArray::assign_minimum);
	godot::ClassDB::bind_method(D_METHOD("assign_maximum", "a", "b"), &NDArray::assign_maximum);

	godot::ClassDB::bind_method(D_METHOD("assign_sign", "a"), &NDArray::assign_sign);
	godot::ClassDB::bind_method(D_METHOD("assign_abs", "a"), &NDArray::assign_abs);
	godot::ClassDB::bind_method(D_METHOD("assign_sqrt", "a"), &NDArray::assign_sqrt);

	godot::ClassDB::bind_method(D_METHOD("assign_exp", "a"), &NDArray::assign_exp);
	godot::ClassDB::bind_method(D_METHOD("assign_log", "a"), &NDArray::assign_log);

	godot::ClassDB::bind_method(D_METHOD("assign_rad2deg", "a"), &NDArray::assign_rad2deg);
	godot::ClassDB::bind_method(D_METHOD("assign_deg2rad", "a"), &NDArray::assign_deg2rad);

	godot::ClassDB::bind_method(D_METHOD("assign_sin", "a"), &NDArray::assign_sin);
	godot::ClassDB::bind_method(D_METHOD("assign_cos", "a"), &NDArray::assign_cos);
	godot::ClassDB::bind_method(D_METHOD("assign_tan", "a"), &NDArray::assign_tan);
	godot::ClassDB::bind_method(D_METHOD("assign_asin", "a"), &NDArray::assign_asin);
	godot::ClassDB::bind_method(D_METHOD("assign_acos", "a"), &NDArray::assign_acos);
	godot::ClassDB::bind_method(D_METHOD("assign_atan", "a"), &NDArray::assign_atan);
	godot::ClassDB::bind_method(D_METHOD("assign_atan2", "x1", "x2"), &NDArray::assign_atan2);

	godot::ClassDB::bind_method(D_METHOD("assign_sinh", "a"), &NDArray::assign_sinh);
	godot::ClassDB::bind_method(D_METHOD("assign_cosh", "a"), &NDArray::assign_cosh);
	godot::ClassDB::bind_method(D_METHOD("assign_tanh", "a"), &NDArray::assign_tanh);
	godot::ClassDB::bind_method(D_METHOD("assign_asinh", "a"), &NDArray::assign_asinh);
	godot::ClassDB::bind_method(D_METHOD("assign_acosh", "a"), &NDArray::assign_acosh);
	godot::ClassDB::bind_method(D_METHOD("assign_atanh", "a"), &NDArray::assign_atanh);

	godot::ClassDB::bind_method(D_METHOD("assign_sum", "a", "axes"), &NDArray::assign_sum, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_prod", "a", "axes"), &NDArray::assign_sum, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_mean", "a", "axes"), &NDArray::assign_mean, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_var", "a", "axes"), &NDArray::assign_var, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_std", "a", "axes"), &NDArray::assign_std, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_max", "a", "axes"), &NDArray::assign_max, DEFVAL(nullptr), DEFVAL(nullptr));
	godot::ClassDB::bind_method(D_METHOD("assign_min", "a", "axes"), &NDArray::assign_min, DEFVAL(nullptr), DEFVAL(nullptr));

	godot::ClassDB::bind_method(D_METHOD("assign_floor", "a"), &NDArray::assign_floor);
    godot::ClassDB::bind_method(D_METHOD("assign_ceil", "a"), &NDArray::assign_ceil);
    godot::ClassDB::bind_method(D_METHOD("assign_round", "a"), &NDArray::assign_round);
    godot::ClassDB::bind_method(D_METHOD("assign_trunc", "a"), &NDArray::assign_trunc);
	godot::ClassDB::bind_method(D_METHOD("assign_rint", "a"), &NDArray::assign_rint);

	godot::ClassDB::bind_method(D_METHOD("assign_equal", "a", "b"), &NDArray::assign_equal);
	godot::ClassDB::bind_method(D_METHOD("assign_not_equal", "a", "b"), &NDArray::assign_not_equal);
	godot::ClassDB::bind_method(D_METHOD("assign_greater", "a", "b"), &NDArray::assign_greater);
	godot::ClassDB::bind_method(D_METHOD("assign_greater_equal", "a", "b"), &NDArray::assign_greater_equal);
	godot::ClassDB::bind_method(D_METHOD("assign_less", "a", "b"), &NDArray::assign_less);
	godot::ClassDB::bind_method(D_METHOD("assign_less_equal", "a", "b"), &NDArray::assign_less_equal);

	godot::ClassDB::bind_method(D_METHOD("assign_logical_and", "a", "b"), &NDArray::assign_logical_and);
	godot::ClassDB::bind_method(D_METHOD("assign_logical_or", "a", "b"), &NDArray::assign_logical_or);
	godot::ClassDB::bind_method(D_METHOD("assign_logical_not", "a"), &NDArray::assign_logical_not);
}

NDArray::NDArray() = default;

NDArray::~NDArray() = default;

String NDArray::_to_string() const {
	return std::visit([](auto&& arg){ return xt_to_string(arg); }, array.to_compute_variant());
}

va::DType NDArray::dtype() const {
	return array.dtype();
}

PackedInt64Array NDArray::shape() const {
	return packed_from_sequence<PackedInt64Array>(array.shape);
}

uint64_t NDArray::size() const {
	return array.size();
}

uint64_t NDArray::array_size_in_bytes() const {
	return array.size_of_array_in_bytes();
}

uint64_t NDArray::ndim() const {
	return array.dimension();
}

Variant NDArray::as_type(va::DType dtype) const {
	return nd::as_array(this, dtype);
}

void NDArray::set(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
	if (arg_count < 1) {
		ERR_FAIL_MSG("First argument (value) must be set. Ignoring assignment.");
	}

	try {
		const Variant &value = *args[0];
		// todo don't need slices if arg_count == 1
		auto slices = variants_as_slice_vector(args + 1, arg_count - 1, error);
		va::VArray sliced = arg_count == 1 ? array : array.slice(slices);

		switch (value.get_type()) {
			case Variant::INT:
				array.fill(static_cast<int64_t>(value));
				return;
			case Variant::FLOAT:
				array.fill(static_cast<double_t>(value));
				return;
			// TODO We could optimize more assignments of literals.
			//  Just need to figure out how, ideally without duplicating code - as_array already does much type checking work.
			default:
				va::VArray a_ = variant_as_array(value);

				array.set_with_array(a_);
				return;
		}
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_MSG(error.what());
	}
}

Ref<NDArray> NDArray::get(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
	try {
		xt::xstrided_slice_vector sv = variants_as_slice_vector(args, arg_count, error);

		auto result = array.slice(sv);
		return {memnew(NDArray(result))};
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}

double_t NDArray::get_float(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
	try {
		xt::xstrided_slice_vector sv = variants_as_slice_vector(args, arg_count, error);
		return va::constant_to_type<double_t>(array.slice(sv).to_single_value());
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_V_MSG(0, error.what());
	}
}

int64_t NDArray::get_int(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
	try {
		xt::xstrided_slice_vector sv = variants_as_slice_vector(args, arg_count, error);
		return va::constant_to_type<int64_t>(array.slice(sv).to_single_value());
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_V_MSG(0, error.what());
	}
}

double_t NDArray::to_float() const {
	try {
		return va::constant_to_type<double_t>(array.to_single_value());
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_V_MSG(0, error.what());
	}
}

int64_t NDArray::to_int() const {
	try {
		return va::constant_to_type<int64_t>(array.to_single_value());
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_V_MSG(0, error.what());
	}
}

PackedFloat32Array NDArray::to_packed_float32_array() const {
	return varray_to_packed<PackedFloat32Array>(array);
}

PackedFloat64Array NDArray::to_packed_float64_array() const {
	return varray_to_packed<PackedFloat64Array>(array);
}

PackedByteArray NDArray::to_packed_byte_array() const {
	return varray_to_packed<PackedByteArray>(array);
}

PackedInt32Array NDArray::to_packed_int32_array() const {
	return varray_to_packed<PackedInt32Array>(array);
}

PackedInt64Array NDArray::to_packed_int64_array() const {
	return varray_to_packed<PackedInt64Array>(array);
}

Array NDArray::to_godot_array() const {
	return varray_to_godot_array(array);
}

template <typename Visitor, typename... Args>
void map_variants_as_arrays_inplace(Visitor visitor, Args... args) {
    try {
        visitor(variant_as_array(args)...);
    }
    catch (std::runtime_error& error) {
        ERR_FAIL_MSG(error.what());
    }
}

inline void reduction_inplace(std::function<void(const va::VArray&, const va::Axes&)> visitor, Variant a, Variant axes) {
	try {
		const auto axes_ = variant_to_axes(axes);
		const auto a_ = variant_as_array(a);

		visitor(a_, axes_);
	}
	catch (std::runtime_error& error) {
		ERR_FAIL_MSG(error.what());
	}
}

#define UNARY_MAP(func, varray1) \
	map_variants_as_arrays_inplace([this](const va::VArray& varray) {\
		auto compute_variant = array.to_compute_variant();\
        va::func(&compute_variant, varray);\
    }, (varray1))

#define BINARY_MAP(func, varray1, varray2) \
	map_variants_as_arrays_inplace([this](const va::VArray& a, const va::VArray& b) {\
		auto compute_variant = array.to_compute_variant();\
        va::func(&compute_variant, a, b);\
    }, (varray1), (varray2))

#define REDUCTION(func, varray1, axes1) \
	reduction_inplace([this](const va::VArray& array, const va::Axes& axes) {\
		auto compute_variant = array.to_compute_variant();\
		va::func(&compute_variant, array, axes1);\
	}, (varray1), (axes1))

void NDArray::assign_add(Variant a, Variant b) const {
	// godot::UtilityFunctions::print(value);
	BINARY_MAP(add, a, b);
}

void NDArray::assign_subtract(Variant a, Variant b) const {
	BINARY_MAP(subtract, a, b);
}

void NDArray::assign_multiply(Variant a, Variant b) const {
	BINARY_MAP(multiply, a, b);
}

void NDArray::assign_divide(Variant a, Variant b) const {
	BINARY_MAP(divide, a, b);
}

void NDArray::assign_remainder(Variant a, Variant b) const {
	BINARY_MAP(remainder, a, b);
}

void NDArray::assign_pow(Variant a, Variant b) const {
	BINARY_MAP(pow, a, b);
}

void NDArray::assign_minimum(Variant a, Variant b) const {
	BINARY_MAP(minimum, a, b);
}

void NDArray::assign_maximum(Variant a, Variant b) const {
	BINARY_MAP(maximum, a, b);
}

void NDArray::assign_sign(Variant a) const {
	UNARY_MAP(sign, a);
}

void NDArray::assign_abs(Variant a) const {
	UNARY_MAP(abs, a);
}

void NDArray::assign_sqrt(Variant a) const {
	UNARY_MAP(sqrt, a);
}

void NDArray::assign_exp(Variant a) const {
	UNARY_MAP(exp, a);
}

void NDArray::assign_log(Variant a) const {
	UNARY_MAP(log, a);
}

void NDArray::assign_rad2deg(Variant a) const {
	UNARY_MAP(rad2deg, a);
}

void NDArray::assign_deg2rad(Variant a) const {
	UNARY_MAP(deg2rad, a);
}

void NDArray::assign_sin(Variant a) const {
	UNARY_MAP(sin, a);
}

void NDArray::assign_cos(Variant a) const {
	UNARY_MAP(cos, a);
}

void NDArray::assign_tan(Variant a) const {
	UNARY_MAP(tan, a);
}

void NDArray::assign_asin(Variant a) const {
	UNARY_MAP(asin, a);
}

void NDArray::assign_acos(Variant a) const {
	UNARY_MAP(acos, a);
}

void NDArray::assign_atan(Variant a) const {
	UNARY_MAP(atan, a);
}

void NDArray::assign_atan2(Variant x1, Variant x2) const {
	BINARY_MAP(atan2, x1, x2);
}

void NDArray::assign_sinh(Variant a) const {
	UNARY_MAP(sinh, a);
}

void NDArray::assign_cosh(Variant a) const {
	UNARY_MAP(cosh, a);
}

void NDArray::assign_tanh(Variant a) const {
	UNARY_MAP(tanh, a);
}

void NDArray::assign_asinh(Variant a) const {
	UNARY_MAP(asinh, a);
}

void NDArray::assign_acosh(Variant a) const {
	UNARY_MAP(acosh, a);
}

void NDArray::assign_atanh(Variant a) const {
	UNARY_MAP(atanh, a);
}

void NDArray::assign_sum(Variant a, Variant axes) const {
	REDUCTION(sum, a, axes);
}

void NDArray::assign_prod(Variant a, Variant axes) const {
	REDUCTION(prod, a, axes);
}

void NDArray::assign_mean(Variant a, Variant axes) const {
	REDUCTION(mean, a, axes);
}

void NDArray::assign_var(Variant a, Variant axes) const {
	REDUCTION(var, a, axes);
}

void NDArray::assign_std(Variant a, Variant axes) const {
	REDUCTION(std, a, axes);
}

void NDArray::assign_max(Variant a, Variant axes) const {
	REDUCTION(max, a, axes);
}

void NDArray::assign_min(Variant a, Variant axes) const {
	REDUCTION(min, a, axes);
}

void NDArray::assign_floor(Variant a) const {
	UNARY_MAP(floor, a);
}

void NDArray::assign_ceil(Variant a) const {
	UNARY_MAP(ceil, a);
}

void NDArray::assign_round(Variant a) const {
	UNARY_MAP(round, a);
}

void NDArray::assign_trunc(Variant a) const {
	UNARY_MAP(trunc, a);
}

void NDArray::assign_rint(Variant a) const {
	// Actually uses nearbyint because rint can throw, which is undesirable in our case, and unlike numpy's behavior.
	UNARY_MAP(nearbyint, a);
}

void NDArray::assign_equal(Variant a, Variant b) const {
	BINARY_MAP(equal_to, a, b);
}

void NDArray::assign_not_equal(Variant a, Variant b) const {
	BINARY_MAP(not_equal_to, a, b);
}

void NDArray::assign_greater(Variant a, Variant b) const {
	BINARY_MAP(greater, a, b);
}

void NDArray::assign_greater_equal(Variant a, Variant b) const {
	BINARY_MAP(greater_equal, a, b);
}

void NDArray::assign_less(Variant a, Variant b) const {
	BINARY_MAP(less, a, b);
}

void NDArray::assign_less_equal(Variant a, Variant b) const {
	BINARY_MAP(less_equal, a, b);
}

void NDArray::assign_logical_and(Variant a, Variant b) const {
	BINARY_MAP(logical_and, a, b);
}

void NDArray::assign_logical_or(Variant a, Variant b) const {
	BINARY_MAP(logical_or, a, b);
}

void NDArray::assign_logical_not(Variant a) const {
	UNARY_MAP(logical_not, a);
}
