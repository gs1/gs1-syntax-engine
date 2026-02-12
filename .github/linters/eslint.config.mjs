import jsoncParser from "jsonc-eslint-parser";
import tsParser from "@typescript-eslint/parser";

export default [
  {
    files: ["**/*.json"],
    languageOptions: {
      parser: jsoncParser,
      parserOptions: {
        jsonSyntax: "JSON",
      },
    },
  },
  {
    files: ["**/*.ts"],
    languageOptions: {
      parser: tsParser,
    },
  },
  {
    // Declaration files are generated; parse but don't lint
    files: ["**/*.d.ts"],
    languageOptions: {
      parser: tsParser,
    },
    rules: {},
  },
  {
    files: ["maintenance/ci/js-wasm/**/*.ts"],
    rules: {
      // Type-checking test files intentionally assign values to verify types compile
      "no-unused-vars": "off",
    },
  },
];
