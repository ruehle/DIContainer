#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include "Injector.h"
#include "internal/RegistrationHelper.h"
#include "internal/RegistrationData.h"
#include "internal/RegistrationKey.h"

#include "Container.h"

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
                *this, createRegistration(creator));
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
            auto container = std::make_shared<Container>();
            
            CopyHelper copyManager;
            for (auto & i : dependencies) {
                auto &reg = registeredTypes[i.second];

                container->dependencies.insert(
                    std::make_pair(i.first, copyManager.getCopy(i.second, reg))
                    );
            }
            return container;
        }

    private:

        bool duplicateCheck = false;

        void wireInterfaceInternal(std::shared_ptr<IService> registration, size_t id)
        {
            if (dependencies.count(RegistrationKey(registration)) > 0 && duplicateCheck)
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

        class CopyHelper
        {
        public:
            std::shared_ptr<RegistrationData> getCopy(size_t key, RegistrationData &registration)
            {
                auto exists = copiedInstances.find(key);
                if (exists == copiedInstances.end())
                {
                    auto copy = std::make_shared<RegistrationData>(registration);
                    copiedInstances[key] = copy;
                    return copy;
                }
                auto item = exists->second;
                return item;
            }
        private:
            std::unordered_map<size_t, std::shared_ptr<RegistrationData>> copiedInstances;
        };

    };

}