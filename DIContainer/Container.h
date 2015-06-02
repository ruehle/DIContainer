#pragma once

#include <stdexcept>
#include <functional>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <map>

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

        Container(
            std::unordered_map < std::type_index,
            std::function < std::shared_ptr<void>(Container &) >
            > dependencies,
            std::map < std::pair<std::string, std::type_index>,
            std::function < std::shared_ptr<void>(Container &) >
            > namedDependencies)
            : dependencies(dependencies), namedDependencies(namedDependencies) {}

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


    private:

        std::unordered_map <
            std::type_index,
            std::function < std::shared_ptr<void>(Container &) >
        > dependencies;

        std::map <
            std::pair<std::string, std::type_index>,
            std::function < std::shared_ptr<void>(Container &) >
        > namedDependencies;

    };

}

