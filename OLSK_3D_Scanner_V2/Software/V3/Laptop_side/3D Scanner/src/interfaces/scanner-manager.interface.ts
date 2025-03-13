export interface ScannerSettings {
  ledMetrics: LEDMetrics;
  rotationSettings: RotationSettings;
  arcSteps: number;
}

export interface LEDMetrics {
  red: number;
  blue: number;
  green: number;
  intensity: number;
}

export interface RotationSettings {
  steps: number;
  speed: number;
}
