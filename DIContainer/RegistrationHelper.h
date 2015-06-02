#pragma once
#include "RegistrationData.h"

namespace DIContainer
{
    class Container;
    class ContainerBuilder;

    /// Helper class to configure the container builder, for internal use
    ///
    template<class ImplementationType>
    class RegistrationHelper
    {
    public:
        explicit RegistrationHelper(
            ContainerBuilder &containerBuilder, 
            RegistrationData *registrationData)
            : containerBuilder(containerBuilder), registration(registrationData) {}

        template<class InterfaceType>
        RegistrationHelper &as()
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does not implement interface");

            containerBuilder.wireInterfaceInternal<InterfaceType>(registration);

            return *this;
        }

        template<class InterfaceType>
        RegistrationHelper &named(const std::string &name)
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does no implement interface");

            containerBuilder.wireInterfaceInternal<InterfaceType>(name, registration);
            return *this;
        }

        void singleInstance()
        {     
            registration->setLifetimeScope(LifetimeScope::SingleInstance);
        }

        ContainerBuilder &containerBuilder;
        RegistrationData *registration;
    };

}