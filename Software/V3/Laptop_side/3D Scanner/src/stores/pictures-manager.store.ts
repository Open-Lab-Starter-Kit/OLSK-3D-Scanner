import { defineStore } from 'pinia';
import { Constants } from 'src/constants';

export const usePicturesManagerStore = defineStore('pictures', {
  state: () => ({
    snapShotsDirectory: null as string | null,
    streamElement: null as HTMLVideoElement | null,
    imageFormat: Constants.IMAGES_FORMATE[0].value as string,
  }),

  actions: {
    async pickDirectory() {
      this.snapShotsDirectory = await window.electronAPI.openFolderPicker();
    },
    async takeSnapShot() {
      if (this.streamElement?.srcObject) {
        const width = this.streamElement.offsetWidth,
          height = this.streamElement.offsetHeight;

        const canvas = document.createElement('canvas');
        canvas.width = width;
        canvas.height = height;

        const context = canvas.getContext('2d');
        context?.drawImage(this.streamElement, 0, 0, width, height);
        const mimeType =
          this.imageFormat === 'png' ? 'image/png' : 'image/jpeg';

        const imageURL = canvas.toDataURL(mimeType, 1);

        // save picture
        await this.saveSnapShotInDirectory(imageURL);
      }
    },
    async saveSnapShotInDirectory(imageURL: string) {
      if (this.snapShotsDirectory) {
        await window.electronAPI.saveSnapShot(
          imageURL,
          this.imageFormat,
          this.snapShotsDirectory
        );
      }
    },
  },
});
