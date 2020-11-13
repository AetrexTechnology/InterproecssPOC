using Newtonsoft.Json;
using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Aetrex.Voice.AppCS
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
                PipeDirection.InOut,
                PipeOptions.Asynchronous,
                TokenImpersonationLevel.Impersonation);

            StreamString ss = new StreamString(pipeClient);

            Task.Run(async () =>
            {
                Console.WriteLine($"ScannerToVoiceService Connecting to voice service as a pipe client.");
                pipeClient.Connect();
                Console.WriteLine("ScannerToVoiceService Connected to voice service...\n");
                
                while (!cancellationToken.IsCancellationRequested)
                {
                    commandReadyToTransmit.WaitOne();

                    //Responds to the service                    
                    Console.WriteLine("\nPosting a message to voice service");
                    await ss.WriteStringFixedAsync(command, cancellationToken);                    
                    Console.WriteLine("Message to voice service posted");
                }

                pipeClient.Close();

            }, cancellationToken);            
        }        
    }    
}