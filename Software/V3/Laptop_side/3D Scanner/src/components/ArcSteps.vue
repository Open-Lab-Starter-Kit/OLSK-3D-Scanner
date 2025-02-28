<template>
  <div class="row full-width q-gutter-y-sm">
    <span class="text-size full-width bg-grey-4">Pictures per step</span>

    <q-slider
      v-model="scannerSettings.arcSteps"
      color="amber-8"
      markers
      :marker-labels="(val) => val.toString()"
      snap
      :min="1"
      :max="30"
      :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
    />
  </div>
</template>
<script setup lang="ts">
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { storeToRefs } from 'pinia';
import { Constants } from 'src/constants';

const scannerManagerStore = useScannerManagerStore();

const { scannerSettings, scannerStatus } = storeToRefs(scannerManagerStore);
</script>
<style scoped>
.text-size {
  font-size: large;
  font-weight: bold;
  padding: 10px;
}
</style>
