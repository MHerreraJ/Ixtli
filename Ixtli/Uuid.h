#ifndef IXTLI_UUID_H
#define IXTLI_UUID_H

#include <iostream>

namespace Ixtli{

using UUID_t = unsigned int;

class UUID{
    public:
        enum Source{
            AUTO,
            RANDOM,
            TIME
        };
        static constexpr UUID_t UUID_NONE = 0;
    private:
        UUID_t id;
    public:
        UUID(): id(UUID_NONE){};
        UUID(Source source);

        inline UUID_t get() const 
            { return id; }
        
        inline bool isValid() const 
            { return id != UUID_NONE; }

        inline bool operator == (const UUID& uuid) const 
            { return id == uuid.id; }
        
        inline bool operator == (UUID_t uuid) const
            { return id == uuid; }

        inline bool operator != (UUID_t uuid) const
            { return id != uuid; }

        
        inline bool operator != (const UUID& uuid) const
            { return id != uuid.id; }
        
        inline static UUID create(Source s = AUTO)
            { return UUID(s); }
}; 

inline std::ostream& operator << (std::ostream& os, const UUID& uuid){
    os << uuid.get();
    return os;
}

}

template<> struct std::hash<Ixtli::UUID>{
    inline std::size_t operator()(Ixtli::UUID const& id) const noexcept{
        return std::hash<Ixtli::UUID_t>{}(id.get());
    }
};

#endif //IXTLI_UUID_H