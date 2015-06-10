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
#include "RegistrationKey.h"

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
            auto registration = createRegistration(
                [](Container &r) { return std::make_shared<T>(); }
            );

            return RegistrationHelper<T>(
                *this, 
                registration);
        }

        template<class T>
        RegistrationHelper<T> registerType(std::function< std::shared_ptr<T>(Container &)> creator)
        {
            return RegistrationHelper<T>(
                *this, createRegistration(creator) );
        }

        template<class T, class... Args>
        RegistrationHelper<T> registerType(Injector<Args...> injector)
        {
            return RegistrationHelper<T>(
                *this, 
                createRegistration([injector](Container &r) { return injector.template create<T>(r); })
                );
        }

        template<class T>
        RegistrationHelper<T> registerInstance(std::shared_ptr<T> instance)
        {
            return RegistrationHelper<T>(
                *this, 
                createRegistration([instance](Container &r) { return instance; })
                );
        }

        void enableDuplicatesCeck(bool isEnabled) { duplicateCheck = isEnabled; }

        std::shared_ptr<Container> build()
        {
            return std::make_shared<Container>(registeredTypes, dependencies);
        }

    private:

        bool duplicateCheck = false;

        void wireInterfaceInternal( std::shared_ptr<IServiceTyper> registration, size_t id )
        {
            if (dependencies.count(RegistrationKey(registration)) > 0 && duplicateCheck )
                throw DuplicateDependencyException();
            dependencies[RegistrationKey(registration)] = id;
        }

        RegistrationData *createRegistration( 
            RegistrationData::UntypedFactory creator
            )
        {                        
            registeredTypes.push_back(RegistrationData(registeredTypes.size(), creator));
            return &registeredTypes.back();
        }

        std::unordered_map <
            RegistrationKey,
            std::size_t
        > dependencies;

        std::vector<RegistrationData> registeredTypes;
        template<class U>
        friend class RegistrationHelper;
    };

}