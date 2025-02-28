import { defineStore } from 'pinia';
import { Constants } from 'src/constants';
import {
  LEDMetrics,
  RotationSettings,
  ScannerSettings,
} from 'src/interfaces/scanner-manager.interface';
import { usePicturesManagerStore } from './pictures-manager.store';

export const useScannerManagerStore = defineStore('scanner', {
  state: () => ({
    scannerStatus: Constants.SCANNER_STATUS.DISCONNECT,
    scannerSettings: {
      ledMetrics: {
        red: 255 as number,
        blue: 255 as number,
        green: 255 as number,
        intensity: 1 as number,
      } as LEDMetrics,
      rotationSettings: {
        steps: 6 as number,
        speed: 500 as number,
      } as RotationSettings,
      arcSteps: 2 as number,
    } as ScannerSettings,
    picturesManagerStore: usePicturesManagerStore(),
  }),
  actions: {
    startScanning() {
      this.setupScannerSettings();
      window.electronAPI.sendDataToSerial(
        Constants.SCANNER_COMMANDS.START_SCANNING_COMMAND
      );
    },
    pauseScanning() {
      window.electronAPI.sendDataToSerial(
        Constants.SCANNER_COMMANDS.PAUSE_SCANNING_COMMAND
      );
    },
    resumeScanning() {
      window.electronAPI.sendDataToSerial(
        Constants.SCANNER_COMMANDS.RESUME_SCANNING_COMMAND
      );
    },
    stopScanning() {
      window.electronAPI.sendDataToSerial(
        Constants.SCANNER_COMMANDS.STOP_SCANNING_COMMAND
      );
    },
    setupLEDColor() {
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.LED_COLOR_COMMAND}=${this.scannerSettings.ledMetrics.red}, ${this.scannerSettings.ledMetrics.green}, ${this.scannerSettings.ledMetrics.blue}, ${this.scannerSettings.ledMetrics.intensity}`
      );
    },
    setupScannerSettings() {
      // Rotation
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.ROTATION_SPEED}=${this.scannerSettings.rotationSettings.speed}`
      );
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.ROTATION_STEPS}=${
          this.scannerSettings.rotationSettings.steps * 2
        }`
      );

      // arc
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.ARC_STEPS}=${this.scannerSettings.arcSteps}`
      );
    },
    async scannerTakePicture() {
      this.picturesManagerStore.takeSnapShot().then(() => {
        window.electronAPI.sendDataToSerial(
          Constants.SCANNER_COMMANDS.RESUME_SCANNING_COMMAND
        );
        setTimeout(
          () => (this.scannerStatus = Constants.SCANNER_STATUS.RUN),
          100
        );
      });
    },

    async handleDataFromScanner(data: string) {
      if (data === Constants.SCANNER_MESSAGES.TAKE_PICTURE) {
        this.scannerStatus = Constants.SCANNER_STATUS.SNAPSHOT;
        await this.scannerTakePicture();
      } else if (data === Constants.SCANNER_MESSAGES.END_OF_JOB) {
        this.scannerStatus = Constants.SCANNER_STATUS.READY;
      } else if (data === Constants.SCANNER_MESSAGES.START_OF_JOB) {
        this.scannerStatus = Constants.SCANNER_STATUS.RUN;
      } else if (data === Constants.SCANNER_MESSAGES.PAUSE_OF_JOB) {
        this.scannerStatus = Constants.SCANNER_STATUS.PAUSE;
      } else if (data === Constants.SCANNER_MESSAGES.RESUME_OF_JOB) {
        this.scannerStatus = Constants.SCANNER_STATUS.RUN;
      }
    },
  },
});
