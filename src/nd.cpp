#include "nd.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xoperation.hpp"

#include "conversion_array.h"
#include "conversion_shape.h"
#include "conversion_slice.h"
#include "conversion_axes.h"
#include "xtv.h"
#include "ndarray.h"
#include "ndrange.h"

using namespace godot;
using namespace xtv;

void nd::_bind_methods() {
	BIND_ENUM_CONSTANT(Float64);
	BIND_ENUM_CONSTANT(Float32);
	BIND_ENUM_CONSTANT(Int8);
	BIND_ENUM_CONSTANT(Int16);
	BIND_ENUM_CONSTANT(Int32);
	BIND_ENUM_CONSTANT(Int64);
	BIND_ENUM_CONSTANT(UInt8);
	BIND_ENUM_CONSTANT(UInt16);
	BIND_ENUM_CONSTANT(UInt32);
	BIND_ENUM_CONSTANT(UInt64);

	godot::ClassDB::bind_static_method("nd", D_METHOD("newaxis"), &nd::newaxis);
	godot::ClassDB::bind_static_method("nd", D_METHOD("from", "start"), &nd::from);
	godot::ClassDB::bind_static_method("nd", D_METHOD("to", "stop"), &nd::to);
	godot::ClassDB::bind_static_method("nd", D_METHOD("range", "start", "stop"), &nd::range);
	godot::ClassDB::bind_static_method("nd", D_METHOD("range_step", "start", "stop", "step"), &nd::range_step);

	godot::ClassDB::bind_static_method("nd", D_METHOD("dtype", "array"), &nd::dtype);
	godot::ClassDB::bind_static_method("nd", D_METHOD("size_of_dtype_in_bytes", "dtype"), &nd::size_of_dtype_in_bytes);
	godot::ClassDB::bind_static_method("nd", D_METHOD("shape", "array"), &nd::shape);
	godot::ClassDB::bind_static_method("nd", D_METHOD("size", "array"), &nd::size);
	godot::ClassDB::bind_static_method("nd", D_METHOD("ndim", "array"), &nd::ndim);

	godot::ClassDB::bind_static_method("nd", D_METHOD("as_type", "array", "dtype"), &nd::as_type);

	godot::ClassDB::bind_static_method("nd", D_METHOD("as_array", "array", "dtype"), &nd::as_array, DEFVAL(nullptr), DEFVAL(nd::DType::DTypeMax));
	godot::ClassDB::bind_static_method("nd", D_METHOD("array", "array", "dtype"), &nd::array, DEFVAL(nullptr), DEFVAL(nd::DType::DTypeMax));

	godot::ClassDB::bind_static_method("nd", D_METHOD("full", "shape", "fill_value", "dtype"), &nd::full, DEFVAL(nullptr), DEFVAL(nullptr), DEFVAL(nd::DType::Float64));
	godot::ClassDB::bind_static_method("nd", D_METHOD("zeros", "shape", "dtype"), &nd::zeros, DEFVAL(nullptr), DEFVAL(nd::DType::Float64));
	godot::ClassDB::bind_static_method("nd", D_METHOD("ones", "shape", "dtype"), &nd::ones, DEFVAL(nullptr), DEFVAL(nd::DType::Float64));

	godot::ClassDB::bind_static_method("nd", D_METHOD("add", "a", "b"), &nd::add);
	godot::ClassDB::bind_static_method("nd", D_METHOD("subtract", "a", "b"), &nd::subtract);
	godot::ClassDB::bind_static_method("nd", D_METHOD("multiply", "a", "b"), &nd::multiply);
	godot::ClassDB::bind_static_method("nd", D_METHOD("divide", "a", "b"), &nd::divide);
	godot::ClassDB::bind_static_method("nd", D_METHOD("remainder", "a", "b"), &nd::remainder);
	godot::ClassDB::bind_static_method("nd", D_METHOD("pow", "a", "b"), &nd::pow);

	godot::ClassDB::bind_static_method("nd", D_METHOD("abs", "a"), &nd::abs);
	godot::ClassDB::bind_static_method("nd", D_METHOD("sqrt", "a"), &nd::sqrt);

	godot::ClassDB::bind_static_method("nd", D_METHOD("exp", "a"), &nd::exp);
	godot::ClassDB::bind_static_method("nd", D_METHOD("log", "a"), &nd::log);

	godot::ClassDB::bind_static_method("nd", D_METHOD("sin", "a"), &nd::sin);
	godot::ClassDB::bind_static_method("nd", D_METHOD("cos", "a"), &nd::cos);
	godot::ClassDB::bind_static_method("nd", D_METHOD("tan", "a"), &nd::tan);

	godot::ClassDB::bind_static_method("nd", D_METHOD("mean", "a"), &nd::mean, DEFVAL(nullptr), DEFVAL(nullptr));
}

