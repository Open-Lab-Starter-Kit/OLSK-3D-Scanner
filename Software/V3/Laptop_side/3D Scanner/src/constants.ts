export const Constants = {
  SERIAL_SETTINGS: {
    BAUDRATE: 115200,
    TIMEOUT: 500, // ms
  } as Record<string, number>,
  GRBL_COMMAND_STATUS: '?' as string,
  NEWLINE: '\n' as string,
  SCANNER_COMMANDS: {
    LED_COLOR_COMMAND: 'RGB',
    START_SCANNING_COMMAND: 'START',
    PAUSE_SCANNING_COMMAND: 'PAUSE',
    RESUME_SCANNING_COMMAND: 'RESUME',
    STOP_SCANNING_COMMAND: 'STOP',
    ROTATION_SPEED: 'SPEED',
    ROTATION_STEPS: 'STOPS_ROTATION',
    ARC_STEPS: 'PICTURES_ARC',
  },
  SCANNER_MESSAGES: {
    TAKE_PICTURE: 'TAKE_PICTURE',
    END_OF_JOB: 'JOB_END',
    START_OF_JOB: 'STARTED',
    PAUSE_OF_JOB: 'PAUSED',
    RESUME_OF_JOB: 'RESUMED',
    PING: 'PING',
  },
  SCANNER_STATUS: {
    READY: 'READY',
    PAUSE: 'PAUSE',
    RUN: 'RUN',
    SNAPSHOT: 'SNAPSHOT',
    DISCONNECT: 'DISCONNECTED',
  },
  IMAGES_FORMATE: [
    { label: 'PNG', value: 'png' },
    { label: 'JPG', value: 'jpg' },
    { label: 'JPEG', value: 'jpeg' },
  ],
};
