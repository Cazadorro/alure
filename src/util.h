//
// Created by Shae Bolt on 12/21/2017.
//

#ifndef ALURE_UTIL_H
#define ALURE_UTIL_H
#include "alure2.h"

namespace alure {

    template<typename T>
    inline std::future_status GetFutureState(const SharedFuture<T> &future) {
        return future.wait_for(std::chrono::seconds::zero());
    }
}
#endif //ALURE_UTIL_H
