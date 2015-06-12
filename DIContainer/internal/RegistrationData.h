#pragma once

#include <memory>
#include <functional>
#include <mutex>

namespace DIContainer
{
    class IComponentContext;

    enum class LifetimeScope
    {
        PerDependency,
        SingleInstance
    };

    class RegistrationData
    {
    public:

        using UntypedFactory = std::function<std::shared_ptr<void>(IComponentContext &)>;

        explicit RegistrationData
            (UntypedFactory factory, 
            LifetimeScope lifetime = LifetimeScope::PerDependency)
            : factory(factory), lifetime(lifetime) {}

        RegistrationData(const RegistrationData &) = delete;

        std::shared_ptr<void> build(IComponentContext &r)
        {
            if ( lifetime ==LifetimeScope::PerDependency )
            {
                return factory(r);
            }
            //std::lock_guard<std::mutex> lock(sharedInstanceMutex);
            if (!instanceIfShared)
                instanceIfShared = factory(r);

            return instanceIfShared;
        };

        void setLifetimeScope( LifetimeScope lifetime )
        {
            this->lifetime = lifetime;
        }

        std::shared_ptr<RegistrationData> copy()
        {
            return std::make_shared<RegistrationData>(factory, lifetime);
        }
    private:
        LifetimeScope lifetime = LifetimeScope::PerDependency;
        std::shared_ptr<void> instanceIfShared;
        std::mutex sharedInstanceMutex;

        UntypedFactory factory;       
    };
}
