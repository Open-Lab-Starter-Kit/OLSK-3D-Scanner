import { Constants } from 'src/constants';

addEventListener('message', async (event) => {
  if (event.data.start) {
    let isConnected = false;
    while (true) {
      // Request connection status from the main thread
      postMessage({ action: 'check-connection' });

      // Wait for the main thread to respond
      await new Promise<void>((resolve) => {
        const handleResponse = (responseEvent: MessageEvent) => {
          if (responseEvent.data.action === 'connection-status') {
            isConnected = responseEvent.data.isConnected;
            removeEventListener('message', handleResponse);
            resolve();
          }
        };
        addEventListener('message', handleResponse);
      });

      if (!isConnected) {
        // Request serial connection from the main thread
        postMessage({ action: 'connect-serial' });

        // Wait for the serial connection response
        await new Promise<void>((resolve) => {
          const handleResponse = (responseEvent: MessageEvent) => {
            if (responseEvent.data.action === 'serial-connected') {
              removeEventListener('message', handleResponse);
              resolve();
            }
          };
          addEventListener('message', handleResponse);
        });
      } else {
        // check if there is data in the serial to read
        postMessage({ action: 'read-from-serial' });
      }
      // Wait for 1 second before checking again
      await new Promise((resolve) =>
        setTimeout(resolve, Constants.SERIAL_SETTINGS.TIMEOUT)
      );
    }
  }
});
