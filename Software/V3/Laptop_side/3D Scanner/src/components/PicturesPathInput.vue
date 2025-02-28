<template>
  <div class="column full-width q-gutter-y-sm">
    <span class="row text-size bg-grey-4">Pictures Folder Path</span>
    <q-input
      outlined
      v-model="snapShotsDirectory"
      :onclick="picturesManagerStore.pickDirectory"
      class="row col"
      color="grey-4"
      error-message="Please Enter Pictures Folder Path"
      :error="!snapShotsDirectory"
      :disable="scannerStatus !== Constants.SCANNER_STATUS.READY"
    >
      <template v-slot:append>
        <q-icon name="folder" color="grey-6" />
      </template>
    </q-input>
  </div>
  <q-separator />
</template>
<script setup lang="ts">
import { storeToRefs } from 'pinia';
import { usePicturesManagerStore } from 'src/stores/pictures-manager.store';
import { useScannerManagerStore } from 'src/stores/scanner-manager.store';

import { Constants } from 'src/constants';

const picturesManagerStore = usePicturesManagerStore();

const { snapShotsDirectory } = storeToRefs(picturesManagerStore);
const scannerManagerStore = useScannerManagerStore();

const { scannerStatus } = storeToRefs(scannerManagerStore);
</script>
<style scoped>
.text-size {
  font-size: large;
  font-weight: bold;
  padding: 10px;
}
</style>
