#include "gtest/gtest.h"
#include "../DIContainer/ContainerBuilder.h"
#include "../DIContainer/Injector.h"

using namespace DIContainer;

namespace {
    class IMyService
    {
    public:
        virtual ~IMyService() = default;
    };

    class ServiceImplementation : public IMyService
    {};

    class ServiceImplementation2 : public IMyService
    {};

    class ServiceImplementation3 : public IMyService
    {};

    class IAnotherService
    {
    public:
        virtual ~IAnotherService() = default;
    };

    class DependentServiceImplementation : public IAnotherService
    {
    public:
        explicit DependentServiceImplementation(
            std::shared_ptr<IMyService> dependency)
        {
            if (dependency == nullptr)
                throw std::logic_error("invalid argument");
        }
    };
}

TEST(DIContainerTests, ResolveUnregisteredDependency_TrowsUnresolvedDependencyException)
{
    ContainerBuilder builder;
    auto resolver = builder.build();
    ASSERT_THROW(resolver->resolve<IMyService>(), UnresolvedDependencyException);
}

TEST(DIContainerTests, RegisteredSameDependencyTwiceWithEnabledDuplicateCheck_ThrowsDuplicateDependencyException)
{
    ContainerBuilder builder;

    builder.enableDuplicatesCeck(true);

    builder.registerType<ServiceImplementation>().as<IMyService>();

    ASSERT_THROW(
        builder.registerType<ServiceImplementation>().as<IMyService>(),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisteredSameDependencyTwice_InstanciatesLastDependency)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>().as<IMyService>();
    builder.registerType<ServiceImplementation2>().as<IMyService>();

    auto resolver = builder.build();

    auto obj = resolver->resolve<IMyService>();
    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation2>(obj), nullptr);
}

TEST(DIContainerTests, ResolveRegisteredDependency_ReturnsInstance)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>().as<IMyService>();

    auto resolver = builder.build();
    auto obj = resolver->resolve< IMyService >();

    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation>(obj), nullptr);
}

std::shared_ptr<Container> buildContainer()
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>().as<IMyService>();

    return builder.build();	
}

TEST(DIContainerTests, ContainerBuilderOutOfScopeWhenResolving_Succeeds)
{
    auto resolver = buildContainer();
    auto obj = resolver->resolve< IMyService >();

    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation>(obj), nullptr);
}


TEST(DIContainerTests, CallResolveTwice_ReturnsDifferentInstances)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>()
        .as<IMyService>();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IMyService >();
    auto obj2 = resolver->resolve< IMyService >();

    ASSERT_NE(obj1, obj2);
}

TEST(DIContainerTests, RegisterDependencyWithSameNameAndTypeAndDuplicateCheckEnabled_ThrowsDuplicateDependencyException)
{
    ContainerBuilder builder;

    builder.enableDuplicatesCeck(true);

    builder.registerType<ServiceImplementation>()
        .named<IMyService>("name");

    ASSERT_THROW(
        builder.registerType<ServiceImplementation>()
        .named<IMyService>("name"),
        DuplicateDependencyException
        );
}

TEST(DIContainerTests, RegisterDependencyWithSameNameAndTypeAnd_InstanciatesLast)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .named<IMyService>("name");

    builder.registerType<ServiceImplementation2>()
        .named<IMyService>("name");

    auto resolver = builder.build();
    auto obj = resolver->resolveNamed<IMyService>( "name" );
    
    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation2>(obj), nullptr);
}

TEST(DIContainerTests, RegisteredNamedAndUnnamedDependency_Succeeds)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>();

    builder.registerType<ServiceImplementation2>()
        .named<IMyService>("name2");

    builder.registerType<ServiceImplementation3>()
        .named<IMyService>("name3");

}

TEST(DIContainerTests, ResolveNamedAndUnnamedTypes_InstanciateCorrectly)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>();

    builder.registerType<ServiceImplementation2>()
        .named<IMyService>("name2");

    builder.registerType<ServiceImplementation3>()
        .named<IMyService>("name3");

    auto resolver = builder.build();

    auto obj = std::dynamic_pointer_cast<ServiceImplementation>(
        resolver->resolve<IMyService>()
        );
    auto obj2 = std::dynamic_pointer_cast<ServiceImplementation2>(
        resolver->resolveNamed<IMyService>("name2")
        );
    auto obj3 = std::dynamic_pointer_cast<ServiceImplementation3>(
        resolver->resolveNamed<IMyService>("name3")
        );

    ASSERT_NE(obj, nullptr);
    ASSERT_NE(obj2, nullptr);
    ASSERT_NE(obj3, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependencyByCode_Succeeds)
{
    ContainerBuilder builder;
    builder.registerType<ServiceImplementation>()
        .as<IMyService>();

    auto create = [](Container &r){
        return std::make_shared<DependentServiceImplementation>(
            r.resolve<IMyService>()
            ); };

    builder.registerType<DependentServiceImplementation>(create)
        .as<IAnotherService>();


    auto resolver = builder.build();
    auto obj = resolver->resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, ResolveImplementationWithDependency_Succeeds)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>();

    builder.registerType<DependentServiceImplementation>(Injector<IMyService>())
        .as<IAnotherService>();

    auto resolver = builder.build();
    auto obj = resolver->resolve< IAnotherService >();
    ASSERT_NE(obj, nullptr);
}

TEST(DIContainerTests, RegisterAndResolveInstance_ReturnsSameInstance)
{
    ContainerBuilder builder;

    auto instance = std::make_shared<ServiceImplementation>();

    builder.registerInstance(instance)
        .as<IMyService>();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IMyService >();
    auto obj2 = resolver->resolve< IMyService >();

    ASSERT_EQ(instance, obj1);
    ASSERT_EQ(instance, obj2);
}

TEST(DIContainerTests, SingleInstanceLifetime_ReturnsSameInstance)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>().singleInstance();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IMyService >();
    auto obj2 = resolver->resolve< IMyService >();

    ASSERT_EQ(obj1, obj2);
}

TEST(DIContainerTests, SingleInstanceLifetimeWithDifferentRegistration_ReturnsSameInstance)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>()
        .named<IMyService>("name")
        .singleInstance();

    auto resolver = builder.build();

    auto obj1 = resolver->resolve< IMyService >();
    auto obj2 = resolver->resolveNamed< IMyService >("name");

    ASSERT_EQ(obj1, obj2);
}


TEST(DIContainerTests, SingleInstanceLifetimeFromDifferentBuilder_ReturnsDifferentInstance)
{
    ContainerBuilder builder;

    builder.registerType<ServiceImplementation>()
        .as<IMyService>().singleInstance();

    auto resolver1 = builder.build();
    auto resolver2 = builder.build();

    auto obj1 = resolver1->resolve< IMyService >();
    auto obj2 = resolver2->resolve< IMyService >();

    ASSERT_NE(obj1, obj2);
}

enum class ServiceType
{
    one, two
};

TEST(DIContainerTests, RegisterAndResolveKeyedInstance_Succeeds)
{
    ContainerBuilder builder;  

    builder.registerType<ServiceImplementation>()
        .keyed<IMyService>(ServiceType::one);

    builder.registerType<ServiceImplementation2>()
        .keyed<IMyService>(ServiceType::two);

    auto resolver = builder.build();

    auto obj1 = resolver->resolveKeyed< IMyService >(ServiceType::one);
    auto obj2 = resolver->resolveKeyed< IMyService >(ServiceType::two);

    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation>(obj1), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<ServiceImplementation2>(obj2), nullptr);
}
