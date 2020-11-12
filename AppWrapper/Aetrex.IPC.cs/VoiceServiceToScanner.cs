using Newtonsoft.Json;
using System;
using System.IO;
using System.IO.Pipes;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    internal class VoiceServiceToScanner: IPCBase
    {
        
        AutoResetEvent commandReadyToTransmit = new AutoResetEvent(false);

        public VoiceServiceToScanner(string pipeName): base(pipeName)
        {                        
            
        }

        protected override void CommunicationThread()
        {
            NamedPipeServerStream pipeServer = new NamedPipeServerStream(
                this.pipeName, 
                PipeDirection.InOut, 
                1, 
                PipeTransmissionMode.Message, 
                PipeOptions.Asynchronous);

            Task.Run(async () => {

                int threadId = Thread.CurrentThread.ManagedThreadId;

                Console.WriteLine($"VoiceServiceToScanner.CommunicationThread() waiting for connection threadId:{Task.CurrentId.ToString()}");

                // Wait for a client to connect
                pipeServer.WaitForConnection();

                Console.WriteLine($"VoiceServiceToScanner.CommunicationThread() Voice service connected ");
                while (!cancellationToken.IsCancellationRequested)
                {
                    try
                    {
                        // commandReadyToTransmit.WaitOne();
                        
                        StreamString ss = new StreamString(pipeServer);

                        Console.WriteLine($"Waiting for a message from voice service");
                        string messageFromVoice = await ss.ReadStringAsync(1000, cancellationToken);
                        if (!String.IsNullOrWhiteSpace(messageFromVoice))
                        {
                            Console.WriteLine(messageFromVoice);
                        }
                        else
                        {
                            Console.WriteLine("VoiceServiceToScanner.CommunicationThread() No message from voice service");
                        }
                        

                        //Console.WriteLine($"About to send a message to voice service");
                        //await ss.WriteStringFixedAsync(command);
                        //Console.WriteLine($"Sent message to voice service");
                    }
                    // Catch the IOException that is raised if the pipe is broken
                    // or disconnected.
                    catch (IOException e)
                    {
                        Console.WriteLine("ERROR: {0}", e.Message);
                    }

                    Thread.Sleep(10000);
                }

                Console.WriteLine("VoiceServiceToScanner closing the pipe");
                pipeServer.Close();
                Console.WriteLine("VoiceServiceToScanner pipe is closed");

            }, cancellationToken);
        }
    }
}
