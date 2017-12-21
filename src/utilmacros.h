//
// Created by Shae Bolt on 12/20/2017.
//

#ifndef ALURE_UTILMACROS_H
#define ALURE_UTILMACROS_H

#ifdef __GNUC__
#define LIKELY(x) __builtin_expect(static_cast<bool>(x), true)
#define UNLIKELY(x) __builtin_expect(static_cast<bool>(x), false)
#else
#define LIKELY(x) static_cast<bool>(x)
#define UNLIKELY(x) static_cast<bool>(x)
#endif

#define DECL_THUNK0(ret, C, Name, cv)                                         \
ret C::Name() cv { return pImpl->Name(); }
#define DECL_THUNK1(ret, C, Name, cv, T1)                                     \
ret C::Name(T1 a) cv { return pImpl->Name(std::forward<T1>(a)); }
#define DECL_THUNK2(ret, C, Name, cv, T1, T2)                                 \
ret C::Name(T1 a, T2 b) cv                                                    \
{ return pImpl->Name(std::forward<T1>(a), std::forward<T2>(b)); }
#define DECL_THUNK3(ret, C, Name, cv, T1, T2, T3)                             \
ret C::Name(T1 a, T2 b, T3 c) cv                                              \
{                                                                             \
    return pImpl->Name(std::forward<T1>(a), std::forward<T2>(b),              \
                       std::forward<T3>(c));                                  \
}

#endif //ALURE_UTILMACROS_H
