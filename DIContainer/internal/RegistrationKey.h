#pragma once

#include <typeinfo>
#include "IService.h"

namespace DIContainer
{
    class RegistrationInfo;

 





    class RegistrationKey
    {
    public:
        explicit RegistrationKey(std::shared_ptr<IService> info)
            : infoPtr(info), info(*info) {}

        static RegistrationKey forLookup(const IService &info)
        {
            return RegistrationKey(info);
        }

        bool operator==(const RegistrationKey &key) const { return info == key.info; }

        size_t hash_code() const
        {
            return info.hash_code();
        }

    private:
        RegistrationKey operator=(const RegistrationKey &) = delete;

        explicit RegistrationKey(const IService &info)
            : info(info) {}

        std::shared_ptr<IService> infoPtr;
        const IService &info;
    };

}

namespace std
{
    template <>
    struct hash < DIContainer::RegistrationKey >
    {
        std::size_t operator()(const DIContainer::RegistrationKey& k) const
        {
            return k.hash_code();
        }
    };
}