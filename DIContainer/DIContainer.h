#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>

class UnresolvedDependencyException : public std::logic_error
{
public:
    UnresolvedDependencyException()
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
    void registerType( std::function<std::shared_ptr<T>(DIContainer &)> creator )
    {
        dependencies[typeid(T)] = creator;
    }    

private:
    std::unordered_map< 
        std::type_index, 
        std::function<std::shared_ptr<void>(DIContainer &) > 
    > dependencies;

};