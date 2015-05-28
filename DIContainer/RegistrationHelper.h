#pragma once

class DIContainer;

template<class ImplementationType>
class RegistrationHelper
{
public:
    explicit RegistrationHelper(DIContainer &container, std::function<std::shared_ptr<ImplementationType>(DIContainer &) > creator)
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

    DIContainer &container;
    std::function<std::shared_ptr<ImplementationType>(DIContainer &) > creator;
};
