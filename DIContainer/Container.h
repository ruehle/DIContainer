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
            TypedService<T> reg;
            auto creatorIter = dependencies.find(RegistrationKey::forLookup(reg));
            if (creatorIter == dependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(registeredTypes[creatorIter->second].build(*this));
        }

        template<class T, class KeyType>
        std::shared_ptr<T> resolveKeyed(const KeyType &key)
        {
            KeyedService<T,KeyType> reg(key);
            auto creatorIter = dependencies.find(RegistrationKey::forLookup(reg));
            if (creatorIter == dependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(registeredTypes[creatorIter->second].build(*this));
        }

        template<class T>
        std::shared_ptr<T> resolveNamed(const std::string &name)
        {
            return resolveKeyed<T>(name);
        }


    private:

        std::vector<RegistrationData> registeredTypes;

        std::unordered_map <
            RegistrationKey,
            std::size_t
        > dependencies;

    };

}

