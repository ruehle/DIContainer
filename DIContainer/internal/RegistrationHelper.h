#pragma once

#include "RegistrationData.h"
#include "TypedService.h"
#include "KeyedService.h"

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
            std::shared_ptr<RegistrationData> registrationData
            )
            : containerBuilder(containerBuilder), registration(registrationData) {}

        template<class InterfaceType>
        RegistrationHelper &as()
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does not implement interface");

            containerBuilder.wireInterfaceInternal(
                std::make_shared<TypedService<InterfaceType>>(),
                registration
                );

            return *this;
        }

        template<class InterfaceType, class KeyType>
        RegistrationHelper &keyed(const KeyType &key)
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does no implement interface");

            containerBuilder.wireInterfaceInternal(
                std::make_shared<KeyedService<InterfaceType, KeyType>>(key),
                registration);
            return *this;
        }

        template<class InterfaceType>
        RegistrationHelper &named(const std::string &name)
        {
            return keyed<InterfaceType>(name);
        }


        void singleInstance()
        {
            registration->setLifetimeScope(LifetimeScope::SingleInstance);
        }

        ContainerBuilder &containerBuilder;
        std::shared_ptr<RegistrationData> registration;
    };

}