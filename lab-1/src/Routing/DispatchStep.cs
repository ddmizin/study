using System;
using System.Collections.Generic;
using Lab1.Models;

namespace Lab1.Routing;

public class DispatchStep : RouteStep
{
    public Warehouse Warehouse { get; }
    public IReadOnlyDictionary<Sku, int> Items { get; }

    public DispatchStep(Warehouse warehouse, IReadOnlyDictionary<Sku, int> items)
    {
        Warehouse = warehouse ?? throw new ArgumentNullException(nameof(warehouse));
        Items = items ?? throw new ArgumentNullException(nameof(items));
    }

    public override RouteStepResult Execute(Truck truck)
    {
        if (truck == null) throw new ArgumentNullException(nameof(truck));

        if (truck.Coordinates.DistanceTo(Warehouse.Coordinates) > Distance.FromMeters(10))
            return new RouteStepResult.Failure("Truck is not at the warehouse (distance > 10m).");

        foreach (var item in Items)
        {
            if (!Warehouse.Stock.ContainsKey(item.Key) || Warehouse.Stock[item.Key] < item.Value)
                return new RouteStepResult.Failure($"Warehouse does not have enough '{item.Key.Name}'.");
        }

        VolumeWeight itemsVolumeWeight = CalculateVolumeWeight(Items);
        VolumeWeight truckCurrentVolumeWeight = CalculateVolumeWeight(truck.Cargo);

        if (truckCurrentVolumeWeight + itemsVolumeWeight > truck.Capacity)
            return new RouteStepResult.Failure("Truck does not have enough capacity to load the items.");

        TimeSpan duration = Warehouse.CalculateDispatchTime(itemsVolumeWeight);

        Warehouse.Dispatch(Items);
        truck.Load(Items);

        return new RouteStepResult.Success(duration);
    }
    
    private VolumeWeight CalculateVolumeWeight(IReadOnlyDictionary<Sku, int> items)
    {
        VolumeWeight total = VolumeWeight.Zero;
        foreach (var item in items)
        {
            total += item.Key.VolumeWeight * item.Value;
        }
        return total;
    }
}
