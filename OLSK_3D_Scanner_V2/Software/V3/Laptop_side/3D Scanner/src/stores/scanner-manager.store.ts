import { defineStore } from 'pinia';
import { Constants } from 'src/constants';
import {
  LEDMetrics,
  RotationSettings,
  ScannerSettings,
} from 'src/interfaces/scanner-manager.interface';
import { usePicturesManagerStore } from './pictures-manager.store';
import { getColorValues } from 'src/services/scanner-settings.service';

export const useScannerManagerStore = defineStore('scanner', {
  state: () => ({
    scannerStatus: Constants.SCANNER_STATUS.DISCONNECT,
    scannerSettings: {
      ledMetrics: {
        red: 100 as number,
        blue: 100 as number,
        green: 100 as number,
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
    setupSideLEDColor(color: string, isLedOn: boolean) {
      const { red, green, blue, intensity } = getColorValues(color);
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.SIDES_LED_COLOR_COMMAND}=${
          isLedOn ? 1 : 0
        }, ${red}, ${green}, ${blue}, ${intensity}`
      );
    },
    setupRingLEDColor(color: string, isLedOn: boolean) {
      const { red, green, blue, intensity } = getColorValues(color);
      window.electronAPI.sendDataToSerial(
        `${Constants.SCANNER_COMMANDS.RING_LED_COLOR_COMMAND}=${
          isLedOn ? 1 : 0
        }, ${red}, ${green}, ${blue}, ${intensity}`
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
          200
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
