#include <cmath>
namespace math {
    // Addition function
    template<typename T>
    void add(T& a, T& b, T& result)
    {
        result = a + b;
    }
    // Subtraction function
    template<typename T>
    void sub(T& a, T& b, T& result)
    {
        result = a - b;
    }
    // Product function
    template<typename T>
    void prod(T& a, T& b, T& result)
    {
        result = a * b;
    }
    // Quotient function
    template<typename T>
    void quot(T& a, T& b, T& result)
    {
        result = a / b;
    }
    // [V1] Ackermann function
    template<typename T>
    T A(T n, T m)
    {
        if (n == 0) return m + 1;                            // if n == 0
        if (n != 0 && m == 0) return math::A(n - 1, static_cast<T>(1));           // if n != 0 and m ==0
        if (n > 0 && m > 0)return math::A(n - 1, math::A(n, m - 1));   // if n > 0 and m > 0
    }
    template<typename T>
    T C(T n, T m)
    {
        if (m == n) return 1;
        if (m == 1) return n;
        return math::C(n - 1, m) + math::C(n - 1, m - 1);
    }
    // [V8] Decimel to Binary
    template<typename T>
    long long dec2bin(T&& a, int i = -1)
    {
        // Equivalent : floor(ln(a) / ln(2))
        if (i == -1) for (int b = a; b != 0; ++i, b >>= 1);
        int c = a - (1 << i);
        if (i == 0) return (c >= 0);
        return
            (c >= 0) * std::pow(10, i) +
            dec2bin(a - (1 << i) * (c >= 0), i - 1);
    }
}