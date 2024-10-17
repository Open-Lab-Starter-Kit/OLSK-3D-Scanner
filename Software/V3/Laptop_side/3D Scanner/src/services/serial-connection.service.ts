import { SerialPort } from 'serialport';
import { Constants } from 'src/constants';

export class SerialConnection {
  private isConnected: boolean;
  private serialConnection: SerialPort | null;
  private dataBuffer: string;

  constructor() {
    this.isConnected = false;
    this.serialConnection = null;
    this.dataBuffer = '';
  }

  // Test serial connection on different ports and baud rates if they exist
  async connectToSerial() {
    try {
      // Check available serial ports
      const availablePorts = await this.serialPorts();
      for (const port of availablePorts) {
        const serial = new SerialPort({
          path: port,
          baudRate: Constants.SERIAL_SETTINGS.BAUDRATE,
          autoOpen: true,
        });
        if (serial) {
          this.serialConnection = serial;
          this.isConnected = true;
          break; // Stop once connected
        }
      }
    } catch (e) {
      console.error('Serial connection error: ', e);
      setTimeout(() => {
        this.connectToSerial();
      }, 5000);
    }
  }

  async isConnectedToController(serial: SerialPort) {
    return new Promise((resolve) => {
      if (!serial) {
        return resolve(false);
      }

      // Remove any existing listeners to avoid duplications
      serial.removeAllListeners('data');

      // Set the timeout to resolve as false if no data is received within the specified time
      const timeout = setTimeout(() => {
        serial.removeAllListeners('data');
        resolve(false);
      }, Constants.SERIAL_SETTINGS.TIMEOUT);

      // Listen for data from the controller
      serial.on('data', (buffer: Buffer) => {
        clearTimeout(timeout);
        if (buffer.byteLength) {
          const data = buffer.toString('utf8').trim().replace('\r', '');
          if (data && data !== Constants.SCANNER_MESSAGES.PING) {
            this.dataBuffer += data + Constants.NEWLINE;
          }
          return resolve(true);
        } else {
          this.disconnectSerial();
          return resolve(false);
        }
      });
    });
  }

  // Check if the serial connection is established
  async isSerialConnected() {
    if (this.serialConnection) {
      const isConnectedToController = await this.isConnectedToController(
        this.serialConnection
      );
      if (isConnectedToController) {
        this.isConnected = true;
        return this.isConnected;
      }
    }
    this.disconnectSerial();
    return this.isConnected;
  }

  // Get list of serial ports
  async serialPorts() {
    try {
      const ports = await SerialPort.list();
      return ports.map((port) => port.path);
    } catch (err) {
      console.error('Error listing ports:', err);
      return [];
    }
  }

  // Write data to serial
  writeToSerial(data: string) {
    if (this.isConnected && this.serialConnection) {
      try {
        this.serialConnection.resume();
        this.serialConnection.write(data + Constants.NEWLINE);
      } catch (err) {
        console.error('Error writing to serial:', err);
        this.disconnectSerial();
      }
    }
  }

  setupListeners() {
    if (this.serialConnection) {
      this.serialConnection.removeAllListeners('data');
      this.serialConnection.on('data', (buffer: Buffer) => {
        const data = buffer.toString('utf8').trim().replace('\r', '');
        if (data && data !== Constants.SCANNER_MESSAGES.PING) {
          this.dataBuffer += data + Constants.NEWLINE;
        }
      });
    }
  }

  // Function to return and clear the buffered data
  readFromSerial() {
    if (this.dataBuffer) {
      const data = this.dataBuffer;
      this.dataBuffer = ''; // Clear the buffer after reading
      return data;
    }
    this.setupListeners();
    return null; // No data to return
  }

  // Close the serial connection
  disconnectSerial() {
    if (this.serialConnection?.isOpen) {
      this.serialConnection.removeAllListeners('close');
      this.serialConnection.close();
      this.serialConnection = null;

      console.log('Serial is disconnected');
    }
    this.isConnected = false;
  }
}

module.exports = SerialConnection;
