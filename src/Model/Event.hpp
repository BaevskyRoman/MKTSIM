#pragma once

#include <cstdint>


namespace Model {

#pragma pack(push, 1)
struct Event {
    Event(uint32_t A, uint32_t B) : indexA(A), indexB(B) {};
    uint32_t indexA;
    uint32_t indexB;
};
#pragma pack(pop)

}