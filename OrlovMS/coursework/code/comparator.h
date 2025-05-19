#pragma once


template<typename Element>
using Comparator = bool(*)(const Element &, const Element &);


template<typename Element>
bool is_greater(const Element &A, const Element &B)
{
    return A > B;
}


template<typename Element>
bool is_less(const Element &A, const Element &B)
{
    return A < B;
}
