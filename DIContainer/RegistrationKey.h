#pragma once

#include <typeinfo>

namespace DIContainer
{
    class RegistrationInfo;

    class IRegistrationInfo
    {
    public:
        virtual ~IRegistrationInfo() = default;

        virtual const std::type_info &typeInfo() const = 0;
        virtual bool operator==(const IRegistrationInfo &key) const = 0;
    };

    template<class T>
    class TypedRegistration : public IRegistrationInfo
    {
    public:
        virtual const std::type_info &typeInfo() const override { return typeid(T); }

        virtual bool operator==(const IRegistrationInfo &key) const override
        {     
            return typeInfo() == key.typeInfo();
            //const IRegistrationInfo *ptr = &key;
            //return dynamic_cast<TypedRegistration<T>>(ptr) != nullptr;
        };
    };

    class RegistrationKey
    {
    public:
        RegistrationKey(std::shared_ptr<IRegistrationInfo> info)
            : infoPtr(info), info(*info) {}

        RegistrationKey( const IRegistrationInfo &info )
            : info(info) {}

        bool operator==(const RegistrationKey &key) const { return info == key.info; }
        size_t hash_code() const
        {
            return info.typeInfo().hash_code();
        }

    private:
        std::shared_ptr<IRegistrationInfo> infoPtr;
        const IRegistrationInfo &info;
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

            //return ((hash<std::type_info>()(k.typeInfo());
                //^ (hash<string>()(k.second) << 1)) >> 1)
                //^ (hash<int>()(k.third) << 1);
        }
    };
}