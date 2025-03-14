#pragma once

#include <list>
#include <iostream>
#include <string>


class SQL
{
public:
    // SQL REFERENCES
    template <typename T>
    struct ref {
        std::size_t id;

        ref& operator=(std::size_t num)
        {
            id = num;
            return *this;
        }

        // operator std::string() const{
        //     return std::to_string(id);
        // }
    };


    // SQL UNIQUE
    template <typename T>
    struct unique {
        T value;

        unique& operator=(T t)
        {
            value = t;
            return *this;
        }
        // operator std::string() const{
        //     return std::to_string(id);
        // }
    };

    // SQL NOT NULL
    template <typename T>
    struct notnull {
        T value;
        notnull& operator=(T t)
        {
            value = t;
            return *this;
        }
    };

    // Strutct of Ref
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


private:

    // Get type
    template <typename T>
    struct get_type_struct { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const ref<T>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const unique<T>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const notnull<T>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const unique<ref<T>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const notnull<ref<T>>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const ref<unique<T>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const notnull<unique<T>>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const ref<notnull<T>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const unique<notnull<T>>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const notnull<unique<ref<T>>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const unique<notnull<ref<T>>>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const ref<notnull<unique<T>>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const notnull<ref<unique<T>>>&> { static constexpr T value; };

    template <typename T>
    struct get_type_struct<const unique<ref<notnull<T>>>&> { static constexpr T value; };
    template <typename T>
    struct get_type_struct<const ref<unique<notnull<T>>>&> { static constexpr T value; };

public:

    template <typename T>
    using getType = decltype(SQL::get_type_struct<T>::value);

};


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
std::string to_string(const SQL::notnull<T>& obj)
{
    return std::to_string(obj.value);
}


template <typename T>
std::string to_string(const SQL::unique<T>& obj)
{
    return std::to_string(obj.value);
}
