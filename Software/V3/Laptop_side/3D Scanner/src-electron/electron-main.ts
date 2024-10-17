import { app, BaseWindow, BrowserWindow, dialog, ipcMain } from 'electron';
import path from 'path';
import os from 'os';
import fs from 'fs';
import { SerialConnection } from 'src/services/serial-connection.service';
// needed in case process is undefined under Linux
const platform = process.platform || os.platform();

let mainWindow: BrowserWindow | undefined;

const serialConn = new SerialConnection();

function createWindow() {
  /**
   * Initial window options
   */
  mainWindow = new BrowserWindow({
    icon: path.resolve(__dirname, 'icons/icon.png'), // tray icon
    width: 1000,
    height: 600,
    fullscreen: true,
    useContentSize: true,
    webPreferences: {
      nodeIntegrationInWorker: true,
      contextIsolation: true,
      // More info: https://v2.quasar.d/ev/quasar-cli-vite/developing-electron-apps/electron-preload-script
      preload: path.resolve(__dirname, process.env.QUASAR_ELECTRON_PRELOAD),
    },
  });

  mainWindow.loadURL(process.env.APP_URL);

  if (process.env.DEBUGGING) {
    // if on DEV or Production with debug enabled
    mainWindow.webContents.openDevTools();
  } else {
    // we're on production; no access to devtools pls
    mainWindow.webContents.on('devtools-opened', () => {
      mainWindow?.webContents.closeDevTools();
    });
  }

  mainWindow.on('closed', () => {
    mainWindow = undefined;
  });
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', async () => {
  if (mainWindow === undefined) {
    createWindow();
  }
});

// Connect to serial port when requested
ipcMain.handle('connect-serial', async () => {
  await serialConn.connectToSerial();
});

ipcMain.handle('is-serial-connected', async () => {
  return await serialConn.isSerialConnected();
});

// Write data to serial
ipcMain.handle('write-serial', async (event, data: string) => {
  serialConn.writeToSerial(data);
});

// Read data from serial
ipcMain.handle('read-serial', async () => {
  const data = serialConn.readFromSerial();
  return data;
});

// Disconnect serial connection
ipcMain.handle('disconnect-serial', () => {
  serialConn.disconnectSerial();
});

// Handle folder selection request
ipcMain.handle('dialog:openFolder', async () => {
  const result = await dialog.showOpenDialog(mainWindow as BaseWindow, {
    properties: ['openDirectory'],
  });
  if (result.canceled) {
    return null;
  } else {
    return result.filePaths[0]; // Return the selected folder path
  }
});

// Handle save image in specific folder
ipcMain.handle('save-snapshot', (event, imageURL, folderPath) => {
  const base64Data = imageURL.replace(/^data:image\/png;base64,/, '');

  // Ensure the folder exists
  if (!fs.existsSync(folderPath)) {
    return console.error('Directory does not exist:', folderPath);
  }

  // Create a unique file name for the image
  const fileName = `snapshot_${Date.now()}.png`;
  const filePath = path.join(folderPath, fileName);

  // Save the image to the specified folder
  fs.writeFile(filePath, base64Data, 'base64', (err) => {
    if (err) {
      console.error('Error saving the snapshot:', err);
    }
  });
});
