#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>
#include "Injector.h"

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

class DIContainer
{
public:

    template<class T>
    std::shared_ptr<T> resolve()
    {
        auto creatorIter = dependencies.find(typeid(T));
        if (creatorIter == dependencies.end())
            throw UnresolvedDependencyException();
        return std::static_pointer_cast<T>( creatorIter->second(*this) );
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

    class RegisterHelper
    {
    public:
        explicit RegisterHelper(DIContainer &container, std::function<std::shared_ptr<void>(DIContainer &) > creator)
            : container(container), creator(creator) {}

        template<class T>
        void as()
        {
            container.wireInterfaceInternal<T>(creator);
        }

        template<class T>
        void named(const std::string &name)
        {
            container.wireInterfaceInternal<T>(name, creator);
        }

        DIContainer &container;
        std::function<std::shared_ptr<void>(DIContainer &) > creator;        
    };

    template<class T>
    RegisterHelper registerType()
    {
        return RegisterHelper(
            *this, [](DIContainer &r) { return std::make_shared<T>(); });
    }

    template<class T>
    RegisterHelper registerType(std::function< std::shared_ptr<T>(DIContainer &)> creator)
    {
        return RegisterHelper(
            *this, creator);
    }

    template<class T, class... Args>
    RegisterHelper registerType( Injector<Args...> injector )
    {
        return RegisterHelper(
            *this, [injector](DIContainer &r) { return injector.template create<T>(r); });
    }

    template<class T>
    RegisterHelper registerInstance(std::shared_ptr<T> instance)
    {
        return RegisterHelper(
            *this, [instance](DIContainer &r) { return instance; });
    }



private:

    template<class T>
    void wireInterfaceInternal(std::function<std::shared_ptr<void>(DIContainer &)> creator)
    {
        if (dependencies.count(typeid(T)) > 0)
            throw DuplicateDependencyException();
        dependencies[typeid(T)] = creator;
    }

    template<class T>
    void wireInterfaceInternal(const std::string &name, std::function<std::shared_ptr<void>(DIContainer &)> creator)
    {
        auto key = std::make_pair(name, std::type_index(typeid(T)));
        if (namedDependencies.count(key) > 0)
            throw DuplicateDependencyException();

        namedDependencies[key] = creator;
    }


    std::unordered_map< 
        std::type_index, 
        std::function<std::shared_ptr<void>(DIContainer &) > 
    > dependencies;

    std::map<
        std::pair<std::string, std::type_index>,
        std::function < std::shared_ptr<void>(DIContainer &) >
    > namedDependencies;

    friend class RegisterHelper;
};