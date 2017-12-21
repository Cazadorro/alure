//
// Created by Shae Bolt on 12/20/2017.
//

#ifndef ALURE_ALERROR_H
#define ALURE_ALERROR_H

#include "alure2.h"
#include "utilmacros.h"

#include <system_error>

namespace alure {

    class alc_category : public std::error_category {
        alc_category() noexcept {}

    public:
        static alc_category sSingleton;

        const char *name() const noexcept override final { return "alc_category"; }

        std::error_condition
        default_error_condition(int code) const noexcept override final { return std::error_condition(code, *this); }

        bool equivalent(int code, const std::error_condition &condition) const noexcept override final {
            return default_error_condition(code) == condition;
        }

        bool equivalent(const std::error_code &code, int condition) const noexcept override final {
            return *this == code.category() && code.value() == condition;
        }

        std::string message(int condition) const override final;
    };

    template<typename T>
    inline std::system_error alc_error(int code, T &&what) {
        return std::system_error(code, alc_category::sSingleton, std::forward<T>(what));
    }

    inline std::system_error alc_error(int code) { return std::system_error(code, alc_category::sSingleton); }

    class al_category : public std::error_category {
        al_category() noexcept {}

    public:
        static al_category sSingleton;

        const char *name() const noexcept override final { return "al_category"; }

        std::error_condition
        default_error_condition(int code) const noexcept override final { return std::error_condition(code, *this); }

        bool equivalent(int code, const std::error_condition &condition) const noexcept override final {
            return default_error_condition(code) == condition;
        }

        bool equivalent(const std::error_code &code, int condition) const noexcept override final {
            return *this == code.category() && code.value() == condition;
        }

        std::string message(int condition) const override final;
    };

    template<typename T>
    inline std::system_error al_error(int code, T &&what) {
        return std::system_error(code, al_category::sSingleton, std::forward<T>(what));
    }

    inline std::system_error al_error(int code) { return std::system_error(code, al_category::sSingleton); }

    inline void throw_al_error(const char *str) {
        ALenum err = alGetError();
        if (UNLIKELY(err != AL_NO_ERROR))
            throw al_error(err, str);
    }
};
#endif //ALURE_ALERROR_H
