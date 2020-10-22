using System;
using System.Collections.Generic;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    class Program
    {
        static void Main(string[] args)
        {
            IPCClient client = null;

            if (args.Length > 0 && args[0] == "server")
            {
                IPCServer server = new IPCServer();
            } 
            else
            {
                client = new IPCClient();
                client.CommunicateWithService();
            }            

            Console.WriteLine("Press any key to exit");
            Console.ReadKey();
            
            if (client != null)
            {
                Console.WriteLine("Closing the connection to the service");
                client.Close();
            }
        }        

    }
}
