import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { nodePolyfills } from "vite-plugin-node-polyfills";

export default defineConfig({
  plugins: [react(), nodePolyfills()],
  resolve: {
    // Work around broken aws-amplify packaging
    alias: {
      "./runtimeConfig": "./runtimeConfig.browser",
    },
  },
});
