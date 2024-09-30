#ifndef VALUE_H
#define VALUE_H

#include <cstdint>
#include <type_traits>
#include <variant>
#include <string>
#include <optional>

namespace VMIR {

enum class ValueType {
    Int8 = 0,
    Int16,
    Int32,
    Int64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Float32,
    Float64,
    Pointer,

    Unknown
};

static inline const char *ValueTypeToStr(const ValueType vt) {
    switch (vt) {
        default:
        case ValueType::Unknown:    return "Unknown";
        case ValueType::Int8:       return "Int8";
        case ValueType::Int16:      return "Int16";
        case ValueType::Int32:      return "Int32";
        case ValueType::Int64:      return "Int64";
        case ValueType::Uint8:      return "Uint8";
        case ValueType::Uint16:     return "Uint16";
        case ValueType::Uint32:     return "Uint32";
        case ValueType::Uint64:     return "Uint64";
        case ValueType::Float32:    return "Float32";
        case ValueType::Float64:    return "Float64";
        case ValueType::Pointer:    return "Pointer";
    }
}

static inline const char *ValueTypeToIdStr(const ValueType vt) {
    switch (vt) {
        default:
        case ValueType::Unknown:    return "vt-unknown?";
        case ValueType::Int8:       return "i8";
        case ValueType::Int16:      return "i16";
        case ValueType::Int32:      return "i32";
        case ValueType::Int64:      return "i64";
        case ValueType::Uint8:      return "ui8";
        case ValueType::Uint16:     return "ui16";
        case ValueType::Uint32:     return "ui32";
        case ValueType::Uint64:     return "ui64";
        case ValueType::Float32:    return "f32";
        case ValueType::Float64:    return "f64";
        case ValueType::Pointer:    return "ptr";
    }
}


template <typename T>
concept NumericType = requires(T n) {
    requires std::is_integral_v<T> || std::is_floating_point_v<T>;
};


template <typename T, typename U>
constexpr bool is_same_no_cv_v() {
    return std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;
}


template <typename T>
constexpr ValueType TypeToValueType() {
    if constexpr (is_same_no_cv_v<T, int8_t>())     return ValueType::Int8;
    if constexpr (is_same_no_cv_v<T, int16_t>())    return ValueType::Int16;
    if constexpr (is_same_no_cv_v<T, int32_t>())    return ValueType::Int32;
    if constexpr (is_same_no_cv_v<T, int64_t>())    return ValueType::Int64;
    if constexpr (is_same_no_cv_v<T, uint8_t>())    return ValueType::Uint8;
    if constexpr (is_same_no_cv_v<T, uint16_t>())   return ValueType::Uint16;
    if constexpr (is_same_no_cv_v<T, uint32_t>())   return ValueType::Uint32;
    if constexpr (is_same_no_cv_v<T, uint64_t>())   return ValueType::Uint64;
    if constexpr (is_same_no_cv_v<T, float>())      return ValueType::Float32;
    if constexpr (is_same_no_cv_v<T, double>())     return ValueType::Float64;
    if constexpr (std::is_pointer_v<T>)             return ValueType::Pointer;
    return ValueType::Unknown;
}


using ValueId = int64_t;

class Value {
public:
    // Constructors
    Value(const ValueId id = -1) : mId{id} {};

    Value(const Value& other) : mValueType{other.mValueType}, mValue{other.mValue}, mId{other.mId} {};

    Value(Value&& other) noexcept : mValueType{std::move(other.mValueType)}, mValue{std::move(other.mValue)}, mId{std::move(other.mId)} {};

    explicit Value(const ValueType vt, const ValueId id) : mValueType{vt}, mId{id} {};

    template <typename T>
    requires NumericType<T>
    explicit Value(const ValueType vt, const ValueId id, const T value) : mValueType{vt}, mValue{value}, mId{id} {};

    // Operators
    Value& operator=(const Value& other) {
        if (this != &other) { 
            mValueType = other.mValueType;
            mValue = other.mValue;
            mId = other.mId;
        }
        return *this;
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            mValueType = std::move(other.mValueType);
            mValue = std::move(other.mValue);
            mId = std::move(other.mId);
        }
        return *this;
    }

    bool operator==(const Value& other) {
        return mValueType == other.mValueType
            && mValue == other.mValue
            && mId == other.mId;
    }

    ~Value() {};

    // Getters. Value class is deliberately immutable, so no setters
    inline ValueType GetValueType() const { return mValueType; } 

    inline bool HasValue() const { return mValue.has_value(); }

    template <typename T>
    requires NumericType<T>
    inline T GetValue() const {
        if (HasValue()) {
            return std::get<std::remove_cv_t<T>>(*mValue);
        }
        return T();
    } 

    inline ValueId GetId() const { return mId; }

    inline std::string GetValueStr() const {
        if (HasValue()) {
            switch (GetValueType()) {
                default:
                case ValueType::Pointer:    break;
                case ValueType::Int8:       return std::to_string(GetValue<int8_t>());
                case ValueType::Int16:      return std::to_string(GetValue<int16_t>());
                case ValueType::Int32:      return std::to_string(GetValue<int32_t>());
                case ValueType::Int64:      return std::to_string(GetValue<int64_t>());
                case ValueType::Uint8:      return std::to_string(GetValue<uint8_t>());
                case ValueType::Uint16:     return std::to_string(GetValue<uint16_t>());
                case ValueType::Uint32:     return std::to_string(GetValue<uint32_t>());
                case ValueType::Uint64:     return std::to_string(GetValue<uint64_t>());
                case ValueType::Float32:    return std::to_string(GetValue<float>());
                case ValueType::Float64:    return std::to_string(GetValue<double>());
            }
        }
        return "v" + std::to_string(GetId());
    }

    inline bool IsValid() const { return mId != -1 && mValueType != ValueType::Unknown; }

private:
    ValueType mValueType = ValueType::Unknown;

    std::optional<std::variant<int8_t,  int16_t,  int32_t,  int64_t,
                  uint8_t, uint16_t, uint32_t, uint64_t, float, double>> mValue{};

    ValueId mId{-1};
};

}   // namespace VMIR

#endif  // VALUE_H
