using Newtonsoft.Json;
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
    internal class ScannerToVoiceService : IPCBase
    {
        private NamedPipeClientStream pipeClient;
        
        AutoResetEvent commandReadyToTransmit = new AutoResetEvent(false);
        string command;

        public ScannerToVoiceService(string pipeName): base(pipeName)
        {
        }

        public void SetMicrophoneIndex(int micIndex)
        {
            //std::string tmp = "{\"time\": \"" + dateTime + "\", \"instruction\" : \"" + instruction + "\", \"microphoneIndex\" : " + std::to_string(microphoneIndex) + "}";
            var commandObj = new
            {
                time = DateTime.UtcNow.ToString("s", System.Globalization.CultureInfo.InvariantCulture), //Time in ISO 8601
                instruction = "SetAudioCaptureDeviceIndex",
                microphoneIndex = micIndex
            };

            command = JsonConvert.SerializeObject(commandObj);
            commandReadyToTransmit.Set();
        }

        protected override void CommunicationThread()
        {
            pipeClient = new NamedPipeClientStream(
                ".",
                pipeName,
                PipeDirection.In,
                PipeOptions.Asynchronous,
                TokenImpersonationLevel.Impersonation);

            StreamString ss = new StreamString(pipeClient);

            Task.Run(async () =>
            {
                Console.WriteLine($"ScannerToVoiceService Connecting to service as a pipe client. threadId:{Task.CurrentId.ToString()}");
                pipeClient.Connect();
                Console.WriteLine("ScannerToVoiceService Connected to service...\n");


                // polling boolean property
                while (!cancellationToken.IsCancellationRequested)
                {
                    commandReadyToTransmit.WaitOne();

                    //Responds to the service                    
                    Console.WriteLine("Posting a message to server");
                    await ss.WriteStringFixedAsync(command, cancellationToken);                    
                    Console.WriteLine("Message to server posted");

                    // Give the client process some time to display results before exiting.
                    //Thread.Sleep(2000);
                }

                pipeClient.Close();

            }, cancellationToken);

            // Task server = FetchMessageFromServer();

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
        /*
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
        */
        /*
        private Task PostMessageToService()
        {
            StreamString ss = new StreamString(pipeClient);
            Random random = new Random();

            return Task.Run(async () =>
            {
                int iMessageIndex = 1;
                // polling boolean property
                while (!cancellationToken.IsCancellationRequested)
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
            }, cancellationToken);
        }
        */        
    }    
}