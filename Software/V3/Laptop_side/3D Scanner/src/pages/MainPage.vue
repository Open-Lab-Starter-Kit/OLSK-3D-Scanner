<template>
  <div class="row full-screen">
    <div
      class="column shadow-5 q-px-xl items-center justify-evenly bg-grey-4 col-md col-sm-12 col-xs-12"
    >
      <company-logo />
      <scanner-status />
      <camera-stream />
      <scan-controls />
    </div>
    <div class="column col q-px-xl q-py-md col-md col-sm-12 col-xs-12">
      <scan-settings />
    </div>
  </div>
</template>

<script setup lang="ts">
import CompanyLogo from 'src/components/CompanyLogo.vue';
import CameraStream from 'src/components/CameraStream.vue';
import ScanSettings from 'src/components/ScanSettings/ScanSettings.vue';
import ScanControls from 'src/components/ScanControls.vue';
import ScannerStatus from 'src/components/ScannerStatus.vue';
</script>
<style scoped>
.full-screen {
  min-height: 100vh;
  min-width: 100vw;
}
</style>
