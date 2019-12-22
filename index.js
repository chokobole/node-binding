const path = require("path");

const nodeAddonApi = require("node-addon-api");

module.exports = {
  include: [
    '"' + path.join(__dirname) + '"',
    nodeAddonApi.include
  ].join(" ")
};
