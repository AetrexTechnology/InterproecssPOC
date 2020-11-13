using System.Threading;

namespace Aetrex.Voice.AppCS
{
    /// <summary>
    /// Inter-process communication interface and base class
    /// </summary>

    public interface IIPC
    {
        void Close();
    }

    abstract class IPCBase : IIPC
    {
        protected CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();
        protected CancellationToken cancellationToken;
        protected string pipeName;

        public IPCBase(string pipeName)
        {
            cancellationToken = cancellationTokenSource.Token;
            this.pipeName = pipeName;

            CommunicationThread();
        }

        protected abstract void CommunicationThread();
        public void Close()
        {
            cancellationTokenSource.Cancel();
        }
    }
}
