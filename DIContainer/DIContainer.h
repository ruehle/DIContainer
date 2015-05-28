#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>

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

    template<class T>
    void registerType( std::function<std::shared_ptr<T>(DIContainer &)> creator )
    {
        if (dependencies.count(typeid(T)) > 0)
            throw DuplicateDependencyException();
        dependencies[typeid(T)] = creator;
    }   

    template<class T>
    void registerType( const std::string &name, std::function<std::shared_ptr<T>(DIContainer &)> creator)
    {
        auto key = std::make_pair(name, std::type_index(typeid(T)));
        if (namedDependencies.count(key) > 0)
            throw DuplicateDependencyException();

        namedDependencies[key] = creator;
    }

private:
    std::unordered_map< 
        std::type_index, 
        std::function<std::shared_ptr<void>(DIContainer &) > 
    > dependencies;

    std::map<
        std::pair<std::string, std::type_index>,
        std::function < std::shared_ptr<void>(DIContainer &) >
    > namedDependencies;
};