#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>
#include "Injector.h"
#include "RegistrationHelper.h"
#include "Container.h"
#include "RegistrationData.h"

namespace DIContainer
{
    /// A dependency for this interface was already registered
    ///
    class DuplicateDependencyException : public std::logic_error
    {
    public:
        DuplicateDependencyException()
            : std::logic_error("A dependency could not be resolved")
        {}
    };

    /// Class do perform wire up and instantiate Container
    ///
    /// All dependencies must be registered at the ContainerBuilder. The build function
    /// creates a container which can be used to resolve dependencies.
    ///
    class ContainerBuilder
    {
    public:

        template<class T>
        RegistrationHelper<T> registerType()
        {
            return RegistrationHelper<T>(
                *this, [](Container &r) { return std::make_shared<T>(); });
        }

        template<class T>
        RegistrationHelper<T> registerType(std::function< std::shared_ptr<T>(Container &)> creator)
        {
            return RegistrationHelper<T>(
                *this, creator);
        }

        template<class T, class... Args>
        RegistrationHelper<T> registerType(Injector<Args...> injector)
        {
            return RegistrationHelper<T>(
                *this, [injector](Container &r) { return injector.template create<T>(r); });
        }

        template<class T>
        RegistrationHelper<T> registerInstance(std::shared_ptr<T> instance)
        {
            return RegistrationHelper<T>(
                *this, [instance](Container &r) { return instance; });
        }

        void enableDuplicatesCeck(bool isEnabled) { duplicateCheck = isEnabled; }

        std::shared_ptr<Container> build()
        {
            return std::make_shared<Container>(dependencies, namedDependencies);
        }

    private:

        bool duplicateCheck = false;

        template<class T>
        void wireInterfaceInternal(std::function<std::shared_ptr<T>(Container &)> creator)
        {
            if (dependencies.count(typeid(T)) > 0 && duplicateCheck )
                throw DuplicateDependencyException();
            dependencies[typeid(T)] = creator;
        }

        template<class T>
        void wireInterfaceInternal(const std::string &name, std::function<std::shared_ptr<T>(Container &)> creator)
        {
            auto key = std::make_pair(name, std::type_index(typeid(T)));
            if (namedDependencies.count(key) > 0 && duplicateCheck )
                throw DuplicateDependencyException();

            namedDependencies[key] = creator;
        }


        std::unordered_map <
            std::type_index,
            std::function < std::shared_ptr<void>(Container &) >
        > dependencies;

        std::map <
            std::pair<std::string, std::type_index>,
            std::function < std::shared_ptr<void>(Container &) >
        > namedDependencies;

        std::vector<std::shared_ptr<RegistrationData>> registeredTypes;
        template<class U>
        friend class RegistrationHelper;
    };

}