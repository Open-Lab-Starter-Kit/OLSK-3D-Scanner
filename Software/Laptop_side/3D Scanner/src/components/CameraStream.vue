<template>
  <div class="column q-gutter-y-sm items-start full-width">
    <span class="row text-size">Camera Stream:</span>

    <video
      ref="streamElement"
      class="row full-width camera-stream"
      autoplay
    ></video>
  </div>
</template>

<script setup lang="ts">
import { storeToRefs } from 'pinia';
import { usePicturesManagerStore } from 'src/stores/pictures-manager.store';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { onMounted } from 'vue';
import { useQuasar } from 'quasar';

const $q = useQuasar();
const photosManagerStore = usePicturesManagerStore();
const scannerManagerStore = useScannerManagerStore();

const { streamElement } = storeToRefs(photosManagerStore);

// Track for current event listeners to remove them later
let videoTrack: MediaStreamTrack | null = null;
let endedHandler: (() => void) | null = null;

// Function to start camera stream
const startCameraStream = async () => {
  try {
    const stream = await navigator.mediaDevices.getUserMedia({
      video: {
        width: { ideal: 4656 },
        height: { ideal: 3490 },
      },
    });

    if (streamElement.value) {
      streamElement.value.srcObject = stream;

      // Get the video track
      videoTrack = stream.getVideoTracks()[0];

      // Define event handlers
      endedHandler = handleCameraDisconnect;

      // Listen for camera disconnect or stream ending
      videoTrack.addEventListener('ended', endedHandler);
    }
  } catch (error) {
    console.error('Camera Error: ', error);
    noCameraFoundDialog();
  }
};

// Function to handle camera disconnection
const handleCameraDisconnect = () => {
  // Stop the scanner
  scannerManagerStore.stopScanning();
  // Clean up event listeners before restarting the stream
  if (videoTrack && endedHandler) {
    videoTrack.removeEventListener('ended', endedHandler);
    // Show dialog and attempt to restart the stream
    noCameraFoundDialog();
  }
};

// Dialog to show when no camera is found
const noCameraFoundDialog = () => {
  $q.dialog({
    title: 'Camera not found',
    message: 'Please ensure the camera is connected.',
    ok: {
      label: 'Retry',
      color: 'brown-3',
    },
    persistent: true,
  }).onOk(() => startCameraStream());
};

// Start the camera stream on mount
onMounted(() => {
  startCameraStream();
});
</script>

<style scoped>
.text-size {
  font-size: 1.5rem;
  font-weight: bold;
  color: #041939;
}
.camera-stream {
  border-radius: 10px;
  border: 2px solid white;
}
</style>
