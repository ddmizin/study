using System;

namespace Lab1.Models;

public record VolumeWeight
{
    public double Value { get; }

    public VolumeWeight(double value)
    {
        if (value < 0)
            throw new ArgumentException("Volume weight cannot be negative.", nameof(value));
        Value = value;
    }

    public static VolumeWeight operator +(VolumeWeight left, VolumeWeight right) => new(left.Value + right.Value);
    public static VolumeWeight operator -(VolumeWeight left, VolumeWeight right) => new(Math.Max(0, left.Value - right.Value));
    public static VolumeWeight operator *(VolumeWeight left, int multiplier)
    {
        if (multiplier < 0) throw new ArgumentException("Multiplier cannot be negative.", nameof(multiplier));
        return new VolumeWeight(left.Value * multiplier);
    }

    public static bool operator >(VolumeWeight left, VolumeWeight right) => left.Value > right.Value;
    public static bool operator <(VolumeWeight left, VolumeWeight right) => left.Value < right.Value;
    public static bool operator >=(VolumeWeight left, VolumeWeight right) => left.Value >= right.Value;
    public static bool operator <=(VolumeWeight left, VolumeWeight right) => left.Value <= right.Value;

    public static VolumeWeight Min(VolumeWeight left, VolumeWeight right) => left.Value < right.Value ? left : right;

    public static VolumeWeight Zero { get; } = new(0);
}
