using System;
using System.Collections.Generic;

namespace Lab1.Models;

public class Truck
{
    public VolumeWeight Capacity { get; }
    public Speed Speed { get; }
    public Coordinates Coordinates { get; private set; }
    
    private readonly Dictionary<Sku, int> _cargo;
    public IReadOnlyDictionary<Sku, int> Cargo => _cargo;

    public Truck(VolumeWeight capacity, Speed speed, Coordinates initialCoordinates)
    {
        Capacity = capacity ?? throw new ArgumentNullException(nameof(capacity));
        Speed = speed ?? throw new ArgumentNullException(nameof(speed));
        Coordinates = initialCoordinates;
        _cargo = new Dictionary<Sku, int>();
    }

    public void Load(IReadOnlyDictionary<Sku, int> items)
    {
        if (items == null) throw new ArgumentNullException(nameof(items));

        VolumeWeight itemsVolumeWeight = CalculateVolumeWeight(items);
        VolumeWeight currentVolumeWeight = CalculateVolumeWeight(_cargo);

        if (currentVolumeWeight + itemsVolumeWeight > Capacity)
            throw new InvalidOperationException("Not enough capacity in the truck.");

        foreach (var item in items)
        {
            if (_cargo.ContainsKey(item.Key))
                _cargo[item.Key] += item.Value;
            else
                _cargo[item.Key] = item.Value;
        }
    }

    public void Unload(IReadOnlyDictionary<Sku, int> items)
    {
        if (items == null) throw new ArgumentNullException(nameof(items));

        foreach (var item in items)
        {
            if (!_cargo.ContainsKey(item.Key) || _cargo[item.Key] < item.Value)
                throw new InvalidOperationException($"Not enough '{item.Key.Name}' in the truck.");
        }

        foreach (var item in items)
        {
            _cargo[item.Key] -= item.Value;
            if (_cargo[item.Key] == 0)
                _cargo.Remove(item.Key);
        }
    }

    public void MoveTo(Coordinates newCoordinates)
    {
        Coordinates = newCoordinates;
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
