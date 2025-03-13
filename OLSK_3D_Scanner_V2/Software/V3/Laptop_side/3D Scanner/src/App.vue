<template>
  <router-view />
</template>

<script setup lang="ts">
import { onMounted } from 'vue';
import { useSerialConnectionStore } from 'src/stores/serial-connection.store';

const serialConnectionStore = useSerialConnectionStore();

onMounted(() => {
  serialConnectionStore.startSerialConnection();
});
</script>