nd::nd() {
}

nd::~nd() {
	// Add your cleanup here.
}

StringName nd::newaxis() {
	return ::newaxis();
}

// Not needed right now, only needed again if static varargs methods are supported
// range_part to_range_part(const Variant& variant) {
// 	switch (variant.get_type()) {
// 		case Variant::INT:
// 			return int64_t(variant);
// 		case NULL:
// 			return xt::placeholders::xtuph{};
// 		default:
// 			throw std::runtime_error("Invalid type for range.");
// 	}
// }

// Variant nd::range(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
// 	try {
// 		switch (arg_count) {
// 			case 1:
// 				return Variant(memnew(NDRange(xt::placeholders::xtuph{}, to_range_part(*args[0]), xt::placeholders::xtuph{})));
// 			case 2:
// 				return Variant(memnew(NDRange(to_range_part(*args[0]), to_range_part(*args[1]), xt::placeholders::xtuph{})));
// 			case 3:
// 				return Variant(memnew(NDRange(to_range_part(*args[0]), to_range_part(*args[1]), to_range_part(*args[2]))));
// 			default:
// 				ERR_FAIL_V_MSG(nullptr, "Argument list not valid for a range, pass 1-3 arguments.");
// 		}
// 	}
// 	catch (std::runtime_error error) {
// 		ERR_FAIL_V_MSG(nullptr, error.what());
// 	}
// }

Ref<NDRange> nd::from(int64_t start) {
	return Ref(memnew(NDRange(start, xt::placeholders::xtuph{}, xt::placeholders::xtuph{})));
}

Ref<NDRange> nd::to(int64_t stop) {
	return Ref(memnew(NDRange(xt::placeholders::xtuph{}, stop, xt::placeholders::xtuph{})));
}

Ref<NDRange> nd::range(int64_t start, int64_t stop) {
	return Ref(memnew(NDRange(start, stop, xt::placeholders::xtuph{})));
}

Ref<NDRange> nd::range_step(int64_t start, int64_t stop, int64_t step) {
	return Ref(memnew(NDRange(start, stop, step)));
}

nd::DType nd::dtype(Variant array) {
	// TODO We can totally do this without constructing an array. More code though.
	std::shared_ptr<xtv::XTVariant> existing_array;
	if (!variant_as_array(array, existing_array)) {
		ERR_FAIL_V_MSG(nd::DType::DTypeMax, "Not an array.");
	}

	return xtv::dtype(*existing_array);
}

uint64_t nd::size_of_dtype_in_bytes(DType dtype) {
	return xtv::size_of_dtype_in_bytes(dtype);
}

PackedInt64Array nd::shape(Variant array) {
	// TODO We can totally do this without constructing an array. More code though.
	std::shared_ptr<xtv::XTVariant> existing_array;
	if (!variant_as_array(array, existing_array)) {
		ERR_FAIL_V_MSG(PackedInt64Array(), "Not an array.");
	}

	auto shape = xtv::shape(*existing_array);
	// TODO This seems a bit weird, but it works for now.
	auto packed = PackedInt64Array();
	for (auto d : shape) {
		packed.append(d);
	}
	return packed;
}

