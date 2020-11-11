using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace Aetrex.IPC.cs
{
    class Program
    {
        static void Main(string[] args)
        {
            //Create a Windows job object, register this process to the job so on job termination, this process and its child processes will terminate by Windows OS
            JobManagement.Job job = new JobManagement.Job();
            job.AddProcess(Process.GetCurrentProcess().Id);


            const string IPCServer = @"D:\Aetrex\Interprocess\AppWrapper\x64\Debug\AppWrapper.exe";
            Process IPCServerProcess = IPCServerProcess = new Process();
            IPCServerProcess.StartInfo.FileName = IPCServer;
            IPCServerProcess.StartInfo.WindowStyle = ProcessWindowStyle.Normal;
            IPCServerProcess.Start();

            Console.WriteLine($"Program.Main() threadId:{Task.CurrentId.ToString()}");

            const string pipeNameVoiceToScanner = "AetrexScannerOS2VoiceActivation";
            VoiceServiceToScanner voiceToScanner = new VoiceServiceToScanner(pipeNameVoiceToScanner);

            //const string pipeNameVoiceToScanner = "eNovaClient";
            //VoiceProcessToScanner voiceToScanner = new VoiceProcessToScanner(pipeNameVoiceToScanner);
            //voiceToScanner.ConnectToService();

            //scannerToVoice.SetMicrophoneIndex(0);

            Console.WriteLine("Press any key to exit");
            Console.ReadKey();
            
            if (voiceToScanner != null)
            {
                Console.WriteLine("Closing the scannerToVoice");
                voiceToScanner.Close();
            }
            
            //if (voiceToScanner != null)
            //{
            //    Console.WriteLine("Closing the connection to the service");
            //    voiceToScanner.Close();
            //}
        }        

    }
}
