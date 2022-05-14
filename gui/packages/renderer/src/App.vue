<template>
  <button @click="openFile">open</button>
  <button @click="saveFile">save</button>
  <div class="flex-container">
    <el-row class="editor">
      <el-col 
        class="editor-col"
        :span="11"
      >
        <p>Pascal-S Code</p>
        <v-ace-editor
          v-model:value="pascalCode"
          class="editor-part"
          lang="pascal"
          theme="chrome"
          style="height: 100%"
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
      <el-col
        class="editor-col"
        :span="11"
      >
        <p>C Code (Read-only)</p>
        <v-ace-editor
          v-model:value="cCode"
          class="editor-part"
          lang="c_cpp"
          theme="chrome"
          style="height: 100%"
          :readonly="true"
        />
      </el-col>
    </el-row>
    <div class="info">
      <el-collapse accordion>
        <el-collapse-item name="1">
          <template #title>
            <span class="info-title">Output&nbsp;<el-icon class="header-icon">
              <info-filled
                v-if="hasError"
                color="grey"
              />
              <warning-filled
                v-else
                color="#E55B5B"
              />
            </el-icon>
            </span>
          </template>
          <div>
            <el-input
              v-model="output"
              :rows="10"
              type="textarea"
              placeholder="no output"
              readonly
            />
          </div>
        </el-collapse-item>
      </el-collapse>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { ref } from 'vue';
import { VAceEditor } from 'vue3-ace-editor';
import 'ace-builds/src-noconflict/mode-c_cpp';
import 'ace-builds/src-noconflict/mode-pascal';
import 'ace-builds/src-noconflict/theme-chrome';
import { Right, WarningFilled, InfoFilled } from '@element-plus/icons-vue';
import Module from '/@/wasm/libparser';
import { ElMessage } from 'element-plus';
import { Md5 } from 'ts-md5';

const pascalCode = ref('');
const cCode = ref('');
const loading = ref(false);
const autoCompile = ref(true);
const hasError = ref(false);
const output = ref('');

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
  lastHash = nowHash;
  loading.value = true;
  const result = translate(pascalCode.value);
  if (result.success === false) {
    ElMessage.error('Conversion failed.');
  } else {
    ElMessage.success('Succeed.');
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
  const res = module.parse(pas);
  console.log(res);
  output.value = res.message;
  hasError.value = res.success;
  return res;
}

function openFile() {
  window.files.open({
    properties: ['openFile'],
    filters: [
      {
        name: 'Pascal',
        extensions: ['pas', 'p'],
      },
    ],
  }).then(res => {
    pascalCode.value = res
  });
}

function saveFile() {
  window.files.save(cCode.value, {
    defaultPath: 'a.c',
    filters: [
      {
        name: 'C',
        extensions: ['c'],
      },
    ],
  })
}
</script>

<style>
html, body {
  display: flex;
  flex-direction: column;
  align-items: center;
  width: 100%;
  height: 100%;
  margin: 0;
  padding: 0;
}
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  width: 100%;
  height: 100%;
}
.flex-container {
  display: flex;
  align-items: center;
  width: 100%;
  height: 100%;
  flex-direction: column;
}
.editor {
  display: flex;
  justify-content: center;
  width: calc(100% - 10vw);
  flex: 1;
}
.editor-col {
  display: flex;
  flex-direction: column;
  width: 100%;
  height: 100%;
}
.editor-par {
  flex: 1;
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
.info {
  width: 100vw;
  margin-top: 20px;
  z-index: 10;
}
.info-title {
  display: flex;
  align-items: center;
  margin-left: 20px;
}
</style>
