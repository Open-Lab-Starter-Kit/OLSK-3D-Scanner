<template>
  <div class="column full-width q-gutter-y-sm">
    <span class="row text-size bg-grey-4">Images Format</span>
    <q-option-group
      class="row q-gutter-xl options"
      :options="Constants.IMAGES_FORMATE"
      type="radio"
      v-model="imageFormat"
    />
  </div>
  <q-separator />
</template>
<script setup lang="ts">
import { storeToRefs } from 'pinia';
import { usePicturesManagerStore } from 'src/stores/pictures-manager.store';
import { Constants } from 'src/constants';

const picturesManagerStore = usePicturesManagerStore();
const { imageFormat } = storeToRefs(picturesManagerStore);
</script>
<style scoped>
.text-size {
  font-size: large;
  font-weight: bold;
  padding: 10px;
}
.options {
  font-size: medium;
}
</style>
