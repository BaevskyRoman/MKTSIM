#pragma once

#include <cstdint>


namespace Model {

#pragma pack(push, 1)
struct Event {
    Event(uint32_t A, uint32_t B) : indexA(A), indexB(B) {};

    uint32_t indexA;
    uint32_t indexB;
};


struct EventSD : Event {
    EventSD(uint32_t A, uint32_t B) : Event(A, B) { };
};


struct EventDD : Event {
    EventDD(uint32_t A, uint32_t B) : Event(A, B) { };
};


struct EventSM : Event {
    EventSM(uint32_t A, uint32_t B) : Event(A, B) { };
};


struct EventDM : Event {
    EventDM(uint32_t A, uint32_t B) : Event(A, B) { };
};


struct EventMM : Event {
    EventMM(uint32_t A, uint32_t B) : Event(A, B) { };
};

#pragma pack(pop)

}