#include "gtest/gtest.h"
#include "../DIContainer/Container.h"
#include "../DIContainer/Injector.h"

using namespace DIContainer;

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
    Container resolver;
    ASSERT_THROW(resolver.resolve<ITestInterface>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance)
{
    Container resolver;
    resolver.registerType<TestImplementation>().as<ITestInterface>();

    auto obj = resolver.resolve< ITestInterface >();
    auto objCasted = std::dynamic_pointer_cast<TestImplementation>(obj);
    ASSERT_NE(objCasted, nullptr);
}

TEST(DIContainerTests, RegisteredSameDependencyTwice_ThrowsDuplicateDependencyException)
{
    Container resolver;

    resolver.registerType<TestImplementation>().as<ITestInterface>();

    ASSERT_THROW(
        resolver.registerType<TestImplementation>().as<ITestInterface>(),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisteredNamedAndUnnamedDependency_Succeeds)
{
    Container resolver;

    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    resolver.registerType<TestImplementation2>()
        .named<ITestInterface>("name2");

    resolver.registerType<TestImplementation3>()
        .named<ITestInterface>("name3");

}

TEST(DIContainerTests, ResolveNamedAndUnnamedTypes_InstanciateCorrectly)
{
    Container resolver;

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
    Container resolver;

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
    Container resolver;
    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    auto obj1 = resolver.resolve< ITestInterface >();
    auto obj2 = resolver.resolve< ITestInterface >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, ResolveImplementationWithDependencyByCode_Succeeds)
{
    Container resolver;
    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    auto create = [](Container &r){
        return std::make_shared<Interface2Implementation>(
            r.resolve<ITestInterface>()
            ); };

    resolver.registerType<Interface2Implementation>(create)
        .as<ITestInterface2>();


    auto obj = resolver.resolve< ITestInterface2 >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependency_Succeeds)
{
    Container resolver;

    resolver.registerType<TestImplementation>()
        .as<ITestInterface>();

    resolver.registerType<Interface2Implementation>(Injector<ITestInterface>())
        .as<ITestInterface2>();

    auto obj = resolver.resolve< ITestInterface2 >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, RegisterAndResolveInstance_ReturnsSameInstance)
{
    Container resolver;

    auto instance = std::make_shared<TestImplementation>();

    resolver.registerInstance(instance)
        .as<ITestInterface>();

    auto obj1 = resolver.resolve< ITestInterface >();
    auto obj2 = resolver.resolve< ITestInterface >();

    ASSERT_EQ(instance, obj1);
    ASSERT_EQ(instance, obj2);
}