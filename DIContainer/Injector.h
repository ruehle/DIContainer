#pragma once
#include <memory>
#include "Container.h"

namespace DIContainer
{
    class Container;

    /// Auto-wireing helper class for constructor injection
    ///
    /// The Injector is a helper class to automatically inject dependencies as
    /// constructor arguments, resolving all the dependencies from the container.
    /// The arguments which are injected are specified by template parameters.
    ///    
    template<class... Args>
    class Injector
    {
    public:
        template<class T>
        std::shared_ptr<T> create(IComponentContext &r) const
        {
            return std::make_shared<T>(r.resolve<Args>()...);
        }
    };

}
