
const HtmlWebPackPlugin = require('html-webpack-plugin')

module.exports = {
    mode: "development",
    entry: "./data/server",
    devServer: {
        contentBase: "./data/server",
    },
  
}