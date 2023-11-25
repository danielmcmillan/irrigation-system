/** @type {import('ts-jest').JestConfigWithTsJest} */
module.exports = {
  preset: "ts-jest/presets/default-esm",
  moduleNameMapper: {
    "^(\\.{1,2}/.*)\\.js$": "$1",
  },
  transform: {
    "^.+\\.tsx?$": [
      "ts-jest",
      {
        useESM: true,
      },
    ],
  },
};

process.env.VAPID_SUBJECT = "mailto:test@vapid.com";
process.env.VAPID_PUBLIC_KEY =
  "BD2VFNG5X-LuguEJbtHsVrvehDLypftKyVbTVBskcSDRLwW-39xba2-Xe7E9otBUEC3Elog92WR7ubVTkxFJkqI";
process.env.VAPID_PRIVATE_KEY = "BsuoiI2eI2M6g3isysKe9lZb_fFuM32p_ZoGli3nndM";
