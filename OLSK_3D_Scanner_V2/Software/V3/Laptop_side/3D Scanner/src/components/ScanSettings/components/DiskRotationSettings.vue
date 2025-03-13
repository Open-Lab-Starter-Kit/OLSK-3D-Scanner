<template>
  <div class="column full-width q-gutter-y-sm">
    <span class="text-size bg-grey-4">Rotation Settings</span>
    <div class="column q-pt-md q-gutter-y-md">
      <div class="row items-center">
        <span class="subtext-size">Angle:</span>
        <span style="font-size: large">{{ rotationAngle }}°</span>
      </div>
      <div class="row items-center">
        <span class="subtext-size">Segments:</span>

        <q-slider
          v-model="scannerSettings.rotationSettings.steps"
          color="deep-purple-5"
          markers
          :marker-labels="(val: number) => `${2 * val}`"
          snap
          :min="1"
          :max="15"
          :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
        />
      </div>

      <div class="row items-center">
        <span class="subtext-size">Speed:</span>

        <q-slider
          v-model="scannerSettings.rotationSettings.speed"
          color="pink-14"
          :min="1"
          :max="1000"
          label
          :label-value="scannerSettings.rotationSettings.speed + ' mm/s'"
          label-always
          switch-label-side
          :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
        />
      </div>
    </div>
  </div>
</template>
<script setup lang="ts">
import { computed } from 'vue';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { storeToRefs } from 'pinia';
import { Constants } from 'src/constants';

const scannerManagerStore = useScannerManagerStore();

const { scannerSettings, scannerStatus } = storeToRefs(scannerManagerStore);

const rotationAngle = computed(() =>
  (360 / (2 * scannerSettings.value.rotationSettings.steps)).toFixed(2)
);
</script>
<style scoped>
.text-size {
  font-size: large;
  font-weight: bold;
  padding: 10px;
  margin: 0;
}
.subtext-size {
  font-size: large;
  font-weight: bold;
  padding-right: 10px;
}
</style>
