#pragma once
namespace DIContainer {

    class IService
    {
    public:
        virtual ~IService() = default;

        virtual const std::type_info &typeInfo() const = 0;
        virtual bool operator==(const IService &key) const = 0;

        virtual size_t hash_code() const = 0;
    };

}

