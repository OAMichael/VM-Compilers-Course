#ifndef VALUE_H
#define VALUE_H

#include <cstdint>
#include <type_traits>
#include <variant>
#include <string>
#include <optional>
#include <set>

namespace VMIR {

enum class ValueType {
    Void = 0,
    Int8,
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

static inline const char *ValueTypeToIdStr(const ValueType vt) {
    switch (vt) {
        default:
        case ValueType::Unknown:    return "vt-unknown?";
        case ValueType::Void:       return "void";
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
constexpr bool is_same_no_cvref_v() {
    return std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
}


template <typename T>
constexpr ValueType TypeToValueType() {
    if constexpr (is_same_no_cvref_v<T, int8_t>())     return ValueType::Int8;
    if constexpr (is_same_no_cvref_v<T, int16_t>())    return ValueType::Int16;
    if constexpr (is_same_no_cvref_v<T, int32_t>())    return ValueType::Int32;
    if constexpr (is_same_no_cvref_v<T, int64_t>())    return ValueType::Int64;
    if constexpr (is_same_no_cvref_v<T, uint8_t>())    return ValueType::Uint8;
    if constexpr (is_same_no_cvref_v<T, uint16_t>())   return ValueType::Uint16;
    if constexpr (is_same_no_cvref_v<T, uint32_t>())   return ValueType::Uint32;
    if constexpr (is_same_no_cvref_v<T, uint64_t>())   return ValueType::Uint64;
    if constexpr (is_same_no_cvref_v<T, float>())      return ValueType::Float32;
    if constexpr (is_same_no_cvref_v<T, double>())     return ValueType::Float64;
    if constexpr (std::is_pointer_v<T>)                return ValueType::Pointer;
    return ValueType::Unknown;
}


constexpr uint64_t kInstructionLiveDiffSpillFill = 2;

struct LiveRange {
    uint64_t start;
    uint64_t end;

    void UniteWith(const LiveRange& other) {
        // Ignore potential live holes
        if (start == end) {
            start = other.start;
            end = other.end;
        }
        else {
            start = std::min(start, other.start);
            end = std::max(end, other.end);
        }
    }

    inline bool IsValid() const {
        if (end <= start || end - start < kInstructionLiveDiffSpillFill) {
            return false;
        }
        return true;
    }

    inline bool operator==(const LiveRange& other) const {
        return start == other.start && end == other.end;
    }
};

// These two are pretty similar
using LiveInterval = LiveRange;

using ValueId = int64_t;
using InstructionId = int64_t;
using BasicBlockId = int64_t;

// General Purpose Register
struct GPRegisterLocation {
    uint32_t registerId;

    bool operator==(const GPRegisterLocation&) const = default;
    bool operator!=(const GPRegisterLocation&) const = default;
};

// Floating Point Register
struct FPRegisterLocation {
    uint32_t registerId;

    bool operator==(const FPRegisterLocation&) const = default;
    bool operator!=(const FPRegisterLocation&) const = default;
};

struct StackLocation {
    uint32_t stackLocationId;

    bool operator==(const StackLocation&) const = default;
    bool operator!=(const StackLocation&) const = default;
};

using Location = std::variant<GPRegisterLocation, FPRegisterLocation, StackLocation>;

class Instruction;

class Value {
public:
    // Constructors
    Value(const ValueId id = -1) : mId{id} {};

    Value(const Value& other) : mValueType{other.mValueType}, mValue{other.mValue}, mId{other.mId} {};

    Value(Value&& other) noexcept : mValueType{std::move(other.mValueType)}, mValue{std::move(other.mValue)}, mId{std::move(other.mId)} {};

    explicit Value(const ValueType vt, const ValueId id) : mValueType{vt}, mId{id} {};

    template <typename T>
    requires NumericType<T>
    explicit Value(const ValueId id, const T value) : mValueType{TypeToValueType<T>()}, mValue{value}, mId{id} {};

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

    inline bool IsIntegralValueType() const {
        switch (GetValueType()) {
            default:                    return false;
            case ValueType::Int8:
            case ValueType::Int16:
            case ValueType::Int32:
            case ValueType::Int64:
            case ValueType::Uint8:
            case ValueType::Uint16:
            case ValueType::Uint32:
            case ValueType::Uint64:     return true;
        }
    }

    inline bool IsFloatingPointValueType() const {
        switch (GetValueType()) {
            default:                    return false;
            case ValueType::Float32:
            case ValueType::Float64:    return true;
        }
    }

    inline bool HasValue() const { return mValue.has_value(); }

    template <typename T>
    requires NumericType<T>
    inline std::optional<T> GetValue() const {
        if (HasValue()) {
            return std::get<std::remove_cv_t<T>>(*mValue);
        }
        return std::nullopt;
    } 

    inline ValueId GetId() const { return mId; }

    inline void AddUser(Instruction* u)                   { mUsers.insert(u); }
    inline bool HasUser(Instruction* u) const             { return mUsers.contains(u); }
    inline void RemoveUser(Instruction* u)                { mUsers.erase(u); }
    inline const std::set<Instruction*>& GetUsers() const { return mUsers; }

    inline Instruction* GetProducer() const { return mProducer; }
    inline void SetProducer(Instruction* prod) { mProducer = prod; }

    inline LiveInterval& GetLiveInterval() { return mLiveInterval; }
    inline const LiveInterval& GetLiveInterval() const { return mLiveInterval; }

    inline Location GetLocation() const { return mLocation; }
    inline void SetLocation(Location loc) { mLocation = loc; }

    inline std::string GetValueStr() const {
        if (HasValue()) {
            switch (GetValueType()) {
                default:
                case ValueType::Pointer:    break;
                case ValueType::Int8:       return std::to_string(GetValue<int8_t>().value());
                case ValueType::Int16:      return std::to_string(GetValue<int16_t>().value());
                case ValueType::Int32:      return std::to_string(GetValue<int32_t>().value());
                case ValueType::Int64:      return std::to_string(GetValue<int64_t>().value());
                case ValueType::Uint8:      return std::to_string(GetValue<uint8_t>().value());
                case ValueType::Uint16:     return std::to_string(GetValue<uint16_t>().value());
                case ValueType::Uint32:     return std::to_string(GetValue<uint32_t>().value());
                case ValueType::Uint64:     return std::to_string(GetValue<uint64_t>().value());
                case ValueType::Float32:    return std::to_string(GetValue<float>().value());
                case ValueType::Float64:    return std::to_string(GetValue<double>().value());
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

    std::set<Instruction*> mUsers{};
    Instruction* mProducer{};
    LiveInterval mLiveInterval{};
    Location mLocation{};
};


static inline bool IsValueZero(Value* value) {
    if (!value || !value->HasValue()) {
        return false;
    }

    switch (value->GetValueType()) {
        default:                    return false;
        case ValueType::Int8:       return value->GetValue<int8_t>()   == int8_t(0);
        case ValueType::Int16:      return value->GetValue<int16_t>()  == int16_t(0);
        case ValueType::Int32:      return value->GetValue<int32_t>()  == int32_t(0);
        case ValueType::Int64:      return value->GetValue<int64_t>()  == int64_t(0);
        case ValueType::Uint8:      return value->GetValue<uint8_t>()  == uint8_t(0);
        case ValueType::Uint16:     return value->GetValue<uint16_t>() == uint16_t(0);
        case ValueType::Uint32:     return value->GetValue<uint32_t>() == uint32_t(0);
        case ValueType::Uint64:     return value->GetValue<uint64_t>() == uint64_t(0);
        case ValueType::Float32:    return value->GetValue<float>()    == float(0.0f);
        case ValueType::Float64:    return value->GetValue<double>()   == double(0.0);
    }
}

}   // namespace VMIR

#endif  // VALUE_H
