import { defineStore } from 'pinia';
import { getSerialConnectionWorker } from 'src/workers';
import { useScannerManagerStore } from './scanner-manager.store';
import { Constants } from 'src/constants';

export const useSerialConnectionStore = defineStore('connection', {
  state: () => ({
    isConnected: false as boolean,
    scannerManagerStore: useScannerManagerStore(),
  }),
  actions: {
    async startSerialConnection() {
      const worker = getSerialConnectionWorker();
      worker.postMessage({
        start: true,
      });
      this.listenToWorkerResponse(worker);
    },

    listenToWorkerResponse(worker: Worker) {
      worker.addEventListener('message', async (e) => {
        const action = e.data.action;

        if (action === 'check-connection') {
          // Check the connection status using electronAPI
          this.isConnected = await window.electronAPI.isSerialConnected();

          // change the status of the scanner
          if (
            this.isConnected &&
            this.scannerManagerStore.scannerStatus ===
              Constants.SCANNER_STATUS.DISCONNECT
          ) {
            this.scannerManagerStore.scannerStatus =
              Constants.SCANNER_STATUS.READY;
          } else if (!this.isConnected) {
            this.scannerManagerStore.scannerStatus =
              Constants.SCANNER_STATUS.DISCONNECT;
          }

          // Send the result back to the worker
          worker.postMessage({
            action: 'connection-status',
            isConnected: this.isConnected,
          });
        }

        if (action === 'connect-serial') {
          // Attempt to reconnect the serial device
          await window.electronAPI.connectSerial();

          // Inform the worker about the connection attempt result
          worker.postMessage({
            action: 'serial-connected',
          });
        }

        if (action === 'read-from-serial') {
          const data = await window.electronAPI.receiveDataFromSerial();
          if (data) {
            data
              .split(Constants.NEWLINE)
              .forEach((message) =>
                this.scannerManagerStore.handleDataFromScanner(message)
              );
          }
        }
      });
    },
  },
});
