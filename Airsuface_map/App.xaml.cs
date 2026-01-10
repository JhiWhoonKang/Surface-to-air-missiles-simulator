using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using Airsuface_map.Services;
using Airsuface_map.ViewModels;

namespace Airsuface_map
{
    public partial class App : Application
    {
        private TcpListener? _server;
        private CancellationTokenSource? _cts;
        public MainViewModel? _mainViewModel { get; private set; } = null!;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            _mainViewModel = new MainViewModel();

            // 서버 소켓 시작 (0.0.0.0:9001)
            _cts = new CancellationTokenSource();
            _server = new TcpListener(IPAddress.Any, 9001);
            _server.Start();
            Task.Run(() => AcceptClientsAsync(_cts.Token));
        }

        private async Task AcceptClientsAsync(CancellationToken token)
        {
            try
            {
                while (!token.IsCancellationRequested)
                {
                    var client = await _server!.AcceptTcpClientAsync(token);
                    _ = Task.Run(() => HandleClientAsync(client, token), token);
                }
            }
            catch (OperationCanceledException)
            {
                // 정상 종료
            }
        }

private async Task HandleClientAsync(TcpClient client, CancellationToken token)
{
    using (client)
    using (var stream = client.GetStream())
    {
        var buffer = new byte[4096];
        var recvBuffer = new List<byte>();

        while (!token.IsCancellationRequested)
        {
            int bytesRead = 0;
            try
            {
                bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length, token);
            }
            catch
            {
                break;
            }

            if (bytesRead == 0) break; // 연결 종료

            recvBuffer.AddRange(buffer.Take(bytesRead));

            // 누적 버퍼에서 메시지 추출
            while (true)
            {
                if (recvBuffer.Count < 5)
                    break; // 최소 헤더 미만

                if (recvBuffer[0] != 0x51)
                {
                    // 싱크 맞추기
                    recvBuffer.RemoveAt(0);
                    continue;
                }

                // 길이 추출
                int payloadLength = BitConverter.ToInt32(recvBuffer.Skip(1).Take(4).ToArray(), 0);
                int totalLength = 1 + 4 + payloadLength;

                if (recvBuffer.Count < totalLength)
                    break; // 아직 도착 안 함

                // radar/lc/ls flag 체크 (payload 시작 직후)
                if (payloadLength < 3 || recvBuffer[5] != 0x01 || recvBuffer[6] != 0x01 || recvBuffer[7] != 0x01)
                {
                    // 잘못된 메시지 삭제
                    recvBuffer.RemoveRange(0, totalLength);
                    continue;
                }

                // 유효 메시지 추출
                byte[] message = recvBuffer.Take(totalLength).ToArray();

                // ViewModel에 전달 (UI 스레드에서)
                Application.Current.Dispatcher.Invoke(() =>
                {
                    _mainViewModel?.OnDataReceived(message);
                });

                recvBuffer.RemoveRange(0, totalLength);
            }
        }
    }
}

        protected override void OnExit(ExitEventArgs e)
        {
            _cts?.Cancel();
            _server?.Stop();
            base.OnExit(e);
        }
    }
}