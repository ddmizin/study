using System;

namespace Lab1.Models;

public class Employee
{
    public VolumeWeight MaxCarryingCapacity { get; }
    public TimeSpan LoadUnloadTime { get; }
    public TimeSpan TransferTime { get; }

    public Employee(VolumeWeight maxCarryingCapacity, TimeSpan loadUnloadTime, TimeSpan transferTime)
    {
        if (maxCarryingCapacity == null) throw new ArgumentNullException(nameof(maxCarryingCapacity));
        if (maxCarryingCapacity.Value == 0)
            throw new ArgumentException("Max carrying capacity must be strictly positive.", nameof(maxCarryingCapacity));
        if (loadUnloadTime < TimeSpan.Zero)
            throw new ArgumentException("Load/unload time cannot be negative.", nameof(loadUnloadTime));
        if (transferTime < TimeSpan.Zero)
            throw new ArgumentException("Transfer time cannot be negative.", nameof(transferTime));
        
        MaxCarryingCapacity = maxCarryingCapacity;
        LoadUnloadTime = loadUnloadTime;
        TransferTime = transferTime;
    }
}
