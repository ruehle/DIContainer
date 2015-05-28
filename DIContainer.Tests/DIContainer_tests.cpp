#include "gtest/gtest.h"
#include "../DIContainer/DIContainer.h"
#include "../DIContainer/Injector.h"

class ITestInterface
{
public:
    virtual ~ITestInterface() = default;
};

class TestImplementation : public ITestInterface
{};

class TestImplementation2 : public ITestInterface
{};

class TestImplementation3 : public ITestInterface
{};

class ITestInterface2
{
public:
    virtual ~ITestInterface2() = default;
};

class Interface2Implementation : public ITestInterface2
{
public:
    explicit Interface2Implementation(
        std::shared_ptr<ITestInterface> dependency)
    {
        if (dependency == nullptr)
            throw std::logic_error("argument is nullptr");
    }
};


TEST(DIContainerTests, ResolveUnregisteredDependency_TrowsUnresolvedDependencyException)
{
    DIContainer resolver;
    ASSERT_THROW(resolver.resolve<ITestInterface>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance)
{
    DIContainer resolver;
    resolver.registerType<TestImplementation>().as<ITestInterface>();

    auto obj = resolver.resolve< ITestInterface >();
    auto objCasted = std::dynamic_pointer_cast<TestImplementation>(obj);
    ASSERT_NE(objCasted, nullptr);
}

TEST(DIContainerTests, RegisteredSameDependencyTwice_ThrowsDuplicateDependencyException)
{
    DIContainer resolver;

    resolver.registerType<TestImplementation>().as<ITestInterface>();

    ASSERT_THROW(
        resolver.registerType<TestImplementation>().as<ITestInterface>(),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisteredNamedAndUnnamedDependency_Succeeds)
{
    DIContainer resolver;

    auto creator = [](DIContainer &r) { return std::make_shared<TestImplementation>(); };

    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();
    
    resolver.registerType<TestImplementation2>()
        .named<ITestInterface>("name2");
    
    resolver.registerType<TestImplementation3>()
        .named<ITestInterface>("name3");
    
}

TEST(DIContainerTests, ResolveNamedAndUnnamedTypes_InstanciateCorrectly)
{
    DIContainer resolver;

    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    resolver.registerType<TestImplementation2>()
        .named<ITestInterface>("name2");

    resolver.registerType<TestImplementation3>()
        .named<ITestInterface>("name3");

    auto obj = std::dynamic_pointer_cast<TestImplementation>(resolver.resolve<ITestInterface>());
    auto obj2 = std::dynamic_pointer_cast<TestImplementation2>(resolver.resolve<ITestInterface>("name2"));
    auto obj3 = std::dynamic_pointer_cast<TestImplementation3>(resolver.resolve<ITestInterface>("name3"));

    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj2, nullptr);
    ASSERT_NE(obj3, nullptr);
}

TEST(DIContainerTests, RegisterDependencyWithSameNameAndType_ThrowsDuplicateDependencyException)
{
    DIContainer resolver;

    auto creator = [](DIContainer &r) { return std::make_shared<TestImplementation>(); };

    resolver.registerType<TestImplementation>()
        .named<ITestInterface>("name");
    ASSERT_THROW(
        resolver.registerType<TestImplementation>()
        .named<ITestInterface>("name"),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, CallResolveTwice_ReturnsDifferentInstances)
{
    DIContainer resolver;
    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    auto obj1 = resolver.resolve< ITestInterface >();
    auto obj2 = resolver.resolve< ITestInterface >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, ResolveImplementationWithDependencyByCode_Succeeds)
{
    DIContainer resolver;
    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    auto create = [](DIContainer &r){
        return Injector<ITestInterface>::create<Interface2Implementation>(r); };

    resolver.registerType<Interface2Implementation>(create)
        .as<ITestInterface2>();


    auto obj = resolver.resolve< ITestInterface2 >();
    ASSERT_NE(obj, nullptr);
}