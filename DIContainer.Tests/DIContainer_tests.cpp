#include "gtest/gtest.h"
#include "../DIContainer/DIContainer.h"

class ITestInterface
{
public:
    virtual ~ITestInterface() = default;
};

class TestImplementation : public ITestInterface
{};

class ITestInterface2
{
public:
    virtual ~ITestInterface2() = default;
};

class TestImplementation2 : public ITestInterface2
{
public:
    explicit TestImplementation2( 
        std::shared_ptr<ITestInterface> dependency )
    {
        if (dependency == nullptr)
            throw std::logic_error("argument is nullptr");        
    }    
};


TEST(DIContainerTests, ResolveUnregisteredDependency_TrowsUnresolvedDependencyException )
{
    DIContainer resolver;
    ASSERT_THROW(resolver.resolve<ITestInterface>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance )
{
    DIContainer resolver;
    resolver.registerType<ITestInterface>(
        [](DIContainer &r) { return std::make_shared<TestImplementation>(); }
    );

    auto obj = resolver.resolve< ITestInterface >();
    auto objCasted = std::dynamic_pointer_cast<TestImplementation>(obj);
    ASSERT_NE( objCasted, nullptr );
}

TEST(DIContainerTests, CallResolveTwice_ReturnsDifferentInstances )
{
    DIContainer resolver;
    resolver.registerType<ITestInterface>(
        [](DIContainer &r) { return std::make_shared<TestImplementation>(); }
    );

    auto obj1 = resolver.resolve< ITestInterface >();
    auto obj2 = resolver.resolve< ITestInterface >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, ResolveImplementationWithDependency_Succeeds)
{
    DIContainer resolver;
    resolver.registerType<ITestInterface>(
        [](DIContainer &r) { return std::make_shared<TestImplementation>(); }
    );

    resolver.registerType<ITestInterface2>( [](DIContainer &r) { 
        return std::make_shared<TestImplementation2>(
            r.resolve<ITestInterface>() ); 
    }
    );

    auto obj = resolver.resolve< ITestInterface2 >();
    ASSERT_NE(obj, nullptr);
}