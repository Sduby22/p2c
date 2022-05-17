import { ipcRenderer, OpenDialogSyncOptions, SaveDialogSyncOptions } from "electron";
import { exposeInMainWorld } from "./exposeInMainWorld";

export const files = {
    open: (options: OpenDialogSyncOptions) => {
        return ipcRenderer.invoke('open-file-dialog', options);
    },

    save: (str: string, options: SaveDialogSyncOptions) => {
        return ipcRenderer.invoke('save-file-dialog', str, options);
    }
}

exposeInMainWorld("files", files);
