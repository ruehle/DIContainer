#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>
#include "Injector.h"
#include "RegistrationHelper.h"

namespace DIContainer
{

    class UnresolvedDependencyException : public std::logic_error
    {
    public:
        UnresolvedDependencyException()
            : std::logic_error("A dependency could not be resolved")
        {}
    };

    class DuplicateDependencyException : public std::logic_error
    {
    public:
        DuplicateDependencyException()
            : std::logic_error("A dependency could not be resolved")
        {}
    };

    class Container
    {
    public:

        template<class T>
        std::shared_ptr<T> resolve()
        {
            auto creatorIter = dependencies.find(typeid(T));
            if (creatorIter == dependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(creatorIter->second(*this));
        }

        template<class T>
        std::shared_ptr<T> resolve(const std::string &name)
        {
            auto key = std::make_pair(name, std::type_index(typeid(T)));
            auto creatorIter = namedDependencies.find(key);
            if (creatorIter == namedDependencies.end())
                throw UnresolvedDependencyException();
            return std::static_pointer_cast<T>(creatorIter->second(*this));
        }

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

    private:

        template<class T>
        void wireInterfaceInternal(std::function<std::shared_ptr<T>(Container &)> creator)
        {
            if (dependencies.count(typeid(T)) > 0)
                throw DuplicateDependencyException();
            dependencies[typeid(T)] = creator;
        }

        template<class T>
        void wireInterfaceInternal(const std::string &name, std::function<std::shared_ptr<T>(Container &)> creator)
        {
            auto key = std::make_pair(name, std::type_index(typeid(T)));
            if (namedDependencies.count(key) > 0)
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

        template<class U>
        friend class RegistrationHelper;
    };

}