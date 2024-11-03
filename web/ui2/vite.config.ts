import react from "@vitejs/plugin-react";
import { defineConfig } from "vite";
import { VitePWA } from "vite-plugin-pwa";
import { nodePolyfills } from "vite-plugin-node-polyfills";

export default defineConfig({
  plugins: [
    react(),
    VitePWA({
      registerType: "autoUpdate",
      manifest: {
        name: "Irrigation System",
        short_name: process.env.VITE_TITLE ?? "Irrigation",
        icons: [
          {
            src: "favicon.ico",
            sizes: "40x40",
            type: "image/x-icon",
          },
          {
            src: "logo192.png",
            type: "image/png",
            sizes: "192x192",
          },
        ],
        theme_color: "#000000",
        background_color: "#ffffff",
      },
      strategies: "injectManifest",
      srcDir: "src",
      filename: "serviceWorker.ts",
      minify: false,
      devOptions: {
        enabled: false,
      },
    }),
    nodePolyfills(),
  ],
  resolve: {
    // Work around broken aws-amplify packaging
    alias: {
      "./runtimeConfig": "./runtimeConfig.browser",
    },
  },
  server: {
    hmr: process.env.NO_HMR === undefined,
  },
});
