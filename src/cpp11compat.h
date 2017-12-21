//
// Created by Shae Bolt on 12/20/2017.
//

#ifndef ALURE_C11COMPAT_H
#define ALURE_C11COMPAT_H

#if __cplusplus >= 201703L
#include <variant>
#else

#include "mpark/variant.hpp"

namespace std {
    using mpark::variant;
    using mpark::monostate;
    using mpark::get;
    using mpark::get_if;
    using mpark::holds_alternative;
} // namespace std
#endif

#endif //ALURE_C11COMPAT_H
