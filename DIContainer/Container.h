#pragma once

#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>
#include "RegistrationData.h"
#include "RegistrationKey.h"


namespace DIContainer
{
    class UnresolvedDependencyException : public std::logic_error
    {
    public:
        UnresolvedDependencyException()
            : std::logic_error("A dependency could not be resolved")
        {}
    };

    /// Class to resolve dependencies.
    ///
    /// All dependencies are resolved at the Container. The container is created
    /// and configured using the ContainerBuilder
    ///
    class Container
    {
    public:

        Container(std::vector<RegistrationData> registeredTypes,
            std::unordered_map <
            RegistrationKey,
            std::size_t
            > dependencies
            )
            : registeredTypes(registeredTypes), dependencies(dependencies) {}

        template<class T>
        std::shared_ptr<T> resolve()
        {
            TypedRegistration<T> reg;
            auto creatorIter = dependencies.find(RegistrationKey::forLookup(reg));
            if (creatorIter == dependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(registeredTypes[creatorIter->second].build(*this));
        }

        template<class T>
        std::shared_ptr<T> resolve(const std::string &name)
        {
            KeyedRegistration<T, std::string> reg(name);
            auto key = std::make_pair(name, std::type_index(typeid(T)));
            auto creatorIter = dependencies.find(RegistrationKey::forLookup(reg));
            if (creatorIter == dependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(registeredTypes[creatorIter->second].build(*this));
        }


    private:

        std::vector<RegistrationData> registeredTypes;

        std::unordered_map <
            RegistrationKey,
            std::size_t
        > dependencies;

    };

}

