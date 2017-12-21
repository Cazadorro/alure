#ifndef ALURE_MAIN_H
#define ALURE_MAIN_H

#include "alure2.h"
#include "cpp11compat.h"

namespace alure {

// Need to use these to avoid extraneous commas in macro parameter lists
using Vector3Pair = std::pair<Vector3,Vector3>;
using UInt64NSecPair = std::pair<uint64_t,std::chrono::nanoseconds>;
using SecondsPair = std::pair<Seconds,Seconds>;
using ALfloatPair = std::pair<ALfloat,ALfloat>;
using ALuintPair = std::pair<ALuint,ALuint>;
using BoolTriple = std::tuple<bool,bool,bool>;


template<typename T>
inline std::future_status GetFutureState(const SharedFuture<T> &future)
{ return future.wait_for(std::chrono::seconds::zero()); }

// This variant is a poor man's optional
std::variant<std::monostate,uint64_t> ParseTimeval(StringView strval, double srate) noexcept;

} // namespace alure

#endif /* ALURE_MAIN_H */
