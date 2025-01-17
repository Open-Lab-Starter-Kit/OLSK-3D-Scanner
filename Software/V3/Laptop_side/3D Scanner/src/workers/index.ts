import serialConnectionWorker from 'src/workers/serialConnection.worker?worker&inline';

export const getSerialConnectionWorker = () => {
  return new serialConnectionWorker();
};
