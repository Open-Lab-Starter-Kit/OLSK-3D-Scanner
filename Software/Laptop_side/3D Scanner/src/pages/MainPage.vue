<template>
  <div class="row full-screen">
    <div
      class="column shadow-5 q-px-md items-center justify-evenly bg-grey-4 col-md col-sm-12 col-xs-12"
    >
      <company-logo />
      <scanner-status />
      <camera-stream />
      <scan-controls />
    </div>
    <div
      class="column col q-px-md items-center justify-evenly col-md col-sm-12 col-xs-12"
    >
      <pictures-path-input />
      <images-format />
      <disk-rotation-settings />
      <arc-steps />
      <led-color />
    </div>
  </div>
</template>

<script setup lang="ts">
import CompanyLogo from 'src/components/CompanyLogo.vue';
import CameraStream from 'src/components/CameraStream.vue';
import PicturesPathInput from 'src/components/PicturesPathInput.vue';
import ImagesFormat from 'src/components/ImagesFormat.vue';
import DiskRotationSettings from 'src/components/DiskRotationSettings.vue';
import ArcSteps from 'src/components/ArcSteps.vue';
import LedColor from 'src/components/LedColor.vue';
import ScanControls from 'src/components/ScanControls.vue';
import ScannerStatus from 'src/components/ScannerStatus.vue';
</script>
<style scoped>
.full-screen {
  min-height: 100vh;
  min-width: 100vw;
}
</style>
