using System;
using System.Collections.Generic;
using Lab1.Models;
using Lab1.Routing;

namespace Lab1.Tests;

public static class TestRunner
{
    public static void AssertTrue(bool condition, string message)
    {
        if (!condition) throw new Exception($"Test failed: {message}");
    }

    public static void AssertThrows<T>(Action action) where T : Exception
    {
        try
        {
            action();
            throw new Exception($"Expected exception {typeof(T).Name} was not thrown.");
        }
        catch (T) { } // Success
    }
    
    public static void RunAll()
    {
        Console.WriteLine("Running tests...");
        TestSkuCreation();
        TestTruckCreation();
        TestTruckLoading();
        TestTruckUnloading();
        TestTruckMoving();
        TestEmployeeMultipleTrips();
        TestWarehouseCapacityLimit();
        TestWarehouseDispatching();
        TestWarehouseStaffCalculations();
        TestWarehouseAcceptance();
        TestWarehouseDispatchTime();
        TestAcceptanceStepErrors();
        TestDispatchStepErrors();
        TestRouteListExecution();
        TestRouteListInterruption();
        Console.WriteLine("All tests passed!");
    }

    private static void TestSkuCreation()
    {
        var sku = new Sku(Guid.NewGuid(), "Box", new VolumeWeight(10));
        AssertTrue(sku.VolumeWeight.Value == 10, "SKU created properly");
        AssertThrows<ArgumentException>(() => new Sku(Guid.NewGuid(), "Box", new VolumeWeight(-5)));
        AssertThrows<ArgumentException>(() => new Sku(Guid.NewGuid(), "Box", new VolumeWeight(0)));
    }

    private static void TestTruckCreation()
    {
        AssertThrows<ArgumentException>(() => new Speed(0));
        AssertThrows<ArgumentException>(() => new Speed(-10));
    }

