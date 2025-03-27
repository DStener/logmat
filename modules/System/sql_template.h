#pragma once

#include "System/utils.h"

#include <list>
#include <iostream>
#include <string>
#include <json/json.h>


#define ASSIGNING(RetType, InType, field_name) \
RetType& operator=(InType t) \
{ \
    field_name = t; \
    return *this; \
} \
RetType& operator+=(InType t) \
{ \
        field_name += t; \
        return *this; \
} \
RetType& operator-=(InType t) \
{ \
        field_name -= t; \
        return *this; \
}

#define COMPARE(InType, field_name) \
bool operator==(InType t) { return field_name == t; } \
bool operator!=(InType t) { return field_name != t; } \
bool operator>(InType t) { return field_name > t; } \
bool operator<(InType t) { return field_name < t; }

class SQL
{
public:

    // SQL REFERENCES
    template <typename T>
    struct ref {
        id_t id;
        ASSIGNING(ref, id_t, id)
        COMPARE(id_t, id)
    };

    // SQL UNIQUE
    template <typename T>
    struct unique {
        T value;
        ASSIGNING(unique, T, value)
        COMPARE(T, value)
    };

    // SQL NOT NULL
    template <typename T>
    struct notnull {
        T value;
        ASSIGNING(notnull, T, value)
        COMPARE(T, value)
    };

    // Strutct of Ref
    // template <typename T>
    // struct isRef { static constexpr bool value = false; };
    // template <typename T>
    // struct isRef<ref<T>&> { static constexpr bool value = true; };
    // template <typename T>
    // struct isRef<unique<ref<T>>&> { static constexpr bool value = true; };
    // template <typename T>
    // struct isRef<notnull<ref<T>>&> { static constexpr bool value = true; };
    // template <typename T>
    // struct isRef<unique<notnull<ref<T>>>&> { static constexpr bool value = true; };
    // template <typename T>
    // struct isRef<notnull<unique<ref<T>>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isRef { static constexpr bool value = false; };
    template <typename T>
    struct isRef<const ref<T>&> { static constexpr bool value = true; };
    template <typename T>
    struct isRef<const unique<ref<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isRef<const notnull<ref<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isRef<const unique<notnull<ref<T>>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isRef<const notnull<unique<ref<T>>>&> { static constexpr bool value = true; };

    // Struct of Unique
    template <typename T>
    struct isUnique { static constexpr bool value = false; };
    template <typename T>
    struct isUnique<const unique<T>&> { static constexpr bool value = true; };
    template <typename T>
    struct isUnique<const ref<unique<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isUnique<const notnull<unique<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isUnique<const ref<notnull<unique<T>>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isUnique<const notnull<ref<unique<T>>>&> { static constexpr bool value = true; };

    template <typename T>
    struct isNotNull { static constexpr bool value = false; };
    template <typename T>
    struct isNotNull<const notnull<T>&> { static constexpr bool value = true; };
    template <typename T>
    struct isNotNull<const ref<notnull<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isNotNull<const unique<notnull<T>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isNotNull<const ref<unique<notnull<T>>>&> { static constexpr bool value = true; };
    template <typename T>
    struct isNotNull<const unique<ref<notnull<T>>>&> { static constexpr bool value = true; };

    template <typename T>
    static auto& REMOVE_ATTRIB(T& t)
    {
        if constexpr (SQL::isRef<const std::remove_cvref_t<T>&>::value)
        {
            return *reinterpret_cast<SQL::ref<size_t>*>(&t);
        }
        else
        {
            // *reinterpret_cast<type_dec*>(&field)
            return *reinterpret_cast<std::remove_cvref_t<SQL::getType<T>>*>(&t);
        }
    }


private:

    // Get type
    template <class T>
    struct get_type_struct { static constexpr T value; };

    template <typename T>
    struct get_type_struct<ref<T>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<unique<T>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<notnull<T>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<unique<ref<T>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<notnull<ref<T>>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<ref<unique<T>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<notnull<unique<T>>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<ref<notnull<T>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<unique<notnull<T>>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<notnull<unique<ref<T>>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<unique<notnull<ref<T>>>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<ref<notnull<unique<T>>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<notnull<ref<unique<T>>>> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<unique<ref<notnull<T>>>> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<ref<unique<notnull<T>>>> { static constexpr T value; };

public:

    template <typename T>
    using getType = decltype(SQL::get_type_struct<std::remove_cvref_t<T>>::value);

};


namespace SQL_ATTRIB
{
    template <typename T>
    using REFERENCES = SQL::ref<T>;
    template <typename T>
    using UNIQUE = SQL::unique<T>;
    template <typename T>
    using NOTNULL = SQL::notnull<T>;
}


template <typename T>
std::ostream& operator<<(std::ostream& os, const SQL::ref<T>& obj)
{
    os << obj.id;
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const SQL::unique<T>& obj)
{
    os << obj.value;
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const SQL::notnull<T>& obj)
{
    os << obj.value;
    return os;
}

template <typename T>
std::string to_string(const SQL::unique<T>& obj)
{
    return std::to_string(obj.value);
}


template <typename T>
std::string to_string(const SQL::notnull<T>& obj)
{
    return std::to_string(obj.value);
}


