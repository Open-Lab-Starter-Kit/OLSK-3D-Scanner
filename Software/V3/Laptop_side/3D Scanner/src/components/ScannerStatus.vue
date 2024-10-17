<template>
  <div
    :class="[
      'column',
      'text-h4',
      'q-pa-lg',
      'flex-center',
      'full-width',
      'shadow-2',
      { 'text-white': scannerStatus !== Constants.SCANNER_STATUS.DISCONNECT },
      {
        'bg-light-blue-3': scannerStatus === Constants.SCANNER_STATUS.READY,
        'bg-orange-12': scannerStatus === Constants.SCANNER_STATUS.SNAPSHOT,
        'bg-light-green-5': scannerStatus === Constants.SCANNER_STATUS.RUN,
        'bg-yellow-8': scannerStatus === Constants.SCANNER_STATUS.PAUSE,
        'bg-white': scannerStatus === Constants.SCANNER_STATUS.DISCONNECT,
      },
    ]"
    style="border-radius: 10px; border: 2px solid white"
  >
    <span class="state-text">{{ scannerStatus }}</span>
  </div>
</template>
<script setup lang="ts">
import { storeToRefs } from 'pinia';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { Constants } from 'src/constants';

const scannerManagerStore = useScannerManagerStore();

const { scannerStatus } = storeToRefs(scannerManagerStore);
</script>
