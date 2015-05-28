# DIContainer
Non-intrusive dependency injection container for C++

## Getting started
```cpp
Container container;
container.registerType< ServiceImplementation >().as<IService>();
auto service = container.Resolve<IService>();
```

## Register types

Register service without dependencies
```cpp
container.registerType< ServiceImplementation >().as<IService>();
```

Register service with dependencies by lambda expression
```cpp
container.registerType< DependentServiceImplementation >( 
    []{Container &r} { return std::make_shared<DependentServiceImplementation>( r.resolve<IService>() ); }
    ).as<IDependentService>();
```

Register service with dependencies using Injector helper class
```cpp
container.registerType< DependentServiceImplementation >( Injector<IService>() ).as<IDependentService>();
```

Register an instance
```cpp
auto instance = std::make_shared<ServiceImplementation>();
container.registerInstance( instance ).as<IService>();
```

## Named services
```cpp
container.registerType< ServiceImplementation >().named<IService>("myservice");
auto service = container.resolve<IService>("myservice");
```