    private static void TestTruckLoading()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(40));
        
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        AssertTrue(truck.Cargo[sku] == 2, "Cargo loaded");
        
        AssertThrows<InvalidOperationException>(() => truck.Load(new Dictionary<Sku, int> { { sku, 1 } })); // 80 + 40 > 100
    }

    private static void TestTruckUnloading()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(40));
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        
        truck.Unload(new Dictionary<Sku, int> { { sku, 1 } });
        AssertTrue(truck.Cargo[sku] == 1, "Cargo unloaded correctly");
        
        AssertThrows<InvalidOperationException>(() => truck.Unload(new Dictionary<Sku, int> { { sku, 2 } })); // Try to unload more than available
    }

    private static void TestTruckMoving()
    {
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0)); 
        var dest = new Coordinates(0, 1); 
        var route = new MovementStep(dest);
        
        var result = route.Execute(truck);
        AssertTrue(result is RouteStepResult.Success, "Movement success");
        if (result is RouteStepResult.Success s)
        {
            AssertTrue(s.Duration.TotalSeconds == 5550, "Correct movement time");
        }
        AssertTrue(truck.Coordinates.Latitude == 0 && truck.Coordinates.Longitude == 1, "Coordinates updated");
    }

    private static void TestEmployeeMultipleTrips()
    {
        var employee = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        
        // Items volume weight = 25 (requires 3 trips: 10, 10, 5)
        var duration = warehouse.CalculateAcceptanceTime(new VolumeWeight(25));
        
        // 1st trip: load/unload = 5, transfer = 10. Done at 15
        // 2nd trip: load/unload = 5, transfer = 10. Done at 30
        // 3rd trip: load/unload = 5, transfer = 10. Done at 45
        // Since it's Acceptance, truck release time is max(unloadDoneTime).
        // 1st trip unload at 5, 2nd trip unload at 15 + 5 = 20, 3rd trip unload at 30 + 5 = 35.
        // Therefore, duration = 35.
        AssertTrue(duration.TotalSeconds == 35, $"Multiple trips duration incorrect, got {duration.TotalSeconds}");
    }

    private static void TestWarehouseCapacityLimit()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        AssertTrue(warehouse.Stock[sku] == 1, "Item accepted");
        
        AssertThrows<InvalidOperationException>(() => warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } })); // 60 + 60 > 100
        AssertTrue(warehouse.Stock[sku] == 1, "Stock didn't change on failure");
    }

    private static void TestWarehouseDispatching()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        warehouse.Dispatch(new Dictionary<Sku, int> { { sku, 1 } });
        AssertTrue(!warehouse.Stock.ContainsKey(sku), "Item dispatched");
        
        AssertThrows<InvalidOperationException>(() => warehouse.Dispatch(new Dictionary<Sku, int> { { sku, 1 } })); // Not enough stock
    }

    private static void TestWarehouseStaffCalculations()
    {
        var employee1 = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var employee2 = new Employee(new VolumeWeight(10), TimeSpan.FromSeconds(5), TimeSpan.FromSeconds(10));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee1, employee2 });
        
        // 2 employees, total needed: 25.
        // E1: 1st trip (10), starts at 0, unload at 5, available at 15
        // E2: 1st trip (10), starts at 0, unload at 5, available at 15
        // E1 (or E2): 2nd trip (5), starts at 15, unload at 20, available at 30
        // Duration = max unload time = 20
        var duration = warehouse.CalculateAcceptanceTime(new VolumeWeight(25));
        AssertTrue(duration.TotalSeconds == 20, $"Multiple employees duration incorrect, got {duration.TotalSeconds}");
    }

    private static void TestWarehouseAcceptance()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(100));
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        truck.Load(new Dictionary<Sku, int> { { sku, 1 } }); 
        
        var acceptStep = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        var result = acceptStep.Execute(truck);
        
        AssertTrue(result is RouteStepResult.Success, "Acceptance success");
        if (result is RouteStepResult.Success s)
        {
            AssertTrue(s.Duration.TotalSeconds == 40, $"Duration was {s.Duration.TotalSeconds}, expected 40");
        }
        AssertTrue(warehouse.Stock[sku] == 1, "Item in warehouse");
        AssertTrue(truck.Cargo.Count == 0, "Truck is empty");
    }

    private static void TestWarehouseDispatchTime()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(100));
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        var dispatchStep = new DispatchStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        var result = dispatchStep.Execute(truck);
        
        AssertTrue(result is RouteStepResult.Success, "Dispatch success");
        if (result is RouteStepResult.Success s)
        {
            AssertTrue(s.Duration.TotalSeconds == 60, $"Duration was {s.Duration.TotalSeconds}, expected 60");
        }
    }

    private static void TestAcceptanceStepErrors()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        var acceptStep = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        
        // 1. Not enough items in truck
        var res1 = acceptStep.Execute(truck);
        AssertTrue(res1 is RouteStepResult.Failure, "Failure due to lack of items in truck");
        
        // 2. Not enough capacity in warehouse
        truck.Load(new Dictionary<Sku, int> { { sku, 2 } });
        var acceptStep2 = new AcceptanceStep(warehouse, new Dictionary<Sku, int> { { sku, 2 } }); // 120 > 100
        var res2 = acceptStep2.Execute(truck);
        AssertTrue(res2 is RouteStepResult.Failure, "Failure due to lack of capacity in warehouse");
        
        // 3. Truck too far
        truck.MoveTo(new Coordinates(1, 1));
        var res3 = acceptStep.Execute(truck);
        AssertTrue(res3 is RouteStepResult.Failure, "Failure due to distance > 10m");
    }

    private static void TestDispatchStepErrors()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(100), new[] { employee });
        var sku = new Sku(Guid.NewGuid(), "HeavyItem", new VolumeWeight(60));
        
        var truck = new Truck(new VolumeWeight(100), new Speed(20), new Coordinates(0, 0));
        var dispatchStep = new DispatchStep(warehouse, new Dictionary<Sku, int> { { sku, 1 } });
        
        // 1. Not enough items in warehouse
        var res1 = dispatchStep.Execute(truck);
        AssertTrue(res1 is RouteStepResult.Failure, "Failure due to lack of items in warehouse");
        
        // 2. Not enough capacity in truck
        warehouse.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        truck.Load(new Dictionary<Sku, int> { { sku, 1 } }); // Truck now has 60, only 40 capacity left.
        var res2 = dispatchStep.Execute(truck);
        AssertTrue(res2 is RouteStepResult.Failure, "Failure due to lack of capacity in truck");
        truck.Unload(new Dictionary<Sku, int> { { sku, 1 } });
        
        // 3. Truck too far
        truck.MoveTo(new Coordinates(1, 1));
        var res3 = dispatchStep.Execute(truck);
        AssertTrue(res3 is RouteStepResult.Failure, "Failure due to distance > 10m");
    }

    private static void TestRouteListExecution()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse1 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var warehouse2 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 1), new VolumeWeight(500), new[] { employee });
        
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(100));
        warehouse1.Accept(new Dictionary<Sku, int> { { sku, 1 } });
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        
        RouteStep route = new RouteList(new RouteStep[] 
        {
            new DispatchStep(warehouse1, new Dictionary<Sku, int> { { sku, 1 } }),
            new MovementStep(warehouse2.Coordinates),
            new AcceptanceStep(warehouse2, new Dictionary<Sku, int> { { sku, 1 } })
        });
        
        var result = route.Execute(truck);
        AssertTrue(result is RouteStepResult.Success, "Route success");
        if (result is RouteStepResult.Success s)
        {
            AssertTrue(s.Duration.TotalSeconds == (60 + 5550 + 40), "Route correct duration");
        }
    }

    private static void TestRouteListInterruption()
    {
        var employee = new Employee(new VolumeWeight(50), TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(20));
        var warehouse1 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 0), new VolumeWeight(500), new[] { employee });
        var warehouse2 = new Warehouse(Guid.NewGuid(), new Coordinates(0, 1), new VolumeWeight(500), new[] { employee });
        
        var sku = new Sku(Guid.NewGuid(), "Item", new VolumeWeight(100));
        
        var truck = new Truck(new VolumeWeight(500), new Speed(20), new Coordinates(0, 0));
        
        RouteStep route = new RouteList(new RouteStep[] 
        {
            new DispatchStep(warehouse1, new Dictionary<Sku, int> { { sku, 1 } }), // Fails, empty warehouse
            new MovementStep(warehouse2.Coordinates)
        });
        
        var result = route.Execute(truck);
        AssertTrue(result is RouteStepResult.Failure, "Route fails if one step fails");
        AssertTrue(truck.Coordinates.Latitude == 0 && truck.Coordinates.Longitude == 0, "Truck didn't move after failure");
    }
}

