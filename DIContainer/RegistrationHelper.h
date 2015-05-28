#pragma once

namespace DIContainer
{

    class Container;

    template<class ImplementationType>
    class RegistrationHelper
    {
    public:
        explicit RegistrationHelper(Container &container, std::function<std::shared_ptr<ImplementationType>(Container &) > creator)
            : container(container), creator(creator) {}

        template<class InterfaceType>
        void as()
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered must implement interface");

            container.wireInterfaceInternal<InterfaceType>(creator);
        }

        template<class InterfaceType>
        void named(const std::string &name)
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered must implement interface");

            container.wireInterfaceInternal<InterfaceType>(name, creator);
        }

        Container &container;
        std::function<std::shared_ptr<ImplementationType>(Container &) > creator;
    };

}