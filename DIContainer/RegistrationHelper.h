#pragma once

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
            std::function<std::shared_ptr<ImplementationType>(Container &) > creator)
            : containerBuilder(containerBuilder), creator(creator) {}

        template<class InterfaceType>
        void as()
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does not implement interface");

            containerBuilder.wireInterfaceInternal<InterfaceType>(creator);
        }

        template<class InterfaceType>
        void named(const std::string &name)
        {
            static_assert(
                std::is_base_of<InterfaceType, ImplementationType>::value,
                "Registered type does no implement interface");

            containerBuilder.wireInterfaceInternal<InterfaceType>(name, creator);
        }

        ContainerBuilder &containerBuilder;
        std::function<std::shared_ptr<ImplementationType>(Container &) > creator;
    };

}