uint64_t nd::size(Variant array) {
	// TODO We can totally do this without constructing an array. More code though.
	std::shared_ptr<xtv::XTVariant> existing_array;
	if (!variant_as_array(array, existing_array)) {
		ERR_FAIL_V_MSG(nd::DType::DTypeMax, "Not an array.");
	}

	return xtv::size(*existing_array);
}

uint64_t nd::ndim(Variant array) {
	// TODO We can totally do this without constructing an array. More code though.
	std::shared_ptr<xtv::XTVariant> existing_array;
	if (!variant_as_array(array, existing_array)) {
		ERR_FAIL_V_MSG(nd::DType::DTypeMax, "Not an array.");
	}

	return xtv::dimension(*existing_array);
}

Ref<NDArray> nd::as_type(Variant array, nd::DType dtype) {
	return nd::as_array(array, dtype);
}

Ref<NDArray> nd::as_array(Variant array, nd::DType dtype) {
	auto type = array.get_type();

	// Can we take a view?
	if (type == Variant::OBJECT) {
		if (auto ndarray = dynamic_cast<NDArray*>((Object*)(array))) {
			if (dtype == nd::DType::DTypeMax || ndarray->dtype() == dtype) {
				return array;
			}
		}
	}

	// Ok, we need a copy of the data.
	return nd::array(array, dtype);
}

