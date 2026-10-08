using System;

namespace Lab1.Models;

public struct Coordinates
{
    public double Latitude { get; }
    public double Longitude { get; }

    public Coordinates(double latitude, double longitude)
    {
        Latitude = latitude;
        Longitude = longitude;
    }

    public Distance DistanceTo(Coordinates other)
    {
        double dLat = (other.Latitude - Latitude) * 111000;
        double dLon = (other.Longitude - Longitude) * 111000;
        return new Distance(Math.Sqrt(dLat * dLat + dLon * dLon));
    }
}
