#pragma once

#include <typeinfo>

namespace DIContainer
{
    class RegistrationInfo;

    class IServiceTyper
    {
    public:
        virtual ~IServiceTyper() = default;

        virtual const std::type_info &typeInfo() const = 0;
        virtual bool operator==(const IServiceTyper &key) const = 0;

        virtual size_t hash_code() const = 0;
    };

    template<class T>
    class TypedService : public IServiceTyper
    {
    public:
        virtual const std::type_info &typeInfo() const override { return typeid(T); }

        virtual bool operator==(const IServiceTyper &key) const override
        {     
            return dynamic_cast<const TypedService<T>*>(&key) != nullptr;
        };

        virtual size_t hash_code() const override
        {
            return typeInfo().hash_code();
        }

    };

    template<class T, class KeyType>
    class KeyedService : public IServiceTyper
    {
    public:
        explicit KeyedService(const KeyType &key)
            : key(key) {}

        virtual const std::type_info &typeInfo() const override { return typeid(T); }

        virtual bool operator==(const IServiceTyper &info) const override
        {
            auto keyed = dynamic_cast<const KeyedService<T, KeyType>*>(&info);
            if( keyed == nullptr)
                return false;
            return key == keyed->key;
        };

        virtual size_t hash_code() const override
        {
            auto typeHash = typeInfo().hash_code();
            auto keyTypeHash = typeid(KeyType).hash_code();
            auto keyValueHash = std::hash<KeyType>()(key);
            
            return ((typeHash
                ^ (keyTypeHash << 1)) >> 1)
                ^ (keyValueHash << 1);
        }


    private:

        KeyType key;
    };


    class RegistrationKey
    {
    public:
        explicit RegistrationKey(std::shared_ptr<IServiceTyper> info)
            : infoPtr(info), info(*info) {}

        static RegistrationKey forLookup(const IServiceTyper &info)
        {
            return RegistrationKey(info);
        }

        bool operator==(const RegistrationKey &key) const { return info == key.info; }

        size_t hash_code() const
        {
            return info.hash_code();
        }

    private:
        explicit RegistrationKey(const IServiceTyper &info)
            : info(info) {}

        std::shared_ptr<IServiceTyper> infoPtr;
        const IServiceTyper &info;
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