<template>
  <div class="column full-width q-gutter-y-sm">
    <span class="text-size bg-grey-4">LED Color Settings</span>
    <q-color
      v-model="ledColor"
      no-header
      flat
      format-model="rgba"
      default-view="tune"
      class="my-picker"
      :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
    />
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from 'vue';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';
import { storeToRefs } from 'pinia';
import { Constants } from 'src/constants';

const scannerManagerStore = useScannerManagerStore();
const { scannerSettings, scannerStatus } = storeToRefs(scannerManagerStore);

// Initialize the color with the values from the store
const ledColor = ref<string>(
  `rgba(${scannerSettings.value.ledMetrics.red}, ${scannerSettings.value.ledMetrics.green}, ${scannerSettings.value.ledMetrics.blue}, ${scannerSettings.value.ledMetrics.intensity})`
);

// Function to extract color values from an RGBA string
const getColorValues = (rgbaString: string) => {
  const rgbaPattern =
    /^rgba?\((\d+),\s*(\d+),\s*(\d+),\s*([0-9]*\.?[0-9]+)?\)$/;
  const matches = rgbaString.match(rgbaPattern);

  if (matches) {
    const red = parseInt(matches[1]);
    const green = parseInt(matches[2]);
    const blue = parseInt(matches[3]);
    const intensity = Math.round(parseFloat(matches[4]) * 100);

    return { red, green, blue, intensity };
  } else {
    throw new Error('Invalid RGBA format');
  }
};

// Watch for changes in the color picker and update the store
watch(ledColor, (newLedColor) => {
  scannerSettings.value.ledMetrics = getColorValues(newLedColor);
  scannerManagerStore.setupLEDColor();
});
</script>
<style scoped>
.text-size {
  font-size: large;
  font-weight: bold;
  padding: 10px;
}
.my-picker {
  min-width: 100%;
  color: black;
  border: 2px solid #d7ccc8;
  font-size: large;
}
</style>
