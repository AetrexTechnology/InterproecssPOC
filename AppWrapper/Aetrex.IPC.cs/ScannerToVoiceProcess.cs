using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    internal class ScannerToVoiceService
    {
        private CancellationTokenSource tokenSource;
        private NamedPipeClientStream pipeClient;
        private CancellationToken token;

        public ScannerToVoiceService(string pipeName)
        {
            tokenSource = new CancellationTokenSource();
            token = tokenSource.Token;

            pipeClient = new NamedPipeClientStream(
                ".",
                pipeName,
                PipeDirection.In, 
                PipeOptions.Asynchronous,
                TokenImpersonationLevel.Impersonation);
        }

        public void ConnectToService()
        {            
            Console.WriteLine($"Connecting to service. threadId:{Task.CurrentId.ToString()}");
            pipeClient.Connect();
            Console.WriteLine("Connected to service...\n");

            Task server = FetchMessageFromServer();

            // Task client = PostMessageToService();

            //Task.Run(() =>
            //{
            //    Task server = FetchMessageFromServer();

            //    Task client = PostMessageToService();

            //    Task.WaitAll(server, client);

            //    // release resources and exit
            //    pipeClient.Close();
            //});

            /*
            StreamString ss = new StreamString(pipeClient);

            Task.Run(() =>
            {
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    //token.ThrowIfCancellationRequested(); // throw OperationCancelledException if requested

                    //Wait for a message from the service
                    Console.WriteLine(ss.ReadString(64));

                    //Responds to the service
                    ss.WriteStringFixed("Message from client");                    
                }
                // release resources and exit
                pipeClient.Close();
                // Give the client process some time to display results before exiting.
                Thread.Sleep(4000);

            }, token);
            */
        }

        private Task FetchMessageFromServer()
        {
            StreamString ss = new StreamString(pipeClient);

            return Task.Run(async () =>
            {
                Console.WriteLine($"VoiceProcessToScanner.FetchMessageFromServer() threadId:{Task.CurrentId.ToString()}");
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    Console.WriteLine("Fetching a message from voice service");
                    //Wait for a message from the service
                    Console.WriteLine(await ss.ReadStringAsync(1000));
                    Console.WriteLine("Message from voice service arrived");

                    Thread.Sleep(4000);
                }
                Console.WriteLine("FetchMessageFromServer exiting");
            }, token);

            
        }
        /*
        private Task PostMessageToService()
        {
            StreamString ss = new StreamString(pipeClient);
            Random random = new Random();

            return Task.Run(async () =>
            {
                int iMessageIndex = 1;
                // polling boolean property
                while (!token.IsCancellationRequested)
                {
                    //Responds to the service                    
                    Console.WriteLine("Posting a message to server");
                    await ss.WriteStringFixedAsync($"Message from client #{iMessageIndex}");
                    iMessageIndex++;
                    Console.WriteLine("Message to server posted");

                    // Give the client process some time to display results before exiting.
                    int randomWait = random.Next(1, 4) * 1000;
                    Thread.Sleep(2000);
                }
            }, token);
        }
        */

        public void Close()
        {
            tokenSource.Cancel();
        }
    }    
}