Ref<NDArray> nd::array(Variant array, nd::DType dtype) {
	auto type = array.get_type();

	std::shared_ptr<xtv::XTVariant> existing_array;
	if (!variant_as_array(array, existing_array)) {
		return Ref<NDArray>();
	}

	// Default value.
	if (dtype == nd::DType::DTypeMax) {
		dtype = xtv::dtype(*existing_array);
	}

	try {
		auto result = xtv::make_xarray(dtype, *existing_array);
		return Ref(memnew(NDArray(result)));
	}
	catch (std::runtime_error error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}


template <typename V>
Ref<NDArray> _full(Variant shape, V value, nd::DType dtype) {
	std::vector<size_t> shape_array;
	if (!variant_as_shape<size_t>(shape, shape_array)) {
		return Ref<NDArray>();
	}

	try {
		return Ref(memnew(NDArray(xtv::full(dtype, value, shape_array))));
	}
	catch (std::runtime_error error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}

Ref<NDArray> nd::full(Variant shape, Variant fill_value, nd::DType dtype) {
	switch (fill_value.get_type()) {
		case Variant::INT:
			if (dtype == nd::DType::DTypeMax) dtype = nd::DType::Int64;
			return _full(shape, int64_t(fill_value), dtype);
		case Variant::FLOAT:
			if (dtype == nd::DType::DTypeMax) dtype = nd::DType::Float64;
			return _full(shape, double_t(fill_value), dtype);
		default:
			ERR_FAIL_V_MSG(Ref<NDArray>(), "The fill value must be a number literal (for now).");
	}
}

Ref<NDArray> nd::zeros(Variant shape, nd::DType dtype) {
	return _full(shape, 0, dtype);
}

Ref<NDArray> nd::ones(Variant shape, nd::DType dtype) {
	return _full(shape, 1, dtype);
}

// The first parameter is the one used by the xarray operation, while the second is used for type deduction.
// It's ok if they're the same.
template <typename FX, typename PromotionRule>
inline Ref<NDArray> binary_operation(Variant a, Variant b) {
	std::shared_ptr<xtv::XTVariant> a_;
	if (!variant_as_array(a, a_)) {
		return Ref<NDArray>();
	}
	std::shared_ptr<xtv::XTVariant> b_;
	if (!variant_as_array(b, b_)) {
		return Ref<NDArray>();
	}

	try {
		auto result = xtv::xoperation<PromotionRule>(XFunction<FX> {}, *a_, *b_);
		return Ref<NDArray>(memnew(NDArray(result)));
	}
	catch (std::runtime_error error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}

Ref<NDArray> nd::add(Variant a, Variant b) {
	// godot::UtilityFunctions::print(value);
	return binary_operation<xt::detail::plus, xtv::promote::function_result<xt::detail::plus>>(a, b);
}

Ref<NDArray> nd::subtract(Variant a, Variant b) {
	return binary_operation<xt::detail::minus, xtv::promote::function_result<xt::detail::minus>>(a, b);
}

Ref<NDArray> nd::multiply(Variant a, Variant b) {
	return binary_operation<xt::detail::multiplies, xtv::promote::function_result<xt::detail::multiplies>>(a, b);
}

Ref<NDArray> nd::divide(Variant a, Variant b) {
	return binary_operation<xt::detail::divides, xtv::promote::function_result<xt::detail::divides>>(a, b);
}

Ref<NDArray> nd::remainder(Variant a, Variant b) {
	return binary_operation<xt::math::remainder_fun, xtv::promote::function_result<xt::math::remainder_fun>>(a, b);
}

Ref<NDArray> nd::pow(Variant a, Variant b) {
	return binary_operation<xt::math::pow_fun, xtv::promote::function_result<xt::math::pow_fun>>(a, b);
}


template <typename FX, typename PromotionRule>
inline Variant unary_operation(Variant a) {
	std::shared_ptr<xtv::XTVariant> a_;
	if (!variant_as_array(a, a_)) {
		return Ref<NDArray>();
	}

	try {
		auto result = xtv::xoperation<PromotionRule>(XFunction<FX> {}, *a_);
		return Ref<NDArray>(memnew(NDArray(result)));
	}
	catch (std::runtime_error error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}

Ref<NDArray> nd::abs(Variant a) {
	return unary_operation<xt::math::abs_fun, xtv::promote::function_result<xt::math::abs_fun>>(a);
}

Ref<NDArray> nd::sqrt(Variant a) {
	return unary_operation<xt::math::sqrt_fun, xtv::promote::function_result<xt::math::sqrt_fun>>(a);
}

Ref<NDArray> nd::exp(Variant a) {
	return unary_operation<xt::math::exp_fun, xtv::promote::function_result<xt::math::exp_fun>>(a);
}

Ref<NDArray> nd::log(Variant a) {
	return unary_operation<xt::math::log_fun, xtv::promote::function_result<xt::math::log_fun>>(a);
}

Ref<NDArray> nd::sin(Variant a) {
	return unary_operation<xt::math::sin_fun, xtv::promote::function_result<xt::math::sin_fun>>(a);
}

Ref<NDArray> nd::cos(Variant a) {
	return unary_operation<xt::math::cos_fun, xtv::promote::function_result<xt::math::cos_fun>>(a);
}

Ref<NDArray> nd::tan(Variant a) {
	return unary_operation<xt::math::tan_fun, xtv::promote::function_result<xt::math::tan_fun>>(a);
}

Ref<NDArray> nd::mean(Variant a, Variant axes) {
	try {
		auto axes_ = variant_to_axes(axes);

		std::shared_ptr<xtv::XTVariant> a_;
		if (!variant_as_array(a, a_)) {
			return Ref<NDArray>();
		}

		auto result = std::visit([a_](auto&& axes) {
			using Axes = std::decay_t<decltype(axes)>;
			if constexpr (std::is_same_v<Axes, nullptr_t>) {
				return xtv::xoperation<xtv::promote::matching_float_or_default<double_t>>([](auto&& a) {
					using T = decltype(a);
					return xt::mean(std::forward<T>(a));
				}, *a_);
			}
			else {
				return xtv::xoperation<xtv::promote::matching_float_or_default<double_t>>([axes](auto&& a) {
					using T = decltype(a);
					return xt::mean(std::forward<T>(a), axes);
				}, *a_);
			}
		}, axes_);

		return Ref<NDArray>(memnew(NDArray(result)));
	}
	catch (std::runtime_error error) {
		ERR_FAIL_V_MSG(Ref<NDArray>(), error.what());
	}
}
