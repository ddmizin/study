using System;
using System.Collections.Generic;
using System.Linq;

namespace Lab1.Models;

public class Warehouse
{
    public Guid Id { get; }
    public Coordinates Coordinates { get; }
    public VolumeWeight Capacity { get; }
    
    private readonly Dictionary<Sku, int> _stock;
    public IReadOnlyDictionary<Sku, int> Stock => _stock;
    
    private readonly List<Employee> _staff;
    public IReadOnlyList<Employee> Staff => _staff;

    public Warehouse(Guid id, Coordinates coordinates, VolumeWeight capacity, IEnumerable<Employee> staff)
    {
        if (capacity == null) throw new ArgumentNullException(nameof(capacity));
        if (staff == null) throw new ArgumentNullException(nameof(staff));

        Id = id;
        Coordinates = coordinates;
        Capacity = capacity;
        _staff = new List<Employee>(staff);
        
        if (_staff.Count == 0)
            throw new ArgumentException("Warehouse must have at least one employee.", nameof(staff));

        _stock = new Dictionary<Sku, int>();
    }

    public void Accept(IReadOnlyDictionary<Sku, int> items)
    {
        if (items == null) throw new ArgumentNullException(nameof(items));

        VolumeWeight itemsVolumeWeight = CalculateVolumeWeight(items);
        VolumeWeight currentVolumeWeight = CalculateVolumeWeight(_stock);

        if (currentVolumeWeight + itemsVolumeWeight > Capacity)
            throw new InvalidOperationException("Not enough capacity in the warehouse.");

        foreach (var item in items)
        {
            if (_stock.ContainsKey(item.Key))
                _stock[item.Key] += item.Value;
            else
                _stock[item.Key] = item.Value;
        }
    }

    public void Dispatch(IReadOnlyDictionary<Sku, int> items)
    {
        if (items == null) throw new ArgumentNullException(nameof(items));

        foreach (var item in items)
        {
            if (!_stock.ContainsKey(item.Key) || _stock[item.Key] < item.Value)
                throw new InvalidOperationException($"Not enough '{item.Key.Name}' in the warehouse.");
        }

        foreach (var item in items)
        {
            _stock[item.Key] -= item.Value;
            if (_stock[item.Key] == 0)
                _stock.Remove(item.Key);
        }
    }

    public TimeSpan CalculateAcceptanceTime(VolumeWeight totalVolumeWeight)
    {
        return CalculateOperationTime(totalVolumeWeight, isAcceptance: true);
    }
    
    public TimeSpan CalculateDispatchTime(VolumeWeight totalVolumeWeight)
    {
        return CalculateOperationTime(totalVolumeWeight, isAcceptance: false);
    }

    private TimeSpan CalculateOperationTime(VolumeWeight totalVolumeWeight, bool isAcceptance)
    {
        VolumeWeight remainingOvh = totalVolumeWeight;
        var employeeAvailability = _staff.Select(e => new EmployeeState(e, TimeSpan.Zero)).ToList();
        TimeSpan truckReleaseTime = TimeSpan.Zero;

        while (remainingOvh > VolumeWeight.Zero)
        {
            var availableEmployeeState = employeeAvailability.OrderBy(e => e.AvailableAt).First();
            VolumeWeight taken = VolumeWeight.Min(availableEmployeeState.Employee.MaxCarryingCapacity, remainingOvh);
            remainingOvh -= taken;

            TimeSpan tripStartTime = availableEmployeeState.AvailableAt;

            if (isAcceptance)
            {
                TimeSpan unloadDoneTime = tripStartTime + availableEmployeeState.Employee.LoadUnloadTime;
                TimeSpan employeeDoneTime = unloadDoneTime + availableEmployeeState.Employee.TransferTime;

                availableEmployeeState.AvailableAt = employeeDoneTime;
                truckReleaseTime = TimeSpan.FromSeconds(Math.Max(truckReleaseTime.TotalSeconds, unloadDoneTime.TotalSeconds));
            }
            else
            {
                TimeSpan loadDoneTime = tripStartTime + availableEmployeeState.Employee.TransferTime + availableEmployeeState.Employee.LoadUnloadTime;
                
                availableEmployeeState.AvailableAt = loadDoneTime;
                truckReleaseTime = TimeSpan.FromSeconds(Math.Max(truckReleaseTime.TotalSeconds, loadDoneTime.TotalSeconds));
            }
        }

        return truckReleaseTime;
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

    private class EmployeeState
    {
        public Employee Employee { get; }
        public TimeSpan AvailableAt { get; set; }

        public EmployeeState(Employee employee, TimeSpan availableAt)
        {
            Employee = employee;
            AvailableAt = availableAt;
        }
    }
}
