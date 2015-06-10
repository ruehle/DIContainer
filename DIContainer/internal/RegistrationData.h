#pragma once

#include <memory>
#include <functional>

namespace DIContainer
{
    class Container;

    enum class LifetimeScope
    {
        PerDependency,
        SingleInstance
    };

    class RegistrationData
    {
    public:

        using UntypedFactory = std::function<std::shared_ptr<void>(Container &)>;

        explicit RegistrationData(std::size_t id,UntypedFactory factory)
            : _id(id), factory(factory) {}

        std::shared_ptr<void> build(Container &r)
        {
            if ( lifetime ==LifetimeScope::PerDependency )
            {
                return factory(r);
            }
            if (!instanceIfShared)
                instanceIfShared = factory(r);

            return instanceIfShared;
        };

        void setLifetimeScope( LifetimeScope lifetime )
        {
            this->lifetime = lifetime;
        }

        std::size_t id() { return _id; }

    private:
        LifetimeScope lifetime = LifetimeScope::PerDependency;
        std::shared_ptr<void> instanceIfShared;

        UntypedFactory factory;
        std::size_t _id;
    };
}
