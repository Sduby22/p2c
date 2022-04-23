<template>
  <el-row>
    <el-col :span="11">
      <p>Pascal-S Code</p>
      <v-ace-editor
        v-model:value="pascalCode"
        lang="pascal"
        theme="chrome"
        style="height: 600px"
      />
    </el-col>
    <el-col :span="2">
      <div class="control-section">
        <div class="control-box">
          <el-button
            class="convert-btn"
            type="primary"
            :loading="loading"
            circle
            :icon="Right"
            @click="compile"
          />
          <el-checkbox
            v-model="autoCompile"
            label="Auto"
            size="large"
          />
        </div>
      </div>
    </el-col>
    <el-col :span="11">
      <p>C Code (Read-only)</p>
      <v-ace-editor
        v-model:value="cCode"
        lang="c_cpp"
        theme="chrome"
        style="height: 600px"
        :readonly="true"
      />
    </el-col>
  </el-row>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { VAceEditor } from 'vue3-ace-editor';
import 'ace-builds/src-noconflict/mode-c_cpp';
import 'ace-builds/src-noconflict/mode-pascal';
import 'ace-builds/src-noconflict/theme-chrome';
import { Right } from '@element-plus/icons-vue';
import Module from '/@/wasm/libparser';
import { ElMessage } from 'element-plus';
import { Md5 } from 'ts-md5';

const pascalCode = ref('');
const cCode = ref('');
const loading = ref(false);
const autoCompile = ref(true);

let lastHash = '';

let module: any = null;
new (Module as any)().then((m: any) => {
  module = m;
});

setInterval(update, 1000);

function compile() {
  const nowHash = Md5.hashStr(pascalCode.value);
  if (nowHash.localeCompare(lastHash) == 0) {
    ElMessage('No change.');
    return;
  }
  loading.value = true;
  const result = translate(pascalCode.value);
  if (result.success === false) {
    ElMessage.error('Conversion failed.');
  } else {
    cCode.value = result.c_code;
  }
  loading.value = false;
}

function update() {
  if (!autoCompile.value) {
    return;
  }

  const nowHash = Md5.hashStr(pascalCode.value);
  if (nowHash.localeCompare(lastHash) == 0) {
    return;
  }
  lastHash = nowHash;
  const result = translate(pascalCode.value);
  if (result.success === false) {
    return;
  }
  cCode.value = result.c_code;
}

function translate(pas: string) {
  return module.parse(pas);
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin: 30px auto;
  max-width: 1100px;
  min-width: 800px;
}
.control-section {
  width: 100%;
  height: 100%;
  display: flex;
  flex-flow: column;
  justify-content: center;
  align-items: center;
}
.control-box {
  width: 100%;
  display: flex;
  flex-flow: column;
  justify-content: center;
  align-items: center;
}
.control-box::before {
  content: " ";
  width: 100%;
  height: 40px;
}
</style>
