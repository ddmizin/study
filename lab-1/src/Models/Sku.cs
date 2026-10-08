using System;

namespace Lab1.Models;

public class Sku
{
    public Guid Id { get; }
    public string Name { get; }
    public VolumeWeight VolumeWeight { get; }

    public Sku(Guid id, string name, VolumeWeight volumeWeight)
    {
        if (volumeWeight == null) throw new ArgumentNullException(nameof(volumeWeight));
        if (volumeWeight.Value == 0)
            throw new ArgumentException("Volume weight for SKU must be strictly positive.", nameof(volumeWeight));
        
        Id = id;
        Name = string.IsNullOrWhiteSpace(name) ? throw new ArgumentException("Name cannot be empty.", nameof(name)) : name;
        VolumeWeight = volumeWeight;
    }
}
