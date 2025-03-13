<template>
  <div class="column full-width q-gutter-y-sm">
    <span class="text-size bg-grey-4">LED Color Settings</span>
    <q-card class="row full-width">
      <q-btn-toggle
        v-model="ledOption"
        toggle-color="brown"
        color="grey-4"
        text-color="brown"
        unelevated
        spread
        :options="Constants.LED_OPTIONS"
        class="full-width"
      />
      <div
        v-if="ledOption === Constants.LED_OPTIONS.at(0)?.value"
        class="column full-width"
      >
        <q-toggle
          v-model="isSidesLedOn"
          checked-icon="lightbulb"
          color="green"
          unchecked-icon="lightbulb"
          left-label
          class="q-px-md items-center justify-between"
        >
          <span class="text-h6">LED Status:</span>
        </q-toggle>

        <q-color
          v-model="sidesLedColor"
          no-header
          flat
          format-model="rgba"
          default-view="tune"
          class="my-picker"
          :disable="
            scannerStatus !== Constants.SCANNER_STATUS.READY || !isSidesLedOn
          "
        />
      </div>
      <div
        v-if="ledOption === Constants.LED_OPTIONS.at(1)?.value"
        class="column full-width"
      >
        <q-toggle
          v-model="isRingLedOn"
          checked-icon="lightbulb"
          color="green"
          unchecked-icon="lightbulb"
          left-label
          class="q-px-md items-center justify-between"
        >
          <span class="text-h6">LED Status:</span>
        </q-toggle>

        <q-color
          v-model="ringLedColor"
          no-header
          flat
          format-model="rgba"
          default-view="tune"
          class="my-picker"
          :disable="
            scannerStatus !== Constants.SCANNER_STATUS.READY || !isRingLedOn
          "
        />
      </div>
      <div
        v-if="ledOption === Constants.LED_OPTIONS.at(2)?.value"
        class="column full-width"
      >
        <q-color
          v-if="isSidesLedOn || isRingLedOn"
          v-model="bothLedColor"
          no-header
          flat
          format-model="rgba"
          default-view="tune"
          class="my-picker"
          :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
        />
        <p v-else class="q-pa-sm text-h6 text-center">
          Please, turn on at least one led.
        </p>
      </div>
    </q-card>
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
const sidesLedColor = ref<string>(
  `rgba(${scannerSettings.value.ledMetrics.red}, ${scannerSettings.value.ledMetrics.green}, ${scannerSettings.value.ledMetrics.blue}, ${scannerSettings.value.ledMetrics.intensity})`
);
const ringLedColor = ref<string>(
  `rgba(${scannerSettings.value.ledMetrics.red}, ${scannerSettings.value.ledMetrics.green}, ${scannerSettings.value.ledMetrics.blue}, ${scannerSettings.value.ledMetrics.intensity})`
);
const bothLedColor = ref<string>(
  `rgba(${scannerSettings.value.ledMetrics.red}, ${scannerSettings.value.ledMetrics.green}, ${scannerSettings.value.ledMetrics.blue}, ${scannerSettings.value.ledMetrics.intensity})`
);

const ledOption = ref<string>(Constants.LED_OPTIONS.at(0)?.value ?? '');

const isSidesLedOn = ref<true>(true);
const isRingLedOn = ref<boolean>(true);

// Watch for changes in the color picker and update the store
watch([sidesLedColor, isSidesLedOn], ([newSideLedColor, newIsSidesLedOn]) => {
  scannerManagerStore.setupSideLEDColor(newSideLedColor, newIsSidesLedOn);
});

watch([ringLedColor, isRingLedOn], ([newRingLedColor, newIsRingLedOn]) => {
  scannerManagerStore.setupRingLEDColor(newRingLedColor, newIsRingLedOn);
});

watch(bothLedColor, (newLedColor) => {
  sidesLedColor.value = ringLedColor.value = newLedColor;
  scannerManagerStore.setupSideLEDColor(newLedColor, isSidesLedOn.value);
  scannerManagerStore.setupRingLEDColor(newLedColor, isRingLedOn.value);
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
  font-size: large;
}
</style>
