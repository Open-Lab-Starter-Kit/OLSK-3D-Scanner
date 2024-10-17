/**
 * This file is used specifically for security reasons.
 * Here you can access Nodejs stuff and inject functionality into
 * the renderer thread (accessible there through the "window" object)
 *
 * WARNING!
 * If you import anything from node_modules, then make sure that the package is specified
 * in package.json > dependencies and NOT in devDependencies
 *
 * Example (injects window.myAPI.doAThing() into renderer thread):
 *
 *   import { contextBridge } from 'electron'
 *
 *   contextBridge.exposeInMainWorld('myAPI', {
 *     doAThing: () => {}
 *   })
 *
 * WARNING!
 * If accessing Node functionality (like importing @electron/remote) then in your
 * electron-main.ts you will need to set the following when you instantiate BrowserWindow:
 *
 * mainWindow = new BrowserWindow({
 *   // ...
 *   webPreferences: {
 *     // ...
 *     sandbox: false // <-- to be able to import @electron/remote in preload script
 *   }
 * }
 */
import { contextBridge, ipcRenderer } from 'electron';

declare global {
  interface Window {
    electronAPI: {
      isSerialConnected: () => Promise<boolean>;
      connectSerial: () => Promise<void>;
      saveSnapShot: (imageURL: string, filePath: string) => Promise<void>;
      openFolderPicker: () => Promise<string>;
      sendDataToSerial: (data: string) => void;
      receiveDataFromSerial: () => Promise<string | null>;
    };
  }
}

contextBridge.exposeInMainWorld('electronAPI', {
  isSerialConnected: async () =>
    await ipcRenderer.invoke('is-serial-connected'),
  connectSerial: async () => await ipcRenderer.invoke('connect-serial'),
  saveSnapShot: (imageURL: string, filePath: string) =>
    ipcRenderer.invoke('save-snapshot', imageURL, filePath),
  openFolderPicker: () => ipcRenderer.invoke('dialog:openFolder'),
  sendDataToSerial: (data: string) => ipcRenderer.invoke('write-serial', data),
  receiveDataFromSerial: () => ipcRenderer.invoke('read-serial'),
});
