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
            if (args.Length > 0 && args[0] == "server")
            {
                IPCServer server = new IPCServer();
            } 
            else
            {
                IPCClient client = new IPCClient(true);                    
            }            

            Console.WriteLine("Press any key to exit");
            Console.ReadKey();
        }        

    }
}
