#pragma once

#include <stdexcept>
#include <memory>
#include <unordered_map>
#include "internal/RegistrationData.h"
#include "internal/RegistrationKey.h"
#include "IComponentContext.h"


namespace DIContainer
{
    class UnresolvedDependencyException : public std::logic_error
    {
    public:
        UnresolvedDependencyException()
            : std::logic_error("A dependency could not be resolved")
        {}
    };
    
    class ContainerBuilder;

    /// Class to resolve dependencies.
    ///
    /// All dependencies are resolved at the Container. The container is created
    /// and configured using the ContainerBuilder
    ///
    class Container : public IComponentContext
    {
    public:

        Container() {}

    private:

        std::unordered_map <
            RegistrationKey,
            std::shared_ptr<RegistrationData>
        > dependencies;

        RegistrationData &lookupRegistration(const IService &reg)
        {
            auto iter = dependencies.find(RegistrationKey::forLookup(reg));
            if (iter == dependencies.end())
                throw UnresolvedDependencyException();
            return *iter->second;
        }

        virtual std::shared_ptr< void > resolveService(const IService & service)
        {
            auto &registarion = lookupRegistration(service);
            return registarion.build(*this);
        }

        friend class ContainerBuilder;
    };

}

