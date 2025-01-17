<template>
  <div class="row justify-evenly full-width">
    <q-btn
      round
      color="white"
      text-color="black"
      :icon="startButtonStatus?.icon"
      size="30px"
      :class="[
        'q-ma-sm',
        {
          'bg-blue-grey-2': startButtonStatus?.disabled,
        },
      ]"
      @click="startProcess()"
      :disable="startButtonStatus?.disabled"
    />
    <q-btn
      rounded
      color="white"
      text-color="black"
      icon="stop"
      label="/ Unlock"
      size="30px"
      @click="stopProcess()"
    />
  </div>
</template>

<script setup lang="ts">
import { storeToRefs } from 'pinia';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { usePicturesManagerStore } from 'src/stores/pictures-manager.store';
import { Constants } from 'src/constants';
import { computed } from 'vue';

const scannerManagerStore = useScannerManagerStore();
const picturesManagerStore = usePicturesManagerStore();

const { scannerStatus } = storeToRefs(scannerManagerStore);
const { snapShotsDirectory } = storeToRefs(picturesManagerStore);

const getStartButtonStatus = () => {
  if (scannerStatus.value === Constants.SCANNER_STATUS.READY) {
    if (!snapShotsDirectory.value)
      return {
        icon: 'play_arrow',
        disabled: true,
      };
    else {
      return {
        icon: 'play_arrow',
        disabled: false,
      };
    }
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.DISCONNECT) {
    return {
      icon: 'play_arrow',
      disabled: true,
    };
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.RUN) {
    return {
      icon: 'pause',
      disabled: false,
    };
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.PAUSE) {
    return {
      icon: 'mdi-play-pause',
      disabled: false,
    };
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.SNAPSHOT) {
    return {
      icon: 'mdi-play-pause',
      disabled: true,
    };
  } else {
    return {
      icon: 'play_arrow',
      disabled: true,
    };
  }
};

const startButtonStatus = computed(() => getStartButtonStatus());

const stopProcess = () => {
  scannerManagerStore.stopScanning();
};

const startProcess = async () => {
  if (scannerStatus.value === Constants.SCANNER_STATUS.READY) {
    scannerManagerStore.startScanning();
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.RUN) {
    scannerManagerStore.pauseScanning();
  } else if (scannerStatus.value === Constants.SCANNER_STATUS.PAUSE) {
    scannerManagerStore.resumeScanning();
  }
};
</script>
