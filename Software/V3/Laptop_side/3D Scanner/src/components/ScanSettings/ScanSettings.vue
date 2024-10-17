<template>
  <div class="row fit items-center">
    <pictures-path-input />
    <disk-rotation-settings />
    <arc-steps />
    <led-color />
  </div>
</template>
<script setup lang="ts">
import PicturesPathInput from './components/PicturesPathInput.vue';
import DiskRotationSettings from './components/DiskRotationSettings.vue';
import ArcSteps from './components/ArcSteps.vue';
import LedColor from './components/LedColor.vue';
</script>
