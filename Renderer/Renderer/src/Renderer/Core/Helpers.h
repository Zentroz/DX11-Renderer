#pragma once

#define FLAG_ENUM_OVERLOADS(type)                                   \
constexpr type operator|(type a, type b) {                          \
    using U = std::underlying_type_t<type>;                         \
    return static_cast<type>(static_cast<U>(a) | static_cast<U>(b));\
}                                                                   \
constexpr type operator&(type a, type b) {                          \
    using U = std::underlying_type_t<type>;                         \
    return static_cast<type>(static_cast<U>(a) & static_cast<U>(b));\
}                                                                   \
inline type& operator|=(type& a, type b) {                          \
    return a = a | b;                                                \
}                                                                   \
inline type& operator&=(type& a, type b) {                          \
    return a = a & b;                                                \
}

template <typename E>
bool HasFlag(E value, E flag) {
    using U = std::underlying_type_t<E>;
    return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
}