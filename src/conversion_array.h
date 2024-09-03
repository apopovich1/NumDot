#ifndef NUMDOT_AS_ARRAY_H
#define NUMDOT_AS_ARRAY_H

#include <godot_cpp/godot.hpp>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"

#include "ndarray.h"
#include "xtv.h"

using namespace godot;

template <typename C, typename T>
inline bool packed_as_array(const T shape_array, std::shared_ptr<xtv::XTVariant> &target) {
	uint64_t size = shape_array.size();

	xt::static_shape<std::size_t, 1> shape_of_shape = { size };

	target = std::make_shared<xtv::XTVariant>(
		xt::xarray<C>(xt::adapt(shape_array.ptr(), size, xt::no_ownership(), shape_of_shape))
	);
	return true;
}

static bool variant_as_array(const Variant array, std::shared_ptr<xtv::XTVariant> &target) {
	auto type = array.get_type();

	// TODO A bunch of interesting types are still missing
	switch (type) {
		case Variant::OBJECT:
			if (auto ndarray = Object::cast_to<NDArray>(array)) {
				target = ndarray->array;
				return true;
			}
			break;
		case Variant::INT:
			target = std::make_shared<xtv::XTVariant>(xt::xarray<int64_t>(int64_t(array)));
			return true;
		case Variant::FLOAT:
			target = std::make_shared<xtv::XTVariant>(xt::xarray<double_t>(double_t(array)));
			return true;
		case Variant::PACKED_BYTE_ARRAY:
			return packed_as_array<uint8_t>(PackedByteArray(array), target);
		case Variant::PACKED_INT32_ARRAY:
			return packed_as_array<int32_t>(PackedInt32Array(array), target);
		case Variant::PACKED_INT64_ARRAY:
			return packed_as_array<int64_t>(PackedInt64Array(array), target);
		case Variant::PACKED_FLOAT32_ARRAY:
			return packed_as_array<float_t>(PackedFloat32Array(array), target);
		case Variant::PACKED_FLOAT64_ARRAY:
			return packed_as_array<double_t>(PackedFloat64Array(array), target);
		case Variant::VECTOR2I: {
			auto vector = Vector2i(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<int64_t>(
				{ int64_t(vector.x), int64_t(vector.y) }
			));
			return true;
		}
		case Variant::VECTOR3I: {
			auto vector = Vector3i(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<int64_t>(
				{ int64_t(vector.x), int64_t(vector.y), int64_t(vector.z) }
			));
			return true;
		}
		case Variant::VECTOR4I: {
			auto vector = Vector4i(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<int64_t>(
				{ int64_t(vector.x), int64_t(vector.y), int64_t(vector.z), int64_t(vector.w) }
			));
			return true;
		}
		case Variant::VECTOR2: {
			auto vector = Vector2(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<double_t>(
				{ double_t(vector.x), double_t(vector.y) }
			));
			return true;
		}
		case Variant::VECTOR3: {
			auto vector = Vector3(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<double_t>(
				{ double_t(vector.x), double_t(vector.y), double_t(vector.z) }
			));
			return true;
		}
		case Variant::VECTOR4: {
			auto vector = Vector4(array);
			target = std::make_shared<xtv::XTVariant>(xt::xarray<double_t>(
				{ double_t(vector.x), double_t(vector.y), double_t(vector.z), double_t(vector.w) }
			));
			return true;
		}

		default:
			break;
	}

	// Try float first. Int may be more lossy.
	if (Variant::can_convert(type, Variant::Type::FLOAT)) {
		target = std::make_shared<xtv::XTVariant>(xt::xarray<double_t>(double_t(array)));
		return true;
	}
	if (Variant::can_convert(type, Variant::Type::INT)) {
		target = std::make_shared<xtv::XTVariant>(xt::xarray<int64_t>(int64_t(array)));
		return true;
	}

	// TODO Godot will happily convert every number to float.
	// We should manually adapt and look through Array to find what its parts are.
	if (Variant::can_convert(type, Variant::Type::PACKED_FLOAT64_ARRAY)) {
		return packed_as_array<double_t>(PackedFloat64Array(array), target);
	}
	if (Variant::can_convert(type, Variant::Type::PACKED_INT64_ARRAY)) {
		return packed_as_array<int64_t>(PackedInt64Array(array), target);
	}

	ERR_FAIL_V_MSG(false, "Variant cannot be converted to an array.");
}

template <typename P>
P xtvariant_to_packed(xtv::XTVariant& array) {
	P p_array = P();

	std::visit([&p_array](auto array){
		p_array.resize(array.size());
		std::copy(array.begin(), array.end(), p_array.ptrw());
	}, array);

	return p_array;
}

Array xtvariant_to_godot_array(xtv::XTVariant& array) {
	Array godot_array = Array();

	std::visit([&godot_array](auto array){
		godot_array.resize(array.size());
		auto start = array.begin();

		for (size_t i = 0; i < array.size(); ++i) {
        	godot_array[i] = *(start + i);
		}
	}, array);

	return godot_array;
}

#endif
